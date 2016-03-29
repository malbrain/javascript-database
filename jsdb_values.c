#ifdef _WIN32
#include <windows.h>
#include <intrin.h>
#endif

#include "jsdb.h"
#include "jsdb_db.h"

//	decrement value_t reference counter
//	return true if goes to zero

bool decrRefCnt (value_t val) {
	if (val.refcount)
#ifndef _WIN32
		return !__sync_fetch_and_add(val.raw[-1].refCnt, -1);
#else
		return !InterlockedDecrement64(val.raw[-1].refCnt);
#endif
	if (val.weakcount)
#ifndef _WIN32
		return !__sync_fetch_and_add(val.raw[-1].weakCnt, -1);
#else
		return !InterlockedDecrement64(val.raw[-1].weakCnt);
#endif
	return false;
}

void incrRefCnt (value_t val) {

	if (val.refcount)
#ifndef _WIN32
		__sync_fetch_and_add(val.raw[-1].refCnt, 1);
#else
		InterlockedIncrement64(val.raw[-1].refCnt);
#endif
		return;

	if (val.weakcount)
#ifndef _WIN32
		__sync_fetch_and_add(val.raw[-1].weakCnt, 1);
#else
		InterlockedIncrement64(val.raw[-1].weakCnt);
#endif
		return;
}

uint64_t totalRefCnt (void *obj) {
rawobj_t *raw = obj;

	return *raw[-1].refCnt + *raw[-1].weakCnt;
}

void jsdb_free (void *obj) {
rawobj_t *raw = obj;

	 free(raw - 1);
}

void *jsdb_alloc(uint32_t len, bool zero) {
rawobj_t *mem = malloc(sizeof(rawobj_t) + len);

	if (!mem) {
		 fprintf (stderr, "out of memory!\n");
		 exit(1);
	}

	if (zero)
		memset(mem + 1, 0, len);

	mem->refCnt[0] = 0;
	mem->weakCnt[0] = 0;
	return mem + 1;
}

// delete values

void deleteValue(value_t val) {
	switch (val.type) {
//	case vt_handle:  close the handle

	case vt_string: {
		jsdb_free(val.raw);
		break;
	}
	case vt_closure: {
		for (int i = 0; i < val.closure->count; i++)
			abandonFrame(val.closure->frames[i]);

		jsdb_free(val.raw);
		break;
	}
	case vt_array: {
		for (int i=0; i< vec_count(val.aval->array); i++)
			if (decrRefCnt(val.aval->array[i]))
				deleteValue(val.aval->array[i]);

		vec_free(val.aval->array);
		jsdb_free(val.raw);
		break;
	}
	case vt_object: {
		for (int i=0; i< vec_count(val.oval->names); i++) {
			if (decrRefCnt(val.oval->values[i]))
				deleteValue(val.oval->values[i]);
			if (decrRefCnt(val.oval->names[i]))
				deleteValue(val.oval->names[i]);
		}

		vec_free(val.oval->values);
		vec_free(val.oval->names);
		free(val.oval->hash);
		jsdb_free(val.raw);
		break;
	}
	case vt_file: {
		fclose(val.file);
		break;
	}
	default:;
	}
}

int value2Str(value_t v, value_t **array, int depth) {
	value_t indent;
	int len;

	indent.bits = vt_string;
	indent.aux = depth * 2;
	indent.str = "                    ";

	if (indent.aux > strlen(indent.str) - 2)
		indent.aux = strlen(indent.str) - 2;

	switch(v.type) {
	case vt_string: {
		value_t quot;

		quot.bits = vt_string;
		quot.str = "\"";
		quot.aux = 1;
		if (!depth)
			return vec_push(*array, v), v.aux;

		vec_push(*array, quot);
		vec_push(*array, v);
		vec_push(*array, quot);
		return v.aux + 2 * quot.aux;
	}

	default: {
		value_t val = conv2Str(v);
		vec_push(*array, val);
		return val.aux;
	}

	case vt_object: {
		value_t toString, *fcn;

		toString.bits = vt_string;
		toString.str = "toString";
		toString.aux = 8;

		fcn = lookup(v, toString, false);

		if (fcn && fcn->type == vt_closure) {
			value_t quot, *arg = NULL;

			vec_push(arg, v);
			v = fcnCall(*fcn, arg, v);
			quot.bits = vt_string;
			quot.str = "\"";
			quot.aux = 1;
			vec_push(*array, quot);
			vec_push(*array, v);
			vec_push(*array, quot);
			return v.aux + 2 * quot.aux;
		}
		}

	case vt_document: {
		value_t colon, prefix, ending, comma;

		if (!vec_count(v.oval->names)) {
			value_t empty;
			empty.str = "{ }\n";
			empty.aux = 4;
			vec_push (*array, empty);
			return empty.aux;
		}

		prefix.bits = vt_string;
		prefix.str = "{\n";
		prefix.aux = 2;

		colon.bits = vt_string;
		colon.str = " : ";
		colon.aux = 3;

		vec_push(*array, prefix);
		len = prefix.aux;

		comma.bits = vt_string;
		indent.aux += 2;

		for (int idx = 0; idx < vec_count(v.oval->names); ) {
			vec_push(*array, indent), len += indent.aux;

			vec_push(*array, v.oval->names[idx]);
			len += v.oval->names[idx].aux;
			vec_push(*array, colon);
			len += colon.aux;

			len += value2Str(v.oval->values[idx], array, depth + 1);

			if (++idx < vec_count(v.oval->names))
				comma.str = ",\n";
			else
				comma.str = "\n";

			comma.aux = strlen(comma.str);
			vec_push(*array, comma), len += comma.aux;
		}

		ending.bits = vt_string;
		ending.str = "}";
		ending.aux = 1;

		indent.aux -= 2;
		vec_push(*array, indent);
		len += indent.aux;

		vec_push(*array, ending);
		return len + ending.aux;
	}

	case vt_docarray:
	case vt_array: {
		value_t prefix, ending, comma;

		vec_push(*array, indent);
		len = indent.aux;

		prefix.bits = vt_string;
		prefix.str = " [ ";
		prefix.aux = 3;
		vec_push(*array, prefix);

		comma.bits = vt_string;
		comma.str = ", ";
		comma.aux = 2;

		ending.bits = vt_string;
		ending.str = " ]\n";
		ending.aux = 3;

		vec_push(*array, prefix);
		len += prefix.aux;

		for (int idx = 0; idx < vec_count(v.aval->array); ) {
			len += value2Str(v.aval->array[idx], array, depth + 1);

			if (++idx < vec_count(v.aval->array))
				vec_push(*array, comma), len += comma.aux;
		}

		vec_push(*array, ending);
		return len + ending.aux;
	}
	}
}

// replace value in frame, array, or object

value_t replaceSlotValue(value_t *slot, value_t value) {

	while (slot->type == vt_ref)
		slot = slot->ref;

	incrRefCnt(value);

	if (decrRefCnt(*slot))
		deleteValue(*slot);

	return *slot = value;
}

//  add reference count to frame

void incrFrameCnt(frame_t *frame) {
rawobj_t *raw = (rawobj_t *)frame;

#ifndef _WIN32
	if (__sync_fetch_and_add(raw[-1].refCnt, 1))
		return;
#else
	if (InterlockedIncrement64(raw[-1].refCnt))
		return;
#endif
}

//	abandon frame

void abandonFrame(frame_t *frame) {
rawobj_t *raw = (rawobj_t *)frame;

#ifndef _WIN32
	if (__sync_fetch_and_add(raw[-1].refCnt, -1))
		return;
#else
	if (InterlockedDecrement64(raw[-1].refCnt))
		return;
#endif
	// abandon frame values

	for (int i = 0; i < frame->count; i++)
		if (decrRefCnt(frame->values[i]))
			deleteValue(frame->values[i]);

	jsdb_free(frame);
}

//  abandon value

void abandonValue(value_t val) {
bool del = false;

	if (val.refcount)
		if (*val.raw[-1].refCnt)
			return;
		else
			del = true;

	if (val.weakcount)
		if (*val.raw[-1].weakCnt)
			return;
		else
			del = true;

	if (del)
		deleteValue(val);
}

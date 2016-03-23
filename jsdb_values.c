#ifdef _WIN32
#include <windows.h>
#include <intrin.h>
#endif

#include "jsdb.h"

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

void deleteSlotValue(value_t slot) {
	switch (slot.type) {
	case vt_handle:
	case vt_string: {
		jsdb_free(slot.raw);
		break;
	}
	case vt_closure: {
		for (int i = 0; i < vec_count(slot.closure->frames); i++)
			abandonFrame(slot.closure->frames[i]);

		vec_free(slot.closure->frames);
		jsdb_free(slot.raw);
		break;
	}
	case vt_array: {
		for (int i=0; i< vec_count(slot.aval->array); i++)
			if (decrRefCnt(slot.aval->array[i]))
				deleteSlotValue(slot.aval->array[i]);

		vec_free(slot.aval->array);
		jsdb_free(slot.raw);
		break;
	}
	case vt_object: {
		for (int i=0; i< vec_count(slot.oval->names); i++) {
			if (decrRefCnt(slot.oval->values[i]))
				deleteSlotValue(slot.oval->values[i]);
			if (decrRefCnt(slot.oval->names[i]))
				deleteSlotValue(slot.oval->names[i]);
		}

		vec_free(slot.oval->values);
		vec_free(slot.oval->names);
		free(slot.oval->hash);
		jsdb_free(slot.raw);
		break;
	}
	case vt_file: {
		fclose(slot.file);
		break;
	}
	default:;
	}
}

void printString(value_t str) {
	printf("%.*s", str.aux, str.str);
}

// print values

void printValue(value_t v, uint32_t depth) {
	uint32_t d;

	switch(v.type) {
	case vt_string:
		printf("%.*s", v.aux, v.str);
		break;
	case vt_bool:
		if (v.boolean)
			printf("true");
		else
			printf("false");
		break;
	case vt_int:
		printf("%lld", v.nval);
		break;
	case vt_dbl:
		printf("%G", v.dbl);

		if (v.dbl - (uint64_t)v.dbl)
			break;
		printf(".0");
		break;
	case vt_status: {
		errorText(v.status);
		break;
	}
	case vt_objId: {
		for (int idx = 0; idx < v.aux; idx++)
			printf("%.2x", v.str[idx]);

		break;
	}
	case vt_array: {
		printf("[\n	");

		for (uint32_t i=0; i<vec_count(v.aval->array); i++) {
			if (i>0) printf(", ");
			printValue((v.aval->array)[i], depth+1);
		}
		printf("\n]");
		break;
	}
	case vt_docarray: {
		printf("[\n	");

		for (uint32_t i=0; i<v.docarray->count; i++) {
			value_t val = v.docarray->array[i];

			if (val.rebaseptr)
				val.rebase = v.rebase - v.docarray->base + val.offset;

			if (i>0) printf(", ");
			printValue(val, depth+1);
		}
		printf("\n]");
		break;
	}
	case vt_object: {
		value_t val;
		int i;

		printf("\n");
		for (d=0; d<depth; d++) printf("	");
		printf("{\n");
		for (i = 0; i<vec_count(v.oval->names); i++) {
			for (d=0; d<depth+1; d++) printf("	");
			printString(v.oval->names[i]);
			printf(" : ");
			val = v.oval->values[i];
			if (vt_object == val.type || vt_array == val.type)
				printValue(val, depth+1);
			else 
				printValue(val, 0);
			if (i < vec_count(v.oval->names) - 1)
				printf(",\n");
		}
		printf("\n");
		for (d=0; d<depth; d++) printf("	");
		printf("}");
		break;
	}
	case vt_document: {
		printf("\n");
		for (d=0; d<depth; d++) printf("	");
		printf("{\n");
		int i;
		for (i = 0; i<v.document->count; i++) {
			value_t val = v.document->names[i + v.document->count];
			value_t key = v.document->names[i];

			if (val.rebaseptr)
				val.rebase = v.rebase - v.document->base + val.offset;

			if (key.rebaseptr)
				key.rebase = v.rebase - v.document->base + key.offset;

			for (d=0; d<depth+1; d++) printf("	");
			printString(key);
			printf(" : ");
			if (vt_object == val.type || vt_array == val.type)
				printValue(val, depth+1);
			else 
				printValue(val, 0);
			if (i < v.document->count - 1)
				printf(",\n");
		}
		printf("\n");
		for (d=0; d<depth; d++) printf("	");
		printf("}");
		break;
	}
	case vt_handle: {
		printf("handle");
		break;
	}
	case vt_docId: {
		printf("%llu", v.docId.bits);
		break;
	}
	default:;
	}
}

// replace value in frame, array, or object

value_t replaceSlotValue(value_t *slot, value_t value) {

	while (slot->type == vt_ref)
		slot = slot->ref;

	incrRefCnt(value);

	if (decrRefCnt(*slot))
		deleteSlotValue(*slot);

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
			deleteSlotValue(frame->values[i]);

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
		jsdb_free(val.raw);
}

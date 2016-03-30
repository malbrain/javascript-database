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

static char vt_handle_str[]  = "handle";
static char vt_docid_str[]   = "docid";
static char vt_string_str[]  = "string";
static char vt_int_str[]	 = "int";
static char vt_dbl_str[]	 = "dbl";
static char vt_file_str[]    = "file";
static char vt_status_str[]  = "status";
static char vt_null_str[]    = "null value";
static char vt_undef_str[]    = "undef value";
static char vt_closure_str[] = "function";

static char *ok_str = "OK";
static char *outofmemory_str = "out of memory";
static char *handleclosed_str = "handle closed";
static char *badhandle_str = "bad handle";
static char *badrecid_str = "bad recid";
static char *endoffile_str = "end of file";
static char *notbasever_str = "not base version";
static char *recorddeleted_str = "record deleted";
static char *recordnotvisible_str = "record not visible";
static char *notcurrentversion_str = "not current version";
static char *cursornotpositioned_str = "cursor not positioned";
static char *invaliddeleterecord_str = "invalid delete record";
static char *cursorbasekeyerror_str = "cursor basekey error";
static char *writeconflict_str = "write conflict error";
static char *duplicatekey_str = "duplicate key";
static char *keynotfound_str = "key not found ";
static char *badtxnstep_str = "bad txn step";
static char *arena_already_closed_str = "arena already closed";
static char *unrecognized_str = "unrecognized";
static char *script_internal_str = "script internal error";
static char *script_unrecognized_function_str = "script unrecognized function";

char *strtype(valuetype_t t) {
	switch (t) {
	case vt_handle: return vt_handle_str;
	case vt_docId: return vt_docid_str;
	case vt_string: return vt_string_str;
	case vt_int: return vt_int_str;
	case vt_dbl: return vt_dbl_str;
	case vt_file: return vt_file_str;
	case vt_status: return vt_status_str;
	case vt_null: return vt_null_str;
	case vt_undef: return vt_undef_str;
	case vt_closure: return vt_closure_str;
	default:;
	}
	return unrecognized_str;
}

char *strstatus(Status s) {
	switch (s) {
	case OK: return ok_str;
	case ERROR_outofmemory: return outofmemory_str;
	case ERROR_handleclosed: return handleclosed_str;
	case ERROR_badhandle: return badhandle_str;
	case ERROR_badrecid: return badrecid_str;
	case ERROR_endoffile: return endoffile_str;
	case ERROR_notbasever: return notbasever_str;
	case ERROR_recorddeleted: return recorddeleted_str;
	case ERROR_recordnotvisible: return recordnotvisible_str;
	case ERROR_notcurrentversion: return notcurrentversion_str;
	case ERROR_cursornotpositioned: return cursornotpositioned_str;
	case ERROR_invaliddeleterecord: return invaliddeleterecord_str;
	case ERROR_cursorbasekeyerror: return cursorbasekeyerror_str;
	case ERROR_writeconflict: return writeconflict_str;
	case ERROR_duplicatekey: return duplicatekey_str;
	case ERROR_keynotfound: return keynotfound_str;
	case ERROR_badtxnstep: return badtxnstep_str;
	case ERROR_arena_already_closed: return arena_already_closed_str;
	case ERROR_script_internal: return script_internal_str;
	case ERROR_script_unrecognized_function: return script_unrecognized_function_str;
	default:;
	}
	return unrecognized_str;
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

		comma.bits = vt_string;
		comma.str = ", ";
		comma.aux = 2;

		ending.bits = vt_string;
		ending.str = " ]\n";
		ending.aux = 3;

		prefix.bits = vt_string;
		prefix.str = "[ ";
		prefix.aux = 2;

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

value_t conv2Bool(value_t cond) {
	value_t result;

	result.bits = vt_bool;

	switch (cond.type) {
	case vt_dbl: result.boolean = cond.dbl != 0; return result;
	case vt_int: result.boolean = cond.nval != 0; return result;
	case vt_status: result.boolean = cond.status == OK; return result;
	case vt_file: result.boolean = cond.file != NULL; return result;
	case vt_array: result.boolean = cond.aval != NULL; return result;
	case vt_object: result.boolean = cond.oval != NULL; return result;
	case vt_handle: result.boolean = cond.hndl != NULL; return result;
	case vt_document: result.boolean = cond.document != NULL; return result;
	case vt_docarray: result.boolean = cond.docarray != NULL; return result;
	case vt_string: result.boolean = cond.aux > 0; return result;
	case vt_closure: result.boolean = cond.closure != NULL; return result;
	case vt_docId: result.boolean = cond.docId.bits > 0; return result;
	case vt_undef: result.boolean = false; return result;
	case vt_null: result.boolean = false; return result;
	case vt_bool: return cond;
	}

	result.boolean = false;
	return result;
}

value_t conv2ObjId(value_t cond) {
	switch (cond.type) {
	case vt_objId:	return cond;
	}

	fprintf(stderr, "Invalid conversion too ObjId: %s\n", strtype(cond.type));
	exit(1);
}

value_t conv2Dbl (value_t val) {
	value_t result;

	result.bits = vt_dbl;

	switch (val.type) {
	case vt_dbl: result.dbl = val.dbl; return result;
	case vt_int: result.dbl = val.nval; return result;
	case vt_bool: result.dbl = val.boolean; return result;
	}

	result.dbl = 0;
	return result;
}

value_t conv2Int (value_t val) {
	value_t result;

	result.bits = vt_int;

	switch (val.type) {
	case vt_int: result.nval = val.nval; return result;
	case vt_dbl: result.nval = val.dbl; return result;
	case vt_bool: result.nval = val.boolean; return result;
	}

	result.nval = 0;
	return result;
}

value_t conv2Str (value_t val) {
	value_t result;
	char buff[64];
	int len;

	result.bits = vt_string;

	switch (val.type) {
	case vt_endlist:
		result.aux = 0;
		return result;
	case vt_string: return val;
	case vt_int:
#ifndef _WIN32
		len = snprintf(buff, sizeof(buff), "%d", val.nval);
#else
		len = _snprintf_s(buff, sizeof(buff), _TRUNCATE, "%d", val.nval);
#endif
		break;

	case vt_bool:
		if (val.boolean)
			memcpy (buff, "true", len = 4);
		else
			memcpy (buff, "false", len = 5);
		break;

	case vt_dbl:
#ifndef _WIN32
		len = snprintf(buff, sizeof(buff), "%#G", val.dbl);
#else
		len = _snprintf_s(buff, sizeof(buff), _TRUNCATE, "%#G", val.dbl);
#endif
		if (!(val.dbl - (uint64_t)val.dbl))
		  if (len + 2 < sizeof(buff))
			buff[len++] = '.', buff[len++] = '0';

		break;

	default:
#ifndef _WIN32
		len = snprintf(buff, sizeof(buff), "type: %s", strtype(val));
#else
		len = _snprintf_s(buff, sizeof(buff), _TRUNCATE, "type: %s", strtype(val.type));
#endif
		break;
	}

	if (len > sizeof(buff))
		len = sizeof(buff);

	result.str = jsdb_alloc(len, false);
	result.refcount = 1;
	result.aux = len;

	memcpy (result.str, buff, len);
	return result;
}


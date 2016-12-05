#ifdef _WIN32
#include <windows.h>
#include <intrin.h>
#endif

#include <ctype.h>

#include "js.h"
#include "js_malloc.h"
#include "js_dbindex.h"

value_t js_strtod(value_t val);
value_t date2Str(value_t val);

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
	if (val.refcount) {
#ifndef _WIN32
		__sync_fetch_and_add(val.raw[-1].refCnt, 1);
#else
		InterlockedIncrement64(val.raw[-1].refCnt);
#endif
		return;
	}

	if (val.weakcount) {
#ifndef _WIN32
		__sync_fetch_and_add(val.raw[-1].weakCnt, 1);
#else
		InterlockedIncrement64(val.raw[-1].weakCnt);
#endif
		return;
	}
}

uint64_t totalRefCnt (void *obj) {
rawobj_t *raw = obj;

	return *raw[-1].refCnt + *raw[-1].weakCnt;
}

// delete values

void deleteValue(value_t val);

void deleteObj(object_t *obj) {
	for (int i=0; i< vec_count(obj->pairs); i++) {
		if (decrRefCnt(obj->pairs[i].name))
			deleteValue(obj->pairs[i].name);
		if (decrRefCnt(obj->pairs[i].value))
			deleteValue(obj->pairs[i].value);
	}

	if (obj->capacity)
		js_free(obj->hashTbl);

	vec_free(obj->pairs);
}

void deleteValue(value_t val) {
	switch (val.type) {
	case vt_handle:
		js_deleteHandle(val);
		break;

	case vt_string: {
		js_free(val.raw);
		break;
	}
	case vt_closure: {
		for (int i = 0; i < val.closure->count; i++)
			abandonFrame(val.closure->frames[i]);

		deleteObj(val.closure->props);
		js_free(val.raw);
		break;
	}
	case vt_array: {
		for (int i=0; i< vec_count(val.aval->values); i++)
			if (decrRefCnt(val.aval->values[i]))
				deleteValue(val.aval->values[i]);

		if (val.aval->obj->capacity)
			deleteObj(val.aval->obj);

		vec_free(val.aval->values);
		js_free(val.raw);
		break;
	}
	case vt_object: {
		deleteObj(val.oval);
		js_free(val.raw);
		break;
	}
	case vt_file: {
		fclose(val.file);
		break;
	}
	default:;
	}
}

static char vt_handle_str[]		= "handle";
static char vt_docid_str[]		= "docid";
static char vt_txnid_str[]		= "txnid";
static char vt_string_str[]		= "string";
static char vt_int_str[]		= "integer";
static char vt_dbl_str[]		= "number";
static char vt_nan_str[]		= "NaN";
static char vt_inf_str[]		= "infinite";
static char vt_file_str[]		= "file";
static char vt_status_str[]		= "status";
static char vt_object_str[]		= "object";
static char vt_undef_str[]		= "undefined";
static char vt_bool_str[]		= "boolean";
static char vt_closure_str[]	= "function";
static char vt_date_str[]		= "date";
static char vt_objId_str[]		= "objId";
static char vt_document_str[]	= "document";
static char vt_docarray_str[]	= "docarray";
static char vt_unknown_str[]	= "unknown";

char *strtype(valuetype_t t) {
	switch (t) {
	case vt_handle:		return vt_handle_str;
	case vt_docId:		return vt_docid_str;
	case vt_txnId:		return vt_txnid_str;
	case vt_string:		return vt_string_str;
	case vt_int:		return vt_int_str;
	case vt_bool:		return vt_bool_str;
	case vt_dbl:		return vt_dbl_str;
	case vt_file:		return vt_file_str;
	case vt_status:		return vt_status_str;
	case vt_undef:		return vt_undef_str;
	case vt_closure:	return vt_closure_str;
	case vt_infinite:	return vt_inf_str;
	case vt_objId:		return vt_objId_str;
	case vt_date:		return vt_date_str;
	case vt_nan:		return vt_nan_str;
	case vt_document:	return vt_document_str;
	case vt_docarray:	return vt_docarray_str;
	case vt_object:	    return vt_object_str;
	default:;
	}
	return vt_unknown_str;
}

//	convert value to string

value_t value2Str(value_t v, bool raw) {
	value_t ans[1];
	value_t quot;

	switch(v.type) {
	  case vt_string: {
		if (raw)
			return v;

		quot.bits = vt_string;
		quot.str = "\"";
		quot.aux = 1;

		*ans = newString("\"", 1);
		valueCat(ans, v);
		valueCat(ans, quot);
		return *ans;
	  }

	  default: {
		return conv2Str(v, true);
	  }

	  case vt_object: {
		value_t colon, ending, comma;
		value_t toString, *fcn;
		object_t *oval = v.oval;
		uint32_t idx = 0;

		toString.bits = vt_string;
		toString.string = "toString";
		toString.aux = 8;

		fcn = lookup(v.oval, toString, false);

		if (fcn && fcn->type == vt_closure) {
			array_t aval[1];
			value_t arg;

			memset(aval, 0, sizeof(aval));
			vec_push(aval->values, v);

			arg.bits = vt_array;
			arg.aval = aval;

			return fcnCall(*fcn, arg, v);
		}

		ans->bits = vt_string;
		ans->str = "{";
		ans->aux = 1;

		colon.bits = vt_string;
		colon.string = ": ";
		colon.aux = 2;

		comma.bits = vt_string;
		comma.string = ", ";
		comma.aux = 2;

		while (idx < vec_count(oval->pairs)) {
			valueCat(ans, value2Str(oval->pairs[idx].name, raw));
			valueCat(ans, colon);

			valueCat(ans, value2Str(oval->pairs[idx].value, false));

			if (++idx < vec_count(oval->pairs))
				valueCat(ans, comma);
		}

		ending.bits = vt_string;
		ending.string = "}";
		ending.aux = 1;

		valueCat(ans, ending);
		return *ans;
	  }

	  case vt_document: {
		value_t colon, ending, comma;
		document_t *doc = v.document;
		uint32_t idx = 0;

		ans->bits = vt_string;
		ans->string = "{";
		ans->aux = 1;

		colon.bits = vt_string;
		colon.string = " : ";
		colon.aux = 3;

		comma.bits = vt_string;
		comma.string = ",";
		comma.aux = 1;

		while (idx < doc->count) {
			valueCat(ans, getDocName(doc, idx));
			valueCat(ans, colon);

			valueCat(ans, value2Str(getDocValue(doc, idx), false));

			if (++idx < doc->count)
				valueCat(ans, comma);
		}

		ending.bits = vt_string;
		ending.string = "}";
		ending.aux = 1;
		valueCat(ans, ending);
		return *ans;
	  }

	  case vt_docarray: {
		docarray_t *array = v.docarray;
		value_t ending, comma;
		uint32_t idx = 0;

		ans->bits = vt_string;
		ans->string = "[";
		ans->aux = 1;

		comma.bits = vt_string;
		comma.string = ",";
		comma.aux = 1;

		while (idx < array->count) {
			valueCat(ans, value2Str(getDocArray(array, idx), false));

			if (++idx < array->count)
				valueCat(ans, comma);
		}

		ending.bits = vt_string;
		ending.string = "]";
		ending.aux = 1;

		valueCat(ans, ending);
		return *ans;
	  }

	  case vt_array: {
		array_t *aval = v.aval;
		value_t ending, comma;
		uint32_t idx = 0;

		ans->bits = vt_string;
		ans->string = "[";
		ans->aux = 1;

		comma.bits = vt_string;
		comma.string = ",";
		comma.aux = 1;

		while (idx < vec_count(aval->values)) {
			valueCat(ans, value2Str(aval->values[idx], false));

			if (++idx < vec_count(aval->values))
				valueCat(ans, comma);
		}

		ending.bits = vt_string;
		ending.string = "]";
		ending.aux = 1;

		valueCat(ans, ending);
		return *ans;
	  }
	}
}

// replace l-value in frame, array, or object

value_t replaceValue(value_t slot, value_t value) {

	incrRefCnt(value);

	if (slot.type != vt_lval) {
		fprintf(stderr, "Not lvalue: %s\n", strtype(slot.type));
		exit(1);
	}

	if (!slot.subType) {
		if (decrRefCnt(*slot.lval))
			deleteValue(*slot.lval);

		return *slot.lval = value;
	}

	storeArrayValue(slot, value);
	return value;
}

// replace slot value in frame, array, or object

void replaceSlot(value_t *slot, value_t value) {

	incrRefCnt(value);

	if (decrRefCnt(*slot))
		deleteValue(*slot);

	*slot = value;
}

//  add reference count to frame

void incrFrameCnt(frame_t *frame) {
rawobj_t *raw = (rawobj_t *)frame;

#ifndef _WIN32
	__sync_fetch_and_add(raw[-1].refCnt, 1);
#else
	InterlockedIncrement64(raw[-1].refCnt);
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
	//	skipping first value which was rtnValue

	for (int i = 0; i < frame->count; i++)
		if (decrRefCnt(frame->values[i+1]))
			deleteValue(frame->values[i+1]);

	// abandon temporary object

	if (decrRefCnt(frame->nextThis))
		deleteValue(frame->nextThis);

	// abandon argument list

	if (decrRefCnt(frame->arguments))
		deleteValue(frame->arguments);

	js_free(frame);
}

//  abandon value

void abandonValue(value_t val) {
bool del = false;

	if (val.refcount) {
		if (*val.raw[-1].refCnt)
			return;
		else
			del = true;
	}

	if (val.weakcount) {
		if (*val.raw[-1].weakCnt)
			return;
		else
			del = true;
	}

	if (del)
		deleteValue(val);
}

value_t conv2Bool(value_t cond, bool abandon) {
	value_t result;

	result.bits = vt_bool;

	while (true) {
	  if (cond.type == vt_array && cond.aval->obj->base.type)
		cond = cond.aval->obj->base;
	  else if (cond.type == vt_object && cond.oval->base.type)
		cond = cond.oval->base;
	  else
		break;
	}

	result.boolean = false;

	switch (cond.type) {
	case vt_nan: result.boolean = false; break;
	case vt_array: result.boolean = true; break;
	case vt_object: result.boolean = true; break;
	case vt_infinite: result.boolean = true; break;
	case vt_dbl: result.boolean = cond.dbl != 0; break;
	case vt_int: result.boolean = cond.nval != 0; break;
	case vt_status: result.boolean = cond.status == OK; break;
	case vt_file: result.boolean = cond.file != NULL; break;
	case vt_document: result.boolean = cond.document != NULL; break;
	case vt_docarray: result.boolean = cond.docarray != NULL; break;
	case vt_string: result.boolean = cond.aux > 0; break;
	case vt_closure: result.boolean = cond.closure != NULL; break;
	case vt_docId: result.boolean = cond.docBits > 0; break;
	case vt_txnId: result.boolean = cond.txnBits > 0; break;
	case vt_undef: result.boolean = false; break;
	case vt_null: result.boolean = false; break;
	case vt_bool: return cond;
	case vt_handle: {
		result.boolean = cond.handle ? 1 : 0;
		break;
	}
	default: break;
	}

	if (abandon)
		abandonValue(cond);

	return result;
}

value_t conv2ObjId(value_t cond, bool abandon) {
	switch (cond.type) {
	case vt_objId:	return cond;
	default: break;
	}

	fprintf(stderr, "Invalid conversion too ObjId: %s\n", strtype(cond.type));
	exit(1);
}

value_t conv2Dbl (value_t val, bool abandon) {
	value_t result;

	while (true) {
	  if (val.type == vt_array && val.aval->obj->base.type)
		val = val.aval->obj->base;
	  else if (val.type == vt_object && val.oval->base.type)
		val = val.oval->base;
	  else
		break;
	}

	result.bits = vt_dbl;
	result.dbl = 0;

	switch (val.type) {
	case vt_undef:	result.bits = vt_nan; break;
	case vt_dbl:	result.dbl = val.dbl; break;
	case vt_int:	result.dbl = val.nval; break;
	case vt_bool:	result.dbl = val.boolean; break;
	case vt_string: result = js_strtod(val); break;
	default: break;
	}

	if (abandon)
		abandonValue (val);

	return result;
}

value_t conv2Int (value_t val, bool abandon) {
	value_t result;

	while (true) {
	  if (val.type == vt_array && val.aval->obj->base.type)
		val = val.aval->obj->base;
	  else if (val.type == vt_object && val.oval->base.type)
		val = val.oval->base;
	  else
		break;
	}

	result.bits = vt_int;
	result.nval = 0;

	switch (val.type) {
	case vt_undef:	result.bits = vt_nan; break;
	case vt_int:	result.nval = val.nval; break;
	case vt_dbl:	result.nval = val.dbl; break;
	case vt_bool:	result.nval = val.boolean; break;
	case vt_null:	result.nval = 0; break;
	case vt_string: result = js_strtod(val); break;

	case vt_array: {
		int cnt = vec_count(val.aval->values);

		if (cnt>1)
			break;

		if (!cnt)
			return result.nval = 0, result;

		return conv2Int(val.aval->values[0], false);
	}
	default:
		result.bits = vt_nan;
	}

	if (abandon)
		abandonValue(val);

	return result;
}

value_t conv2Str (value_t val, bool abandon) {
	char buff[64];
	int len;

	while (true) {
	  if (val.type == vt_array && val.aval->obj->base.type)
		val = val.aval->obj->base;
	  else if (val.type == vt_object && val.oval->base.type)
		val = val.oval->base;
	  else
		break;
	}

	switch (val.type) {
	case vt_string: return val;
	case vt_int:
#ifndef _WIN32
		len = snprintf(buff, sizeof(buff), "%" PRIi64, val.nval);
#else
		len = _snprintf_s(buff, sizeof(buff), _TRUNCATE, "%" PRIi64, val.nval);
#endif
		break;

	case vt_infinite:
		val.bits = vt_string;

		if (val.negative)
			val.string = "-Infinity", val.aux = 9;
		else
			val.string = "Infinity", val.aux = 8;

		return val;

	case vt_bool:
		val.bits = vt_string;

		if (val.boolean)
			val.string = "true", val.aux = 4;
		else
			val.string = "false", val.aux = 5;

		return val;

	case vt_dbl:
#ifndef _WIN32
		len = snprintf(buff, sizeof(buff), "%.16G", val.dbl);
#else
		len = _snprintf_s(buff, sizeof(buff), _TRUNCATE, "%.16G", val.dbl);
#endif
		if (!(val.dbl - (uint64_t)val.dbl))
			buff[len++] = '.', buff[len++] = '0', buff[len] = 0;

		break;

	case vt_null:
		val.bits = vt_string;
		val.string = "null";
		val.aux = 4;
		return val;

	case vt_nan:
		val.bits = vt_string;
		val.string = "NaN";
		val.aux = 3;
		return val;

	case vt_date:
		return date2Str(val);

	case vt_array:
	case vt_object:
	case vt_document:
	case vt_docarray: {
		return value2Str(val, true);
	}

	default:
#ifndef _WIN32
		len = snprintf(buff, sizeof(buff), "%s", strtype(val.type));
#else
		len = _snprintf_s(buff, sizeof(buff), _TRUNCATE, "%s", strtype(val.type));
#endif
		break;
	}

	if (len > sizeof(buff))
		len = sizeof(buff);

	return newString(buff, len);
}

//	concatenate two strings

void valueCat (value_t *left, value_t right) {
	uint32_t len = left->aux + right.aux;
	value_t val;

	if (left->refcount && left->raw[-1].refCnt[0] == 0)
	  if (js_size(left->raw) > len) {
		memcpy (left->str + left->aux, right.str, right.aux);
		abandonValue(right);
		left->aux += right.aux;
		left->str[len] = 0;
		return;
	  }

	val.bits = vt_string;

	val.str = js_alloc(len + 1, false);
	val.str[len] = 0;
	val.refcount = 1;

	memcpy(val.str, left->str, left->aux);
	memcpy(val.str + left->aux, right.str, right.aux);
	val.aux  = len;

	abandonValue(right);
	abandonValue(*left);
	*left = val;
}

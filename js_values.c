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
		return !InterlockedDecrement(val.raw[-1].refCnt);
#endif
	if (val.weakcount)
#ifndef _WIN32
		return !__sync_fetch_and_add(val.raw[-1].weakCnt, -1);
#else
		return !InterlockedDecrement(val.raw[-1].weakCnt);
#endif
	return false;
}

void incrRefCnt (value_t val) {
	if (val.refcount) {
#ifndef _WIN32
		__sync_fetch_and_add(val.raw[-1].refCnt, 1);
#else
		InterlockedIncrement(val.raw[-1].refCnt);
#endif
		return;
	}

	if (val.weakcount) {
#ifndef _WIN32
		__sync_fetch_and_add(val.raw[-1].weakCnt, 1);
#else
		InterlockedIncrement(val.raw[-1].weakCnt);
#endif
		return;
	}
}

uint32_t totalRefCnt (void *obj) {
rawobj_t *raw = obj;

	return *raw[-1].refCnt + *raw[-1].weakCnt;
}

// delete values

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

		if (decrRefCnt(val.closure->protoObj))
			deleteValue(val.closure->protoObj);

		deleteValue(val.closure->obj);
		js_free(val.raw);
		break;
	}
	case vt_array: {
		for (int i=0; i< vec_count(val.aval->values); i++)
			if (decrRefCnt(val.aval->values[i]))
				deleteValue(val.aval->values[i]);

		if (decrRefCnt(val.aval->obj))
			deleteValue(val.aval->obj);

		vec_free(val.aval->values);
		js_free(val.raw);
		break;
	}
	case vt_object: {
		for (int i=0; i< vec_count(val.oval->pairs); i++) {
			if (decrRefCnt(val.oval->pairs[i].name))
				deleteValue(val.oval->pairs[i].name);
			if (decrRefCnt(val.oval->pairs[i].value))
				deleteValue(val.oval->pairs[i].value);
		}

		if (decrRefCnt(val.oval->protoChain))
			deleteValue(val.oval->protoChain);

		if (val.oval->capacity)
			js_free(val.oval->hashTbl);

		vec_free(val.oval->pairs);
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
static char vt_inf_str[]		= "Infinity";
static char vt_file_str[]		= "file";
static char vt_status_str[]		= "status";
static char vt_object_str[]		= "object";
static char vt_undef_str[]		= "undefined";
static char vt_bool_str[]		= "boolean";
static char vt_closure_str[]	= "function";
static char vt_objId_str[]		= "objId";
static char vt_document_str[]	= "document";
static char vt_docarray_str[]	= "docarray";
static char vt_unknown_str[]	= "unknown";
static char vt_date_str[]		= "date";
static char vt_null_str[]		= "null";
static char vt_propval_str[]	= "builtinProp";
static char vt_propfcn_str[]	= "builtinFcn";

char *strtype(valuetype_t t) {
	switch (t) {
	case vt_propval:	return vt_propval_str;
	case vt_propfcn:	return vt_propfcn_str;
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
	case vt_nan:		return vt_nan_str;
	case vt_document:	return vt_document_str;
	case vt_docarray:	return vt_docarray_str;
	case vt_object:	    return vt_object_str;
	case vt_date:		return vt_date_str;
	case vt_null:		return vt_null_str;
	default:;
	}
	return vt_unknown_str;
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
	InterlockedIncrement(raw[-1].refCnt);
#endif
}

//	abandon frame

void abandonFrame(frame_t *frame) {
rawobj_t *raw = (rawobj_t *)frame;

#ifndef _WIN32
	if (__sync_fetch_and_add(raw[-1].refCnt, -1))
		return;
#else
	if (InterlockedDecrement(raw[-1].refCnt))
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
	result.boolean = false;

	if (cond.type == vt_object || cond.objvalue)
		cond = callObjFcn(&cond, "valueOf", abandon);

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

	if (cond.type == vt_object || cond.objvalue)
		cond = callObjFcn(&cond, "valueOf", abandon);

	switch (cond.type) {
	case vt_objId:	return cond;
	default: break;
	}

	fprintf(stderr, "Invalid conversion too ObjId: %s\n", strtype(cond.type));
	exit(1);
}

value_t conv2Dbl (value_t val, bool abandon) {
	value_t result;

	if (val.type == vt_object || val.objvalue)
		val = callObjFcn(&val, "valueOf", abandon);

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

	if (val.type == vt_object || val.objvalue)
		val = callObjFcn(&val, "valueOf", abandon);

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

value_t conv2Str (value_t v, bool abandon, bool quote) {
	value_t ans[1], args;

	if (v.type == vt_object || v.objvalue)
		v = callObjFcn(&v, "valueOf", abandon);

	args.bits = vt_undef;

	if (v.type == vt_string) {
	  if (quote) {
		value_t q;
		q.bits = vt_string;
		q.str = "\"";
		q.aux = 1;
		*ans = q;
		valueCat (ans, v);
		valueCat (ans, q);
		if (abandon)
			abandonValue(v);
		return *ans;
	  } else {
		return v;
	  }
	}

	return callObjFcn(&v, "toString", abandon);
}

value_t fcnPropToString(value_t *args, value_t thisVal) {
	value_t ans[1];

	switch (thisVal.type) {
	  case vt_propval:
		return getPropFcnName(thisVal);

	  case vt_propfcn:
		return getPropFcnName(thisVal);

	  default:
		fprintf(stderr, "fcnPropToString: invalid type: %s\n", strtype(thisVal.type));
		exit(1);
	}

	ans->bits = vt_string;
	ans->str = strtype(thisVal.type);
	ans->aux = strlen(ans->str);
	return *ans;
}

//	concatenate string to string value_t

void valueCat (value_t *left, value_t right) {
	uint32_t len = left->aux + right.aux;
	value_t val;

	if (left->refcount && left->raw[-1].refCnt[0] < 2)
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

	if (left->refcount)
	  if (!left->raw[-1].refCnt[0] || decrRefCnt(*left))
		deleteValue(*left);

	*left = val;
}

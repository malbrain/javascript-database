#ifdef _WIN32
#include <windows.h>
#include <intrin.h>
#endif

#include <ctype.h>

#include "js.h"
#include "js_malloc.h"
#include "js_string.h"

value_t js_strtod(uint8_t *buff, int len);

void js_deleteHandle(value_t val);
void deleteDocument(value_t val);

/*
*   Expression evaluation produces a value.
*   Recursive expression tree evaluation will call 'dispatch' on each node.
*   Dispatch calls 'eval_<NodeType>()' which is responsible for returning a value.
*   Values from child node dispatches must be abandoned, stored or returned directly by the parent.
*   These values may be stored in an lval, (i.e.) an object, array slot, frame slot (symbol).
*   All non-atomic values have a refcount which counts the distinct frame slots pointing at the value.
*   Stored values have their reference counts incremented.
*   Prior (overwritten) values have their reference counts decremented.
*   Prior values which reach refcount==0 are deleted.
*   All evaluation takes place in the context of some function frame.
*   Two special frame slots exist, one for the caller's arg list, and one for 'this'.
*/

bool decrRefCnt (value_t val) {
	if (val.refcount)
#ifndef _WIN32
		return !__sync_add_and_fetch(val.raw[-1].refCnt, -1);
#else
		return !InterlockedDecrement(val.raw[-1].refCnt);
#endif
	if (val.weakcount)
#ifndef _WIN32
		return !__sync_add_and_fetch(val.raw[-1].weakCnt, -1);
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

//	assign value

value_t eval_assign(Node *a, environment_t *env)
{
	binaryNode *bn = (binaryNode*)a;
	bool prev = env->lval;
	value_t right, left;

	if (evalDebug) printf("node_assign\n");

	env->lval = true;
	left = dispatch(bn->left, env);
	env->lval = prev;

	if (left.type != vt_lval) {
		fprintf(stderr, "Not lvalue: %s\n", strtype(left.type));
		abandonValue(left);
		return makeError(a, env, "not lvalue");
	}

	right = dispatch(bn->right, env);
	return replaceValue(left, right);
}

// delete values

void deleteSlot(value_t *slot) {
	deleteValue(*slot);
	slot->type = vt_undef;
}

void deleteValue(value_t val) {
	uint32_t idx, i;

	if (val.ishandle) {
	  if (decrRefCnt(val))
		js_deleteHandle(val);

	  return;
	}

	if (val.type == vt_lval)
		val = *val.lval;

	if (val.marshaled)
		return;

	switch (val.type) {
	case vt_string: {
		if (val.refcount)
			js_free(val.raw);
		break;
	}
	case vt_closure: {
		for (idx = 1; idx < val.closure->depth; idx++)
			abandonScope(val.closure->scope[idx]);

		if (decrRefCnt(val.closure->protoObj))
			deleteValue(val.closure->protoObj);

		if (decrRefCnt(val.closure->obj))
			deleteValue(val.closure->obj);

		js_free(val.raw);
		break;
	}
	case vt_array: {
		for (i=0; i< vec_cnt(val.aval->valuePtr); i++)
			if (decrRefCnt(val.aval->valuePtr[i]))
				deleteValue(val.aval->valuePtr[i]);

		abandonValue(val.aval->obj);

		vec_free(val.aval->valuePtr);
		js_free(val.raw);
		break;
	}
	case vt_object: {
		for (i=0; i< vec_cnt(val.oval->pairsPtr); i++) {
			if (decrRefCnt(val.oval->pairsPtr[i].name))
				deleteValue(val.oval->pairsPtr[i].name);
			if (decrRefCnt(val.oval->pairsPtr[i].value))
				deleteValue(val.oval->pairsPtr[i].value);
		}

		if (decrRefCnt(val.oval->protoChain))
			deleteValue(val.oval->protoChain);

		deleteValue(*val.oval->baseVal);
		vec_free(val.oval->pairsPtr);
		js_free(val.raw);
		break;
	}
	case vt_file: {
		fclose(val.file);
		break;
	}
	case vt_document: {
		deleteDocument(val);
		break;
	}
	default:
		break;
	}
}

static char vt_control_str[]	= "control";
static char vt_endlist_str[]	= "endlist";
static char vt_docId_str[]		= "docId";
static char vt_txnId_str[]		= "txnId";
static char vt_lval_str[]		= "lval";
static char vt_centi_str[]		= "centi";
static char vt_array_str[]		= "array";
static char vt_binary_str[]		= "binary";
static char vt_function_str[]	= "function";
static char vt_uuid_str[]		= "uuid";
static char vt_md5_str[]		= "md5";
static char vt_weakref_str[]	= "weakReference";

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
static char vt_unknown_str[]	= "unknown";
static char vt_date_str[]		= "date";
static char vt_null_str[]		= "null";
static char vt_propval_str[]	= "builtinProp";
static char vt_propfcn_str[]	= "builtinFcn";

char *strtype(valuetype_t t) {
	switch (t) {
	case vt_db:			return "database";
	case vt_store:		return "docStore";
	case vt_index:		return "index";
	case vt_cursor:		return "cursor";
	case vt_iter:		return "iterator";
	case vt_txn:		return "txn";
	case vt_key:		return "key";
	case vt_propval:	return vt_propval_str;
	case vt_propfcn:	return vt_propfcn_str;
	case vt_docId:		return vt_docId_str;
	case vt_txnId:		return vt_txnId_str;
	case vt_string:		return vt_string_str;
	case vt_int:		return vt_int_str;
	case vt_bool:		return vt_bool_str;
	case vt_dbl:		return vt_dbl_str;
	case vt_file:		return vt_file_str;
	case vt_status:		return vt_status_str;
	case vt_undef:		return vt_undef_str;
	case vt_closure:	return vt_closure_str;
	case vt_infinite:	return vt_inf_str;
	case vt_nan:		return vt_nan_str;
	case vt_object:	    return vt_object_str;
	case vt_date:		return vt_date_str;
	case vt_null:		return vt_null_str;
	case vt_control:	return vt_control_str;
	case vt_endlist:	return vt_endlist_str;
	case vt_lval:		return vt_lval_str;
	case vt_centi:		return vt_centi_str;
	case vt_array:		return vt_array_str;
	case vt_binary:		return vt_binary_str;
	case vt_function:	return vt_function_str;
	case vt_uuid:		return vt_uuid_str;
	case vt_md5:		return vt_md5_str;
	case vt_weakref:	return vt_weakref_str;
	default:;
	}
	return vt_unknown_str;
}

//	clone a marshaled value to an immediate value

value_t cloneValue(value_t val) {
  value_t doc;
  doc.document = val.document;
  doc.bits = vt_document;

  if (val.marshaled)
	switch (val.type) {
	  case vt_string: {
		string_t *str = js_dbaddr(val, val.document);
		return newString(str->val, str->len);
	  }

	  case vt_array:
		return cloneArray(val, doc);

	  case vt_object:
		return cloneObject(val, doc);

	  default:
		break;
	}

  return val;
}

// replace l-value in frame, array, or object

value_t replaceValue(value_t slot, value_t value) {

	incrRefCnt(value);

	if (slot.type != vt_lval) {
		fprintf(stderr, "Not lvalue: %s\n", strtype(slot.type));
		exit(1);
	}

	if (slot.subType)
		return storeArrayValue(slot, value), value;

	if (decrRefCnt(*slot.lval))
		deleteValue(*slot.lval);

	return *slot.lval = value;
}

// replace slot value in frame, array, or object

void replaceSlot(value_t *slot, value_t value) {

	incrRefCnt(value);

	if (decrRefCnt(*slot))
		deleteValue(*slot);

	*slot = value;
}

//  add reference count to frame

void incrScopeCnt(scope_t *scope) {
rawobj_t *raw = (rawobj_t *)scope;

#ifndef _WIN32
	__sync_fetch_and_add(raw[-1].refCnt, 1);
#else
	InterlockedIncrement(raw[-1].refCnt);
#endif
}

//	abandon literals at end of statement

void abandonLiterals(environment_t *env) {
	int idx;

	while ((idx = vec_cnt(env->literals))) {
		if (decrRefCnt(env->literals[idx - 1]))
			deleteValue(env->literals[idx - 1]);
		vec_size(env->literals)--;
	}
}

//	abandon scope

void abandonScope(scope_t *scope) {
rawobj_t *raw = (rawobj_t *)scope;
uint32_t idx;

#ifndef _WIN32
	if (__sync_add_and_fetch(raw[-1].refCnt, -1))
		return;
#else
	if (InterlockedDecrement(raw[-1].refCnt))
		return;
#endif

	// abandon scope values

	for (idx = 0; idx < scope->count; idx++)
		if (decrRefCnt(scope->values[idx+1]))
			deleteValue(scope->values[idx+1]);

	// abandon frame values

	//	skipping first value which was rtnValue

	for (idx = 0; idx < scope->frame->count; idx++)
		if (decrRefCnt(scope->frame->values[idx+1]))
			deleteValue(scope->frame->values[idx+1]);

	if (decrRefCnt(scope->frame->thisVal))
		deleteValue(scope->frame->thisVal);

	if (decrRefCnt(scope->frame->arguments))
		deleteValue(scope->frame->arguments);

	js_free(scope->frame);
	js_free(scope);
}

//  compare and abandon value
//  return true if they are different objects


bool abandonValueIfDiff(value_t val, value_t test) {
	if (!val.refcount)
		return true;

	if (val.type != test.type || val.raw != test.raw)
		if (!*val.raw[-1].refCnt) {
		  deleteValue(val);
		  return true;
		}

	return false;
}
			
//  abandon value

void abandonSlot(value_t *slot) {
	abandonValue(*slot);
	slot->type = vt_undef;
}

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

value_t conv2Bool(value_t src, bool abandon) {
	value_t result, cond;

	result.bits = vt_bool;
	result.boolean = false;

	if (src.type == vt_object || src.objvalue)
		cond = callObjFcn(src, &ValueOfStr, abandon, NULL);
	else
		cond = src;

	if (cond.ishandle) {
		result.boolean = 1;
		return result;
	}

	switch (cond.type) {
	case vt_nan: result.boolean = false; break;
	case vt_array: result.boolean = true; break;
	case vt_object: result.boolean = true; break;
	case vt_infinite: result.boolean = true; break;
	case vt_dbl: result.boolean = cond.dbl != 0; break;
	case vt_int: result.boolean = cond.nval != 0; break;
	case vt_status: result.boolean = cond.status == OK; break;
	case vt_file: result.boolean = cond.file != NULL; break;
	case vt_closure: result.boolean = cond.closure != NULL; break;
	case vt_docId: result.boolean = cond.idBits > 0; break;
	case vt_txnId: result.boolean = cond.idBits > 0; break;
	case vt_document: result.boolean = true; break;
	case vt_undef: result.boolean = false; break;
	case vt_null: result.boolean = false; break;
	case vt_bool: return cond;

	case vt_string: {
		string_t *str = js_addr(cond);
		result.boolean = str->len > 0;
		break;
	}
	default: break;
	}

	return result;
}

value_t conv2Dbl (value_t src, bool abandon) {
	value_t result, val;

	if (src.type == vt_object || src.objvalue)
		val = callObjFcn(src, &ValueOfStr, abandon, NULL);
	else
		val = src;

	result.bits = vt_dbl;
	result.dbl = 0;

	switch (val.type) {
	case vt_undef:	result.bits = vt_nan; break;
	case vt_dbl:	result.dbl = val.dbl; break;
	case vt_int:	result.dbl = (double)val.nval; break;
	case vt_bool:	result.dbl = (double)val.boolean; break;

	case vt_string: {
		string_t *str = js_addr(val);
		result = js_strtod(str->val, str->len);
		break;
	}

	default: break;
	}

	abandonValueIfDiff(val, src);

	if (abandon)
		abandonValue(src);

	return result;
}

value_t conv2Int (value_t src, bool abandon) {
	value_t result, val;

	if (src.marshaled)
		val = src;
	else if (src.type == vt_object || src.objvalue)
		val = callObjFcn(src, &ValueOfStr, abandon, NULL);
	else
		val = src;

	result.bits = vt_int;
	result.nval = 0;

	switch (val.type) {
	case vt_undef:	result.bits = vt_nan; break;
	case vt_int:	result.nval = val.nval; break;
	case vt_dbl:	result.nval = (int64_t)val.dbl; break;
	case vt_bool:	result.nval = val.boolean; break;
	case vt_null:	result.nval = 0; break;
	case vt_string: {
		string_t *str = js_addr(val);
		result = js_strtod(str->val, str->len);
		break;
	}

	case vt_array: {
		value_t *values;
		uint32_t cnt;

		if (val.marshaled) {
			dbarray_t *dbaval = js_addr(val);
			values = dbaval->valueArray;
			cnt = dbaval->cnt;
		} else {
			values = val.aval->valuePtr;
			cnt = vec_cnt(values);
		}

		if (cnt>1)
			break;

		if (!cnt)
			return result.nval = 0, result;

		return conv2Int(values[0], false);
	}

	default:
		result.bits = vt_nan;
	}

	abandonValueIfDiff(val, src);

	if (abandon)
		abandonValue(src);

	return result;
}

//	convert an arbitrary value to a string value

value_t conv2Str (value_t v, bool abandon, bool quote) {
	value_t ans[1], original = v;

	if (v.type == vt_document)
		v = getDocObject(v);

	if (v.type != vt_string)
		v = callObjFcn(v, &ToStringStr, abandon, NULL);

	if (v.type == vt_undef)
		v = newString(strtype(original.type), -1);

	if (quote) {
		value_t q;
		q.bits = vt_string;
		q.addr = &QuoteStr;
		*ans = q;
		valueCat (ans, v, abandon);
		valueCat (ans, q, false);
		return *ans;
	}

	return v;
}

//	concatenate string to string value_t

void valueCat (value_t *left, value_t right, bool abandon) {
	string_t *rightstr = js_addr(right);

	valueCatStr(left, rightstr->val, rightstr->len);

	if (abandon)
		abandonValue(right);
}

//	concatenate string to string value_t

void valueCatStr (value_t *left, uint8_t *rightval, uint32_t rightlen) {
	string_t *leftstr = js_addr(*left), *valstr;
	uint32_t len = rightlen + leftstr->len;
	value_t val;

	//  can we extend existing string value?

	if (left->refcount && left->raw[-1].refCnt[0] < 2)
	  if (js_size(left->raw) - sizeof(string_t) > len) {
		memcpy (leftstr->val + leftstr->len, rightval, rightlen);
		leftstr->len += rightlen;
		leftstr->val[len] = 0;
		return;
	  }

	val = newString(NULL, len);
	valstr = val.addr;

	valstr->val[len] = 0;

	memcpy(valstr->val, leftstr->val, leftstr->len);
	memcpy(valstr->val + leftstr->len, rightval, rightlen);
	valstr->len = len;

	replaceSlot(left, val);
}

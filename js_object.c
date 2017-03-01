#include "js.h"
#include "js_props.h"
#include "js_string.h"

value_t newObject(valuetype_t type) {
	object_t *oval;
	value_t v;

	v.bits = vt_object;
	v.addr = js_alloc(sizeof(object_t), true);
	v.refcount = 1;

	oval = v.addr;
	oval->protoBase = type;

	return v;
}

value_t newArray(enum ArrayType subType) {
	array_t *aval;
	value_t v;

	v.bits = vt_array;

	v.addr = js_alloc(sizeof(array_t), true);

	aval = v.addr;
	aval->obj = newObject(vt_array);

	v.subType = subType;
	v.objvalue = 1;
	v.refcount = 1;
	return v;
}

void cloneObject(value_t *obj) {
	object_t *oval = js_addr(*obj), *newObj;
	pair_t *pairs = oval->marshaled ? oval->pairArray : oval->pairsPtr;
	uint32_t cnt = oval->marshaled ? oval->cnt : vec_cnt(pairs);
	int idx;

	obj->bits = vt_object;
	obj->addr = js_alloc(sizeof(object_t),true);
	obj->refcount = 1;

	incrRefCnt(*obj);

	newObj = obj->addr;
	newObj->pairsPtr = newVector(cnt + cnt / 4, sizeof(pair_t), true);

	for (idx = 0; idx < cnt; idx++)
	  replaceSlot(lookup(newObj, pairs[idx].name, true, 0), pairs[idx].value);
}

value_t convArray2Value(void *val, enum ArrayType type) {
	value_t result;

	switch (type) {
	case array_value:
		return *(value_t *)val;
	case array_int8:
		result.bits = vt_int;
		result.nval = *(int8_t *)val;
		return result;
	case array_uint8:
		result.bits = vt_int;
		result.nval = *(uint8_t *)val;
		return result;
	case array_int16:
		result.bits = vt_int;
		result.nval = *(int16_t *)val;
		return result;
	case array_uint16:
		result.bits = vt_int;
		result.nval = *(uint16_t *)val;
		return result;
	case array_int32:
		result.bits = vt_int;
		result.nval = *(int32_t *)val;
		return result;
	case array_uint32:
		result.bits = vt_int;
		result.nval = *(uint32_t *)val;
		return result;
	case array_float32:
		result.bits = vt_dbl;
		result.dbl = *(float *)val;
		return result;
	case array_float64:
		result.bits = vt_dbl;
		result.dbl = *(double *)val;
		return result;
	}

	result.bits = vt_undef;
	return result;
}

void storeArrayValue(value_t left, value_t right) {
	string_t *leftstr = js_addr(left);

	switch (left.subType) {
	case array_value:
		*left.lval = right;
		return;
	case array_int8:
		*(int8_t *)leftstr->val = conv2Int(right, true).nval;
		return;
	case array_uint8:
		*(uint8_t *)leftstr->val = conv2Int(right, true).nval;
		return;
	case array_int16:
		*(int16_t *)leftstr->val = conv2Int(right, true).nval;
		return;
	case array_uint16:
		*(uint16_t *)leftstr->val = conv2Int(right, true).nval;
		return;
	case array_int32:
		*(int32_t *)leftstr->val = conv2Int(right, true).nval;
		return;
	case array_uint32:
		*(uint32_t *)leftstr->val = conv2Int(right, true).nval;
		return;
	case array_float32:
		*(float *)leftstr->val = conv2Dbl(right, true).dbl;
		return;
	case array_float64:
		*(double *)leftstr->val = conv2Dbl(right, true).dbl;
		return;
	}
}

uint64_t hashStr(uint8_t *str, uint32_t len) {
	uint64_t hash = 0;
	uint64_t mask;

	while (len>=8) {
		len -= 8;
		hash += *((uint64_t *) &str[len]);
		hash *= 5;
	}

	mask = 1ULL << len * 8;
	return hash += --mask & (*((uint64_t *) &str[0]));
}

void hashStore(void *table, uint32_t hashEnt, uint32_t idx, uint32_t val) {
	switch (hashEnt) {
	  case 1:
		((uint8_t *)table)[idx] = val;
		return;
	  case 2:
		((uint16_t *)table)[idx] = val;
		return;
	  case 4:
		((uint32_t *)table)[idx] = val;
		return;
	  default:
		fprintf(stderr, "bad hash table entry size: %d\n", hashEnt);
		exit(0);
	}
}

uint32_t hashEntry(void *table, uint32_t hashEnt, uint32_t idx) {
	switch (hashEnt) {
	  case 1:
		return ((uint8_t *)table)[idx];
	  case 2:
		return ((uint16_t *)table)[idx];
	  case 4:
		return ((uint32_t *)table)[idx];
	  default:
		fprintf(stderr, "bad hash table entry size: %d\n", hashEnt);
		exit(0);
	}
}

uint32_t hashBytes(uint32_t cap) {
	if (cap < 255) {
		return sizeof(uint8_t);
	} else if (cap < 65535) {
		return sizeof(uint16_t);
	}

	return sizeof(uint32_t);
}

//  evaluate object slot value

value_t evalProp(value_t *slot, value_t base, bool lval) {
	value_t v;

	if (slot->type == vt_propval)
		return callFcnProp(*slot, base, lval);

	if (!lval)
		return *slot;

	v.bits = vt_lval;
	v.lval = slot;
	return v;
}

//	execute lookup/access operation in object/prototype/builtins

value_t lookupAttribute(value_t obj, value_t field, bool lVal, value_t *original) {
	valuetype_t base = original->type, next = 0;
	string_t *fldstr = js_addr(field);
	value_t v, *slot;
	object_t *oval;
	uint64_t hash;
	int done = 0;

	//	reference to fcn prototype?

	if (obj.type == vt_closure) {
	  if (fldstr->len == 9 && !memcmp(fldstr->val, "prototype", 9)) {
		if (lVal ) {
		  v.bits = vt_lval;
		  v.lval = &obj.closure->protoObj;
		} else {
		  v = obj.closure->protoObj;
		}

		*original = obj.closure->protoObj;
		return v;
	  }
	}

	// attribute on object like things

	if (obj.objvalue)
		obj = *obj.lval;

	hash = hashStr(fldstr->val, fldstr->len);

	//  examine prototype chain
	//	then builtin properties

	while (true) {
	  if (obj.type != vt_object) {
		if (base) {
		  obj = builtinProto[base];
		  base = 0;
		} else if ((base = next) && !done++)
		  next = 0;
		else
		  break;

		continue;
	  }

	  oval = js_addr(obj);

	  if ((slot = lookup(oval, field, lVal, hash)))
		return evalProp(slot, *original, lVal);

	  if (lVal)
		break;

	  if (!next)
		  next = oval->protoBase;

	  obj = oval->protoChain;
	}

	v.bits = vt_undef;
	return v;
}

value_t *lookup(object_t *oval, value_t name, bool lVal, uint64_t hash) {
	uint32_t idx, h, cap, hashMod, hashEnt;
	string_t *namestr = js_addr(name);
	pair_t pair, *pairs;
	uint32_t start;
	void *hashTbl;
	value_t *val;
	
	if (!hash)
		hash = hashStr(namestr->val, namestr->len);

	pairs = oval->marshaled ? oval->pairArray : oval->pairsPtr;
	cap = oval->marshaled ? oval->cap : vec_max(pairs);

	hashEnt = hashBytes(cap);
	hashMod = 3 * cap / 2;
	hashTbl = pairs + cap;

	if (cap) {
	  start = hash % hashMod;
	  h = start;

	  while ((idx = hashEntry(hashTbl, hashEnt, h))) {
		string_t *keystr = js_addr(pairs[idx - 1].name);

		if (keystr->len == namestr->len) {
		  if (!memcmp(keystr->val, namestr->val, namestr->len)) {
			val = &pairs[idx - 1].value;
		  	goto lookupxit;
		  }
		}

		if (++h == hashMod)
			h = 0;

		if (h == start) {
			fprintf(stderr, "hash table overflow looking for %.*s\n", namestr->len, namestr->val);
			exit(0);
		}
	  }
	}

	if (!lVal)
		return NULL;

	assert (!oval->marshaled);

	pair.value.bits = vt_undef;
	pair.name = name;
	incrRefCnt(name);

	//  append the new object pair vector
	//	is the pair vector full?

	if (vec_cnt(oval->pairsPtr) + 1 < cap) {
	  oval->pairsPtr[vec_size(oval->pairsPtr)++] = pair;
	  hashStore(hashTbl, hashEnt, h, vec_size(oval->pairsPtr));
	} else {
	  oval->pairsPtr = vec_grow (oval->pairsPtr, cap, sizeof(pair_t), true);
	  cap = vec_max(oval->pairsPtr);
	  hashEnt = hashBytes(cap);
	  hashTbl = oval->pairsPtr + cap;
	  hashMod = 3 * cap / 2;

	  //  append the new object pair vector
	  //  with the new property

	  oval->pairsPtr[vec_size(oval->pairsPtr)++] = pair;

	  // rehash current & new entries

	  for (int i=0; i< vec_cnt(oval->pairsPtr); i++) {
		namestr = js_addr(oval->pairsPtr[i].name);
		h = hashStr(namestr->val, namestr->len) % hashMod;

	  	while (hashEntry(hashTbl, hashEnt, h))
		  if (++h == hashMod)
			h = 0;

		hashStore(hashTbl, hashEnt, h, i + 1);
	  }
	}

	idx = vec_cnt(oval->pairsPtr);
	val = &oval->pairsPtr[idx - 1].value;

	//  return slot value address

lookupxit:
	if (lVal && val->type == vt_object) {
	  oval = js_addr(*val);
	  if (oval->marshaled)
		cloneObject(val);
	}

	return val;
}

// TODO -- remove the field from the name & value vectors and hash table

value_t *deleteField(object_t *obj, value_t name) {
	uint32_t cap = vec_max(obj->pairsPtr);
	void *hashTbl = obj->pairsPtr + cap;
	string_t *namestr = js_addr(name);
	uint32_t hashEnt = hashBytes(cap);
	uint32_t hashMod = 3 * cap / 2;
	uint32_t idx, start, h;

	if (cap)
		h = hashStr(namestr->val, namestr->len) % hashMod;
	else
		return NULL;

	start = h;

	do {
		if ((idx = hashEntry(hashTbl, hashEnt, h))) {
			pair_t *key = obj->pairsPtr + idx - 1;
			string_t *keystr = js_addr(key->name);

			if (keystr->len == namestr->len)
			  if (!memcmp(keystr->val, namestr->val, namestr->len))
				return &obj->pairsPtr[idx - 1].value;
		}

		if (++h == hashMod)
			h = 0;
	} while (h != start);

	// not there
	return NULL;
}

value_t fcnArrayToString(value_t *args, value_t *thisVal, environment_t *env) {
	value_t ending, comma, ans[1];
	uint32_t idx = 0;
	value_t *values;
	uint32_t cnt;
	array_t *aval;

	if (vec_cnt(args)) {
		aval = js_addr(*args);
		values = args->marshaled ? aval->valueArray : aval->valuePtr;
		cnt = args->marshaled ? aval->cnt : vec_cnt(aval->valuePtr);
	} else {
		aval = js_addr(*thisVal);
		values = thisVal->marshaled ? aval->valueArray : aval->valuePtr;
		cnt = thisVal->marshaled ? aval->cnt : vec_cnt(aval->valuePtr);
	}

	ans->bits = vt_string;
	ans->addr = &LeftBrackStr;

	comma.bits = vt_string;
	comma.addr = &CommaStr;

	while (idx < cnt) {
		value_t v = values[idx];
		v = conv2Str(v, false, v.type == vt_string);
		valueCat(ans, v, true);

		if (++idx < cnt)
			valueCat(ans, comma, false);
	}

	ending.bits = vt_string;
	ending.addr = &RightBrackStr;

	valueCat(ans, ending, false);
	return *ans;
}

/*
value_t fcnObjectIs(value_t *args, value_t *thisVal, environment_t *env) {
	object_t *oval;
	value_t val;

	if (thisVal->objvalue)
		oval = js_addr(*thisVal->lval);
	else
		oval = js_addr(*thisVal);

	val.bits = vt_undef;
	return val;
}

value_t fcnObjectKeys(value_t *args, value_t *thisVal, environment_t *env) {
	object_t *oval;
	value_t val;

	if (thisVal->objvalue)
		oval = js_addr(*thisVal->lval);
	else
		oval = js_addr(*thisVal);

	val.bits = vt_undef;
	return val;
}

value_t fcnObjectPreventExtensions(value_t *args, value_t *thisVal, environment_t *env) {
	object_t *oval;
	value_t val;

	if (thisVal->objvalue)
		oval = js_addr(*thisVal->lval);
	else
		oval = js_addr(*thisVal);

	val.bits = vt_undef;
	return val;
}

value_t fcnObjectSeal(value_t *args, value_t *thisVal, environment_t *env) {
	object_t *oval;
	value_t val;

	if (thisVal->objvalue)
		oval = js_addr(*thisVal->lval);
	else
		oval = js_addr(*thisVal);

	val.bits = vt_undef;
	return val;
}

value_t fcnObjectSetPrototypeOf(value_t *args, value_t *thisVal, environment_t *env) {
	object_t *oval;
	value_t val;

	if (thisVal->objvalue)
		oval = js_addr(*thisVal->lval);
	else
		oval = js_addr(*thisVal);

	val.bits = vt_undef;
	return val;
}

value_t fcnObjectValues(value_t *args, value_t *thisVal, environment_t *env) {
	object_t *oval;
	value_t val;

	if (thisVal->objvalue)
		oval = js_addr(*thisVal->lval);
	else
		oval = js_addr(*thisVal);

	val.bits = vt_undef;
	return val;
}

value_t fcnObjectIsExtensible(value_t *args, value_t *thisVal, environment_t *env) {
	object_t *oval;
	value_t val;

	if (thisVal->objvalue)
		oval = js_addr(*thisVal->lval);
	else
		oval = js_addr(*thisVal);

	val.bits = vt_undef;
	return val;
}

value_t fcnObjectIsFrozen(value_t *args, value_t *thisVal, environment_t *env) {
	object_t *oval;
	value_t val;

	if (thisVal->objvalue)
		oval = js_addr(*thisVal->lval);
	else
		oval = js_addr(*thisVal);

	val.bits = vt_undef;
	return val;
}

value_t fcnObjectIsSealed(value_t *args, value_t *thisVal, environment_t *env) {
	object_t *oval;
	value_t val;

	if (thisVal->objvalue)
		oval = js_addr(*thisVal->lval);
	else
		oval = js_addr(*thisVal);

	val.bits = vt_undef;
	return val;
}

value_t fcnObjectCreate(value_t *args, value_t *thisVal, environment_t *env) {
	object_t *oval;
	value_t val;

	if (thisVal->objvalue)
		oval = js_addr(*thisVal->lval);
	else
		oval = js_addr(*thisVal);

	val.bits = vt_undef;
	return val;
}

value_t fcnObjectEntries(value_t *args, value_t *thisVal, environment_t *env) {
	object_t *oval;
	value_t val;

	if (thisVal->objvalue)
		oval = js_addr(*thisVal->lval);
	else
		oval = js_addr(*thisVal);

	val.bits = vt_undef;
	return val;
}

value_t fcnObjectFreeze(value_t *args, value_t *thisVal, environment_t *env) {
	object_t *oval;
	value_t val;

	if (thisVal->objvalue)
		oval = js_addr(*thisVal->lval);
	else
		oval = js_addr(*thisVal);

	val.bits = vt_undef;
	return val;
}

value_t fcnObjectGetOwnPropDesc(value_t *args, value_t *thisVal, environment_t *env) {
	object_t *oval;
	value_t val;

	if (thisVal->objvalue)
		oval = js_addr(*thisVal->lval);
	else
		oval = js_addr(*thisVal);

	val.bits = vt_undef;
	return val;
}

value_t fcnObjectGetOwnPropNames(value_t *args, value_t *thisVal, environment_t *env) {
	object_t *oval;
	value_t val;

	if (thisVal->objvalue)
		oval = js_addr(*thisVal->lval);
	else
		oval = js_addr(*thisVal);

	val.bits = vt_undef;
	return val;
}

value_t fcnObjectGetOwnPropSymbols(value_t *args, value_t *thisVal, environment_t *env) {
	object_t *oval;
	value_t val;

	if (thisVal->objvalue)
		oval = js_addr(*thisVal->lval);
	else
		oval = js_addr(*thisVal);

	val.bits = vt_undef;
	return val;
}

value_t fcnObjectDefineProp(value_t *args, value_t *thisVal, environment_t *env) {
	object_t *oval;
	value_t val;

	if (thisVal->objvalue)
		oval = js_addr(*thisVal->lval);
	else
		oval = js_addr(*thisVal);

	val.bits = vt_undef;
	return val;
}

value_t fcnObjectDefineProps(value_t *args, value_t *thisVal, environment_t *env) {
	object_t *oval;
	value_t val;

	if (thisVal->objvalue)
		oval = js_addr(*thisVal->lval);
	else
		oval = js_addr(*thisVal);

	val.bits = vt_undef;
	return val;
}
*/

value_t fcnObjectSetValue(value_t *args, value_t *thisVal, environment_t *env) {
	object_t *oval;
	value_t base;

	if (thisVal->objvalue)
		oval = js_addr(*thisVal->lval);
	else
		oval = js_addr(*thisVal);

	if (vec_cnt(args))
		base = args[0];
	else
		base.bits = vt_undef;

	replaceSlot(oval->base, base);
	return base;
}

value_t fcnObjectHasOwnProperty(value_t *args, value_t *thisVal, environment_t *env) {
	object_t *oval;
	value_t val;

	if (thisVal->objvalue)
		oval = js_addr(*thisVal->lval);
	else
		oval = js_addr(*thisVal);

	val.bits = vt_bool;

	if (vec_cnt(args))
		val.boolean = lookup(oval, args[0], false, 0) ? true : false;
	else
		val.boolean = false;

	return val;
}

value_t fcnObjectValueOf(value_t *args, value_t *thisVal, environment_t *env) {
	object_t *oval;

	if (vec_cnt(args))
		oval = js_addr(args[0]);
	else if (thisVal->objvalue)
		oval = js_addr(*thisVal->lval);
	else
		oval = js_addr(*thisVal);

	if (oval->base->type == vt_undef)
		return *thisVal;

	return *oval->base;
}

/*
value_t fcnObjectLock(value_t *args, value_t *thisVal, environment_t *env) {
	object_t *oval = js_addr(*thisVal);
	value_t val;

	if (thisVal->objvalue)
		oval = js_addr(*thisVal->lval);

	if (vec_cnt(args) > 0)
		mode = conv2Int(args[0], false);
	else
		return val.bits = vt_undef, val;

	if (mode.type != vt_int)
		return val.bits = vt_undef, val;

	switch (mode.nval) {
	case 0:	readLock(oval->lock); break;
	case 1:	writeLock(oval->lock); break;
	}

	val.bits = vt_bool;
	val.boolean = true;
	return val;
}

value_t fcnObjectUnlock(value_t *args, value_t *thisVal, environment_t *env) {
	object_t *oval = js_addr(*thisVal);
	value_t val;

	if (thisVal->objvalue)
		oval = js_addr(*thisVal->lval);

	rwUnlock(oval->lock);
	val.bits = vt_bool;
	val.boolean = true;
	return val;
}
*/
value_t fcnObjectToString(value_t *args, value_t *thisVal, environment_t *env) {
	object_t *oval = vec_cnt(args) ? js_addr(args[0]) : js_addr(*thisVal);
	pair_t *pairs = oval->marshaled ? oval->pairArray : oval->pairsPtr;
	uint32_t cnt = oval->marshaled ? oval->cnt : vec_cnt(pairs);
	value_t colon, ending, comma, ans[1];
	uint32_t idx = 0;

	if (oval->base->type > vt_undef)
		return conv2Str(*oval->base, false, false);

	ans->bits = vt_string;
	ans->addr = &LeftBraceStr;

	colon.bits = vt_string;
	colon.addr = &ColonStr;

	comma.bits = vt_string;
	comma.addr = &CommaStr;

	while (idx < cnt) {
		value_t v = conv2Str(pairs[idx].name, false, true);
		valueCat(ans, v, true);
		valueCat(ans, colon, false);

		v = pairs[idx].value;
		v = conv2Str(v, true, v.type == vt_string);
		valueCat(ans, v, true);

		if (++idx < cnt)
			valueCat(ans, comma, false);
	}

	ending.bits = vt_string;
	ending.addr = &RightBraceStr;

	valueCat(ans, ending, false);
	return *ans;
}

value_t propObjProto(value_t val, bool lVal) {
	object_t *oval = js_addr(val);
	value_t ref, *proto = &oval->protoChain;

	if (!lVal)
		return *proto;

	ref.bits = vt_lval;
	ref.lval = proto;
	return ref;
}

value_t propObjLength(value_t val, bool lVal) {
	value_t len;

	len.bits = vt_int;
	len.nval = 1;
	return len;
}

value_t fcnArraySlice(value_t *args, value_t *thisVal, environment_t *env) {
	value_t array = newArray(array_value);
	array_t *src = js_addr(*thisVal);
	array_t *aval = array.addr;
	value_t slice, end;
	int start, count;
	value_t *values;
	int idx, cnt;

	values = thisVal->marshaled ? src->valueArray : src->valuePtr;
	cnt = thisVal->marshaled ? src->cnt : vec_cnt(src->valuePtr);

	if (vec_cnt(args) > 0)
		slice = conv2Int(args[0], false);
	else {
		slice.bits = vt_int;
		slice.nval = 0;
	}

	if (vec_cnt(args) > 1)
		end = conv2Int(args[1], false);
	else {
		end.bits = vt_int;
		end.nval = cnt;
	}

	if (slice.type != vt_int)
		slice.nval = 0;

	if (end.nval < 0)
		end.nval += cnt;

	if (end.nval > cnt || end.nval == 0)
		end.nval = cnt;

	if (slice.nval < 0) {
		start = slice.nval + cnt;
		count = -slice.nval;
	} else {
		start = slice.nval;
		count = end.nval - start;
	}

	for (idx = 0; idx < count; idx++) {
		value_t nxt = values[start + idx];
		vec_push(aval->valuePtr, nxt);
		incrRefCnt(nxt);
	}

	return array;
}

value_t fcnArrayConcat(value_t *args, value_t *thisVal, environment_t *env) {
	value_t array = newArray(array_value);
	array_t *src = js_addr(*thisVal);
	array_t *aval = array.addr;
	value_t *values;
	int idx, cnt;

	values = thisVal->marshaled ? src->valueArray : src->valuePtr;
	cnt = thisVal->marshaled ? src->cnt : vec_cnt(src->valuePtr);

	//  clone existing array values

	for (idx = 0; idx < cnt; idx++) {
		value_t nxt = values[idx];
		vec_push(aval->valuePtr, nxt);
		incrRefCnt(nxt);
	}

	//  append new argument elements

	for (idx = 0; idx < vec_cnt(args); idx++) {

	  if (args[idx].type == vt_array) {
	    array_t *nxt = js_addr(args[idx]);
	    value_t *nxtvalues = args[idx].marshaled ? nxt->valueArray : nxt->valuePtr;
	    int nxtcnt = args[idx].marshaled ? nxt->cnt : vec_cnt(nxt->valuePtr);

		for (int j = 0; j < nxtcnt; j++) {
		  vec_push(aval->valuePtr, nxtvalues[j]);
		  incrRefCnt(nxtvalues[j]);
		}
	  } else {
		vec_push (aval->valuePtr, args[idx]);
		incrRefCnt(args[idx]);
	  }
	}

	return array;
}

value_t fcnArrayValueOf(value_t *args, value_t *thisVal, environment_t *env) {

	if (vec_cnt(args))
		return args[0];
	else
		return *thisVal;
}

value_t fcnArrayJoin(value_t *args, value_t *thisVal, environment_t *env) {
	array_t *aval = js_addr(*thisVal);
	value_t *values = thisVal->marshaled ? aval->valueArray : aval->valuePtr;
	uint32_t cnt = thisVal->marshaled ? aval->cnt : vec_cnt(aval->valuePtr);
	value_t delim, val[1], v;

	if (vec_cnt(args) > 0)
		delim = conv2Str(args[0], false, false);
	else {
		delim.bits = vt_string;
		delim.addr = &CommaStr;
	}

	val->bits = vt_string;
	val->addr = &EmptyStr;

	for (int idx = 0; idx < cnt; idx++) {
		v = conv2Str(values[idx], false, false);
		valueCat(val, v, true);

		if (idx < cnt - 1)
			valueCat(val, delim, false);
	}

	abandonValue (delim);
	return *val;
}

/*
value_t fcnArrayLock(value_t *args, value_t *thisVal, environment_t *env) {
	array_t *array = js_addr(*thisVal);
	value_t val, mode;

	if (vec_cnt(args) > 0)
		mode = conv2Int(args[0], false);
	else
		return val.bits = vt_undef, val;

	if (mode.type != vt_int)
		return val.bits = vt_undef, val;

	switch (mode.nval) {
	case 0:	readLock(aval->lock); break;
	case 1:	writeLock(aval->lock); break;
	}

	val.bits = vt_bool;
	val.boolean = true;
	return val;
}

value_t fcnArrayUnlock(value_t *args, value_t *thisVal, environment_t *env) {
	array_t *array = js_addr(*thisVal);
	value_t val;

	rwUnlock(array->lock);
	val.bits = vt_bool;
	val.boolean = true;
	return val;
}
*/
value_t fcnArraySetValue(value_t *args, value_t *thisVal, environment_t *env) {
	array_t *aval = js_addr(*thisVal);
	value_t undef;

	if (vec_cnt(args))
		undef = args[0];
	else
		undef.bits = vt_undef;

	return aval->obj = undef;
}

value_t propArrayProto(value_t val, bool lVal) {
	array_t *aval = js_addr(val);
	object_t *oval;
	value_t ref;

	oval = js_addr(aval->obj);

	if (!lVal)
		return oval->protoChain;

	ref.bits = vt_lval;
	ref.lval = &oval->protoChain;
	return ref;
}

value_t propArrayLength(value_t val, bool lVal) {
	array_t *aval = js_addr(val);
	uint32_t cnt = val.marshaled ? aval->cnt : vec_cnt(aval->valuePtr);
	value_t num;

	num.bits = vt_int;
	num.nval = cnt;
	return num;
}

PropVal builtinObjProp[] = {
	{ propObjLength, "length"},
	{ propObjProto, "prototype", true },
	{ NULL, NULL}
};

PropFcn builtinObjFcns[] = {
//	{ fcnObjectLock, "lock" },
//	{ fcnObjectUnlock, "unlock" },
	{ fcnObjectValueOf, "valueOf" },
	{ fcnObjectToString, "toString" },
	{ fcnObjectSetValue, "setValue" },
//	{ fcnObjectHasOwnProperty, "hasOwnProperty" },
//	{ fcnObjectIs, "is", true },
//	{ fcnObjectKeys, "keys", true },
//	{ fcnObjectPreventExtensions, "preventExtensions", true },
//	{ fcnObjectSeal, "seal", true },
//	{ fcnObjectSetPrototypeOf, "setPrototypeOf", true },
//	{ fcnObjectValues, "values", true },
//	{ fcnObjectIsExtensible, "isExtensible", true },
//	{ fcnObjectIsFrozen, "isFrozen", true },
//	{ fcnObjectIsSealed, "isSealed", true },
//	{ fcnObjectCreate, "create", true },
//	{ fcnObjectEntries, "entries", true },
//	{ fcnObjectFreeze, "freeze", true },
//	{ fcnObjectGetOwnPropDesc, "GetOwnPropertyDescriptors", true },
//	{ fcnObjectGetOwnPropNames, "GetOwnPropertyNames", true },
//	{ fcnObjectGetOwnPropSymbols, "GetOwnPropertySymbols", true },
//	{ fcnObjectDefineProp, "defineProperty", true },
//	{ fcnObjectDefineProps, "defineProperties", true },
	{ NULL, NULL}
};

PropVal builtinArrayProp[] = {
	{ propArrayLength, "length" },
	{ propArrayProto, "prototype", true },
	{ NULL, NULL}
};

PropFcn builtinArrayFcns[] = {
//	{ fcnArrayLock, "lock" },
//	{ fcnArrayUnlock, "unlock" },
	{ fcnArrayToString, "toString" },
	{ fcnArraySetValue, "setValue" },
	{ fcnArrayValueOf, "valueOf" },
	{ fcnArrayConcat, "concat" },
	{ fcnArraySlice, "slice" },
	{ fcnArrayJoin, "join" },
	{ NULL, NULL}
};


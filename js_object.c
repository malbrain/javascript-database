#include "js.h"
#include "js_props.h"
#include "js_string.h"

extern PropFcn *builtinFcn[];

value_t newObject(valuetype_t type) {
	value_t v;

	v.bits = vt_object;
	v.addr = js_alloc(sizeof(object_t), true);
	v.oval->protoBase = type;
	v.refcount = 1;

	return v;
}

value_t newArray(enum ArrayType subType, uint32_t initSize) {
	value_t v;

	v.bits = vt_array;

	v.addr = js_alloc(sizeof(array_t), true);
	v.aval->obj = newObject(vt_array);

	if (initSize)
		v.aval->valuePtr = newVector (initSize, sizeof(value_t), false);

	v.subType = subType;
	v.objvalue = 1;
	v.refcount = 1;
	return v;
}

//  retrieve baseVal from object

value_t *baseObject(value_t obj) {
	if (obj.marshaled)
		return NULL;

	return obj.oval->baseVal;
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

//  evaluate object value

value_t evalBuiltin(value_t v, value_t arg, value_t *original, bool lval, bool eval) {
	if (v.type == vt_lval)
		return v;

	if (eval && v.type == vt_propfcn)
		return (builtinFcn[v.subType][v.nval].fcn)(NULL, original, NULL);

	if (v.type == vt_propval)
		return callFcnProp(v, arg, *original, lval);

	if (eval && v.type == vt_closure) {
		array_t aval[1];
		value_t args;

		memset (aval, 0, sizeof(args));
		args.bits = vt_array;
		args.aval = aval;

		return fcnCall(v, args, *original, false, NULL);
	}

	if (v.marshaled)
		v.addr = arg.addr;

	return v;
}

//	execute lookup/access operation in object/prototype/builtins

typedef enum {
	ProtoChain,
	ProtoBase,
	OriginalVal,
	AllDone
} LookupPhase;

value_t lookupAttribute(value_t obj, value_t field, bool lVal, bool eval) {
	string_t *fldstr = js_addr(field);
	LookupPhase phase = ProtoChain;
	value_t v, original = obj;
	uint64_t hash;

	//	reference to fcn prototype?

	if (obj.type == vt_closure) {
	  if (fldstr->len == 9 && !memcmp(fldstr->val, "prototype", 9)) {
		if (lVal ) {
		  v.bits = vt_lval;
		  v.lval = &obj.closure->protoObj;
		} else
		  v = obj.closure->protoObj;

		return v;
	  }
	}

	//	document lookup

	if (obj.type == vt_document)
		obj = convDocument(obj, lVal);

	// attribute on object like things

	if (obj.objvalue)
		obj = *obj.lval;

	hash = hashStr(fldstr->val, fldstr->len);

	//  go to builtins if not an object

	if (obj.type != vt_object)
		obj = builtinProto[obj.type];

	if (obj.type != vt_object)
	  return v.bits = vt_undef, v;

	if (obj.marshaled) {
	  // 1st, look in the object

	  if ((v = lookup(obj, field, lVal, hash)).type != vt_undef)
		  return evalBuiltin(v, obj, &original, lVal, eval);

	  // 2nd, look in the original type builtins

	  if ((v = lookup(builtinProto[original.type], field, lVal, hash)).type != vt_undef)
		  return evalBuiltin(v, obj, &original, lVal, eval);

	  // 3rd, look in the object type builtins

	  if ((v = lookup(builtinProto[vt_object], field, lVal, hash)).type != vt_undef)
		  return evalBuiltin(v, obj, &original, lVal, eval);

	  return v.bits = vt_undef, v;
	}

	//  examine prototype chain
	//	then examine baseVal builtins

	while (obj.type == vt_object) {
	  object_t *oval = obj.oval;

	  //  look for attribute
	  //  in the object

	  if ((v = lookup(obj, field, lVal, hash)).type != vt_undef)
		return evalBuiltin(v, obj, &original, lVal, eval);

	  if (lVal)
		break;

	  switch (phase) {
		case ProtoChain:
		  obj = oval->protoChain;

		  if (obj.type == vt_object)
			continue;

		  phase = ProtoBase;

		//	check original object builtins

		case ProtoBase:
		  phase = OriginalVal;

		  if (oval->protoBase) {
			obj = builtinProto[oval->protoBase];
			continue;
		  }

		//  check built-ins for original type

		case OriginalVal:
		  phase = AllDone;

		  obj = builtinProto[original.type];
		  continue;

		//	nothing found

		case AllDone:
		  break;
	  }

	  break;
	}

	v.bits = vt_undef;
	return v;
}

//	return vector value slot idx + 1 (>0)
//	or hash slot h (<= 0)

int lookupValue(value_t obj, value_t name, uint64_t hash, bool find) {
	dbobject_t *dboval = js_addr(obj);
	string_t *namestr = js_addr(name);
	int idx, h, hashMod, hashEnt;
	uint32_t start, cap;
	pair_t *pairs;
	void *hashTbl;
	
	pairs = obj.marshaled ? dboval->pairs : obj.oval->pairsPtr;
	cap = obj.marshaled ? dboval->cnt : vec_max(obj.oval->pairsPtr);

	if (!cap)
		return 0;

	if (!hash)
		hash = hashStr(namestr->val, namestr->len);

	hashEnt = hashBytes(cap);
	hashMod = 3 * cap / 2;
	hashTbl = pairs + cap;

	start = hash % hashMod;
	h = start;

	while ((idx = hashEntry(hashTbl, hashEnt, h))) {
	  if (find) {
		value_t v = pairs[idx - 1].name;
		string_t *keystr = v.marshaled ? js_dbaddr(v, obj.addr) : v.addr;

		if (keystr->len == namestr->len) {
		  if (!memcmp(keystr->val, namestr->val, namestr->len))
			return idx;
		}
	  }

	  if (++h == hashMod)
		h = 0;

	  if (h == start) {
		fprintf(stderr, "hash table overflow looking for %.*s\n", namestr->len, namestr->val);
		exit(0);
	  }
	}

	return -h;
}

//	
//	insert new object value

value_t *setAttribute(object_t *oval, value_t name, uint32_t h) {
	uint32_t cap = vec_max(oval->pairsPtr), idx, hashMod, hashEnt;
	string_t *namestr;
	void *hashTbl;
	pair_t pair;

	hashTbl = oval->pairsPtr + cap;
	hashEnt = hashBytes(cap);
	hashMod = 3 * cap / 2;

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

	  for (idx=0; idx < vec_cnt(oval->pairsPtr); idx++) {
		namestr = js_addr(oval->pairsPtr[idx].name);
		h = hashStr(namestr->val, namestr->len) % hashMod;

	  	while (hashEntry(hashTbl, hashEnt, h))
		  if (++h == hashMod)
			h = 0;

		hashStore(hashTbl, hashEnt, h, idx + 1);
	  }
	}

	//  return new slot value address

	idx = vec_cnt(oval->pairsPtr);
	return &oval->pairsPtr[idx - 1].value;
}

//	lookup value in object/dbobject

value_t lookup(value_t obj, value_t name, bool lVal, uint64_t hash) {
	dbobject_t *dboval = js_addr(obj);
	value_t v;
	int idx;

	v.bits = vt_undef;

	idx = lookupValue(obj, name, hash, true);

	if (idx > 0) {
	  if (obj.marshaled)
		v = dboval->pairs[idx - 1].value;
	  else
		v = obj.oval->pairsPtr[idx - 1].value;
	  if (v.marshaled)
		v.addr = obj.addr;
	} else
	  idx = -idx;

	if (!lVal)
	  return v;

	if (obj.marshaled)
	  obj = convDocument(obj, lVal);

	v.bits = vt_lval;
	v.lval = setAttribute(obj.oval, name, idx);
	return v;
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
	value_t *array = vec_cnt(args) ? args : thisVal;
	dbarray_t *dbaval = js_addr(*array);
	value_t ending, comma, ans[1];
	uint32_t idx = 0;
	value_t *values;
	uint32_t cnt;

	values = array->marshaled ? dbaval->valueArray : array->aval->valuePtr;
	cnt = array->marshaled ? dbaval->cnt : vec_cnt(values);

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
	value_t val;

	val.bits = vt_undef;
	return val;
}

value_t fcnObjectKeys(value_t *args, value_t *thisVal, environment_t *env) {
	value_t val;

	val.bits = vt_undef;
	return val;
}

value_t fcnObjectPreventExtensions(value_t *args, value_t *thisVal, environment_t *env) {
	value_t val;

	val.bits = vt_undef;
	return val;
}

value_t fcnObjectSeal(value_t *args, value_t *thisVal, environment_t *env) {
	value_t val;

	val.bits = vt_undef;
	return val;
}

value_t fcnObjectSetPrototypeOf(value_t *args, value_t *thisVal, environment_t *env) {
	value_t val;

	val.bits = vt_undef;
	return val;
}

value_t fcnObjectValues(value_t *args, value_t *thisVal, environment_t *env) {
	value_t val;

	val.bits = vt_undef;
	return val;
}

value_t fcnObjectIsExtensible(value_t *args, value_t *thisVal, environment_t *env) {
	value_t val;

	val.bits = vt_undef;
	return val;
}

value_t fcnObjectIsFrozen(value_t *args, value_t *thisVal, environment_t *env) {
	value_t val;

	val.bits = vt_undef;
	return val;
}

value_t fcnObjectIsSealed(value_t *args, value_t *thisVal, environment_t *env) {
	value_t val;

	val.bits = vt_undef;
	return val;
}

value_t fcnObjectCreate(value_t *args, value_t *thisVal, environment_t *env) {
	value_t val;

	val.bits = vt_undef;
	return val;
}

value_t fcnObjectEntries(value_t *args, value_t *thisVal, environment_t *env) {
	value_t val;

	val.bits = vt_undef;
	return val;
}

value_t fcnObjectFreeze(value_t *args, value_t *thisVal, environment_t *env) {
	value_t val;

	val.bits = vt_undef;
	return val;
}

value_t fcnObjectGetOwnPropDesc(value_t *args, value_t *thisVal, environment_t *env) {
	value_t val;

	val.bits = vt_undef;
	return val;
}

value_t fcnObjectGetOwnPropNames(value_t *args, value_t *thisVal, environment_t *env) {
	value_t val;

	val.bits = vt_undef;
	return val;
}

value_t fcnObjectGetOwnPropSymbols(value_t *args, value_t *thisVal, environment_t *env) {
	value_t val;

	val.bits = vt_undef;
	return val;
}

value_t fcnObjectDefineProp(value_t *args, value_t *thisVal, environment_t *env) {
	value_t val;

	val.bits = vt_undef;
	return val;
}

value_t fcnObjectDefineProps(value_t *args, value_t *thisVal, environment_t *env) {
	value_t val;

	val.bits = vt_undef;
	return val;
}
*/

value_t fcnObjectSetValue(value_t *args, value_t *thisVal, environment_t *env) {
	object_t *oval;
	value_t base;

	if (thisVal->marshaled)
		return base.bits = vt_undef, base;

	if (thisVal->objvalue)
		oval = js_addr(*thisVal->lval);
	else
		oval = js_addr(*thisVal);

	if (vec_cnt(args))
		base = args[0];
	else
		base.bits = vt_undef;

	replaceSlot(oval->baseVal, base);
	return base;
}

value_t fcnObjectHasOwnProperty(value_t *args, value_t *thisVal, environment_t *env) {
	value_t val, obj;

	if (thisVal->objvalue)
		obj = *thisVal->lval;
	else
		obj = *thisVal;

	val.bits = vt_bool;

	if (vec_cnt(args))
		val.boolean = lookup(obj, args[0], false, 0).type == vt_undef ? false : true;
	else
		val.boolean = false;

	return val;
}

value_t fcnObjectValueOf(value_t *args, value_t *thisVal, environment_t *env) {
	value_t obj;

	if (vec_cnt(args))
		obj = args[0];
	else if (thisVal->objvalue)
		obj = *thisVal->lval;
	else
		obj = *thisVal;

	if (obj.marshaled)
		return obj;

	if (obj.oval->baseVal->type == vt_undef)
		return *thisVal;

	return *obj.oval->baseVal;
}

/*
value_t fcnObjectLock(value_t *args, value_t *thisVal, environment_t *env) {
	object_t *oval;
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
	object_t *oval;
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
	value_t *obj = vec_cnt(args) ? args : thisVal;
	value_t colon, ending, comma, ans[1];
	dbobject_t *dboval = js_addr(*obj);
	uint32_t idx = 0;
	pair_t *pairs;
	uint32_t cnt;

	pairs = obj->marshaled ? dboval->pairs : obj->oval->pairsPtr;
	cnt = obj->marshaled ? dboval->cnt : vec_cnt(pairs);

	ans->bits = vt_string;
	ans->addr = &LeftBraceStr;

	colon.bits = vt_string;
	colon.addr = &ColonStr;

	comma.bits = vt_string;
	comma.addr = &CommaStr;

	while (idx < cnt) {
		value_t v = pairs[idx].name;

		if (v.marshaled)
			v.addr = obj->addr;

		v = conv2Str(v, false, true);
		valueCat(ans, v, true);
		valueCat(ans, colon, false);

		v = pairs[idx].value;

		if (v.marshaled)
			v.addr = obj->addr;

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
	value_t ref, *proto = &val.oval->protoChain;

	if (val.marshaled)
		return builtinProto[vt_object];

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
	dbarray_t *dbaval = js_addr(*thisVal);
	value_t slice, end;
	int start, count;
	value_t *values;
	value_t array;
	int idx, cnt;

	values = thisVal->marshaled ? dbaval->valueArray : thisVal->aval->valuePtr;
	cnt = thisVal->marshaled ? dbaval->cnt : vec_cnt(values);

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

	array = newArray(array_value, count);

	for (idx = 0; idx < count; idx++) {
		value_t nxt = values[start + idx];
		array.aval->valuePtr[idx] = nxt;
		incrRefCnt(nxt);
	}

	return array;
}

value_t fcnArrayConcat(value_t *args, value_t *thisVal, environment_t *env) {
	value_t array = newArray(array_value, 0);
	dbarray_t *dbaval = js_addr(*thisVal);
	value_t *values;
	int idx, cnt;

	values = thisVal->marshaled ? dbaval->valueArray : thisVal->aval->valuePtr;
	cnt = thisVal->marshaled ? dbaval->cnt : vec_cnt(values);

	//  clone existing array values

	for (idx = 0; idx < cnt; idx++) {
		value_t nxt = values[idx];
		vec_push(array.aval->valuePtr, nxt);
		incrRefCnt(nxt);
	}

	//  append new argument elements

	for (idx = 0; idx < vec_cnt(args); idx++) {

	  if (args[idx].type == vt_array) {
	    dbarray_t *nxt = js_addr(args[idx]);
	    value_t *nxtvalues = args[idx].marshaled ? nxt->valueArray : args[idx].aval->valuePtr;
	    int nxtcnt = args[idx].marshaled ? nxt->cnt : vec_cnt(nxtvalues);

		for (int j = 0; j < nxtcnt; j++) {
		  vec_push(array.aval->valuePtr, nxtvalues[j]);
		  incrRefCnt(nxtvalues[j]);
		}
	  } else {
		vec_push (array.aval->valuePtr, args[idx]);
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
	dbarray_t *dbaval = js_addr(*thisVal);
	value_t delim, val[1], v;
	value_t *values;
	int cnt;

	values = thisVal->marshaled ? dbaval->valueArray : thisVal->aval->valuePtr;
	cnt = thisVal->marshaled ? dbaval->cnt : vec_cnt(values);

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
	value_t undef;

	if (vec_cnt(args))
		undef = args[0];
	else
		undef.bits = vt_undef;

	if (!thisVal->marshaled)
		thisVal->aval->obj = undef;

	return undef;
}

value_t propArrayProto(value_t val, bool lval) {
	value_t ref;

	if (val.marshaled)
		return ref.bits = vt_undef, ref;

	if (val.type == vt_lval)
		val.lval = &val.oval->protoChain;
	else
		val = val.oval->protoChain;

	return val;
}

value_t propArrayLength(value_t val, bool lval) {
	value_t num;
	
	num.bits = vt_int;

	if (val.marshaled) {
		dbarray_t *dbaval = js_addr(val);
		num.nval = dbaval->cnt;
	} else
		num.nval = vec_cnt(val.aval->valuePtr);

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


#include "js.h"
#include "js_props.h"
#include "js_object.h"

#define firstCapacity 10

value_t newObject(value_t prototype) {
	value_t v;

	v.bits = vt_object;
	v.oval = js_alloc(sizeof(object_t),true);
	v.oval->protoChain = prototype;

	v.refcount = 1;

	incrRefCnt(v.oval->protoChain);
	return v;
}

value_t newArray(enum ArrayType subType) {
	value_t v;

	v.bits = vt_array;
	v.aval = js_alloc(sizeof(array_t), true);
	v.aval->obj = newObject(builtinProto[vt_array]);

	v.subType = subType;
	v.objvalue = 1;
	v.refcount = 1;
	return v;
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
	switch (left.subType) {
	case array_value:
		*left.lval = right;
		return;
	case array_int8:
		*(int8_t *)left.slot = conv2Int(right, true).nval;
		return;
	case array_uint8:
		*(uint8_t *)left.slot = conv2Int(right, true).nval;
		return;
	case array_int16:
		*(int16_t *)left.slot = conv2Int(right, true).nval;
		return;
	case array_uint16:
		*(uint16_t *)left.slot = conv2Int(right, true).nval;
		return;
	case array_int32:
		*(int32_t *)left.slot = conv2Int(right, true).nval;
		return;
	case array_uint32:
		*(uint32_t *)left.slot = conv2Int(right, true).nval;
		return;
	case array_float32:
		*(float *)left.slot = conv2Dbl(right, true).dbl;
		return;
	case array_float64:
		*(double *)left.slot = conv2Dbl(right, true).dbl;
		return;
	}
}

uint64_t hashStr(value_t s) {
	uint32_t len = s.aux;
	uint64_t hash = 0;
	uint64_t mask;

	while (len>=8) {
		len -= 8;
		hash += *((uint64_t *) &s.str[len]);
		hash *= 5;
	}

	mask = 1ULL << len * 8;
	return hash += --mask & (*((uint64_t *) &s.str[0]));
}

value_t getDocName(document_t *doc, uint32_t idx) {
	value_t v = doc->pairs[idx].name;

	if (v.rebaseptr)
		v.rebase = (uint8_t *)doc - doc->base + v.offset;

	return v;
}

value_t getDocValue(document_t *doc, uint32_t idx) {
	value_t v = doc->pairs[idx].value;

	if (v.rebaseptr)
		v.rebase = (uint8_t *)doc - doc->base + v.offset;

	return v;
}

value_t getDocArray(docarray_t *array, uint32_t idx) {
	value_t v = array->values[idx];

	if (v.rebaseptr)
		v.rebase = (uint8_t *)array - array->base + v.offset;

	return v;
}

void hashStore(void *table, uint32_t capacity, uint32_t idx, uint32_t val) {
	if (capacity < 256)
		((uint8_t *)table)[idx] = val;
	else if (capacity < 65536)
		((uint16_t *)table)[idx] = val;
	else
		((uint32_t *)table)[idx] = val;
}

uint32_t hashEntry(void *table, uint32_t capacity, uint32_t idx) {
	if (capacity < 256)
		return ((uint8_t *)table)[idx];
	if (capacity < 65536)
		return ((uint16_t *)table)[idx];

	return ((uint32_t *)table)[idx];
}

value_t lookupDoc(document_t *doc, value_t name) {
	void *hashTbl = (uint32_t *)(doc->pairs + doc->count);
	uint8_t *rebase = (uint8_t *)doc;
	uint32_t h, start, idx;
	value_t v;
	
	if (doc->capacity)
		h = start = hashStr(name) % doc->capacity;
	else
		return v.bits = vt_undef, v;

	while ((idx = hashEntry(hashTbl, doc->capacity, h))) {
		value_t key = doc->pairs[idx - 1].name;

		if (key.rebaseptr)
			key.rebase = rebase - doc->base + key.offset;

		if (key.aux == name.aux)
			if (!memcmp(key.str, name.str, name.aux)) {
				v = doc->pairs[idx - 1].value;
				if (v.rebaseptr)
					v.rebase = rebase - doc->base + v.offset;
				return v;
			}

		h = (h+1) % doc->capacity;

		if (h == start)
			break;
	}

	v.bits = vt_undef;
	return v;
}

value_t *lookup(object_t *obj, value_t name, bool lVal, bool noProtoChain) {
	uint64_t hash = hashStr(name);
	uint32_t idx, h;
	pair_t pair;
	
retry:
	if (obj->capacity) {
	  uint32_t start = hash % obj->capacity;

	  h = start;

	  while ((idx = hashEntry(obj->hashTbl, obj->capacity, h))) {
		pair_t *key = obj->pairs + idx - 1;

		if (key->name.aux == name.aux)
			if (!memcmp(key->name.str, name.str, name.aux))
				return &key->value;

		h = (h+1) % obj->capacity;

		if (h == start)
			break;
	  }
	}

	if (noProtoChain)
		return NULL;

	if (!lVal) {
	  if (obj->protoChain.type == vt_undef)
		return NULL;

	  obj = obj->protoChain.oval;
	  goto retry;
	}

	pair.value.bits = vt_undef;
	pair.name = name;
	incrRefCnt(name);

	vec_push(obj->pairs, pair);

	//  is the hash table over filled?

	if (4*vec_count(obj->pairs) > 3*obj->capacity) {
		int entSize = sizeof(uint32_t);

		if (obj->hashTbl)
			js_free(obj->hashTbl);

		if (obj->capacity)
			obj->capacity *= 2;
		else
			obj->capacity = firstCapacity;

		// rehash current entries

		if (obj->capacity + 1 < 256)
			entSize = sizeof(uint8_t);
		else if (obj->capacity + 1 < 65536)
			entSize = sizeof(uint16_t);

		obj->hashTbl = js_alloc(obj->capacity * entSize, true);

		for (int i=0; i< vec_count(obj->pairs); i++) {
			h = hashStr(obj->pairs[i].name) % obj->capacity;

	  		while (hashEntry(obj->hashTbl, obj->capacity, h))
				h = (h+1) % obj->capacity;

			hashStore(obj->hashTbl, obj->capacity, h, i + 1);
		}
	} else
		hashStore(obj->hashTbl, obj->capacity, h, vec_count(obj->pairs));

	return &obj->pairs[vec_count(obj->pairs)-1].value;
}

// TODO -- remove the field from the name & value vectors and hash table

value_t *deleteField(object_t *obj, value_t name) {
	uint32_t idx, start, h;

	if (obj->capacity)
		h = hashStr(name) % obj->capacity;
	else
		return NULL;

	start = h;

	do {
		if ((idx = hashEntry(obj->hashTbl, obj->capacity, h))) {
			pair_t *key = obj->pairs + idx - 1;

			if (key->name.aux == name.aux)
				if (!memcmp(key->name.str, name.str, name.aux))
					return &obj->pairs[idx - 1].value;
		}

		h = (h+1) % obj->capacity;
	} while (h != start);

	// not there
	return NULL;
}

value_t js_objectOp (uint32_t args, environment_t *env) {
	value_t arglist, op, thisVal, s;

	arglist = eval_arg(&args, env);
	s.bits = vt_status;
	int cnt;

	if (arglist.type != vt_array) {
		fprintf(stderr, "Error: objectOp => expecting argument array => %s\n", strtype(arglist.type));
		return s.status = ERROR_script_internal, s;
	}

	cnt = vec_count(arglist.aval->values);

	if (cnt > 0)
		thisVal = arglist.aval->values[0];
	else {
		fprintf(stderr, "Error: objectOp => expecting object argument\n");
		return s.status = ERROR_script_internal, s;
	}

	if (thisVal.objvalue)
		thisVal = *thisVal.lval;

	if (thisVal.type != vt_object) {
		fprintf(stderr, "Error: objectOp => expecting object type argument => %s\n", strtype(thisVal.type));
		return s.status = ERROR_script_internal, s;
	}

	op = eval_arg(&args, env);

	if (op.type != vt_int) {
		fprintf(stderr, "Error: objectOp => expecting integer argument => %s\n", strtype(op.type));
		return s.status = ERROR_script_internal, s;
	}

	switch (op.nval) {
	case obj_keys: {
		value_t array = newArray(array_value);

		for (int idx = 0; idx < vec_count(thisVal.oval->pairs); idx++) {
			value_t name = thisVal.oval->pairs[idx].name;
			vec_push (array.aval->values, name);
			incrRefCnt (name);
		}

		return array;
	}
	case obj_values: {
		value_t array = newArray(array_value);

		for (int idx = 0; idx < vec_count(thisVal.oval->pairs); idx++) {
			value_t value = thisVal.oval->pairs[idx].value;
			vec_push (array.aval->values, value);
			incrRefCnt (value);
		}

		return array;
	}
	case obj_setprototype: {
		value_t prototype;

		if (cnt > 1)
			prototype = arglist.aval->values[1];
		else {
			fprintf(stderr, "Error: setPrototypeOf => expecting prototype argument\n");
			return s.status = ERROR_script_internal, s;
		}

		if (prototype.type == vt_undef || prototype.type == vt_object)
			replaceSlot(&thisVal.oval->protoChain, prototype);
		else {
			fprintf(stderr, "Error: setPrototypeOf => expecting object argument: %s\n", strtype(prototype.type));
			return s.status = ERROR_script_internal, s;
		}

		return thisVal;
	}
	}

	s.status = ERROR_script_internal;
	return s;
}

value_t fcnArrayToString(value_t *args, value_t *thisVal) {
	value_t ending, comma, ans[1];
	array_t *aval = thisVal->aval;
	uint32_t idx = 0;

	ans->bits = vt_string;
	ans->string = "[";
	ans->aux = 1;

	comma.bits = vt_string;
	comma.string = ",";
	comma.aux = 1;

	while (idx < vec_count(aval->values)) {
		valueCat(ans, conv2Str(aval->values[idx], false, true));

		if (++idx < vec_count(aval->values))
			valueCat(ans, comma);
	}

	ending.bits = vt_string;
	ending.string = "]";
	ending.aux = 1;

	valueCat(ans, ending);
	return *ans;
}

value_t fcnDocArrayToString(value_t *args, value_t *thisVal) {
	docarray_t *array = thisVal->docarray;
	value_t ending, comma, ans[1];
	uint32_t idx = 0;

	ans->bits = vt_string;
	ans->string = "[";
	ans->aux = 1;

	comma.bits = vt_string;
	comma.string = ",";
	comma.aux = 1;

	while (idx < array->count) {
		valueCat(ans, conv2Str(getDocArray(array, idx), false, true));

		if (++idx < array->count)
			valueCat(ans, comma);
	}

	ending.bits = vt_string;
	ending.string = "]";
	ending.aux = 1;

	valueCat(ans, ending);
	return *ans;
}

value_t fcnDocToString(value_t *args, value_t *thisVal) {
	value_t colon, ending, comma, ans[1];
	document_t *doc = thisVal->document;
	uint32_t idx = 0;

	ans->bits = vt_string;
	ans->string = "{";
	ans->aux = 1;

	colon.bits = vt_string;
	colon.string = ":";
	colon.aux = 1;

	comma.bits = vt_string;
	comma.string = ",";
	comma.aux = 1;

	while (idx < doc->count) {
		valueCat(ans, getDocName(doc, idx));
		valueCat(ans, colon);

		valueCat(ans, conv2Str(getDocValue(doc, idx), false, true));

		if (++idx < doc->count)
			valueCat(ans, comma);
	}

	ending.bits = vt_string;
	ending.string = "}";
	ending.aux = 1;
	valueCat(ans, ending);
	return *ans;
}

value_t fcnObjectIs(value_t *args, value_t *thisVal) {
	value_t val, obj = *thisVal;

	if (obj.objvalue)
		obj = *obj.lval;

	val.bits = vt_undef;
	return val;
}

value_t fcnObjectKeys(value_t *args, value_t *thisVal) {
	value_t val, obj = *thisVal;

	if (obj.objvalue)
		obj = *obj.lval;

	val.bits = vt_undef;
	return val;
}

value_t fcnObjectPreventExtensions(value_t *args, value_t *thisVal) {
	value_t val, obj = *thisVal;

	if (obj.objvalue)
		obj = *obj.lval;

	val.bits = vt_undef;
	return val;
}

value_t fcnObjectSeal(value_t *args, value_t *thisVal) {
	value_t val, obj = *thisVal;

	if (obj.objvalue)
		obj = *obj.lval;

	val.bits = vt_undef;
	return val;
}

value_t fcnObjectSetPrototypeOf(value_t *args, value_t *thisVal) {
	value_t val, obj = *thisVal;

	if (obj.objvalue)
		obj = *obj.lval;

	val.bits = vt_undef;
	return val;
}

value_t fcnObjectValues(value_t *args, value_t *thisVal) {
	value_t val, obj = *thisVal;

	if (obj.objvalue)
		obj = *obj.lval;

	val.bits = vt_undef;
	return val;
}

value_t fcnObjectIsExtensible(value_t *args, value_t *thisVal) {
	value_t val, obj = *thisVal;

	if (obj.objvalue)
		obj = *obj.lval;

	val.bits = vt_undef;
	return val;
}

value_t fcnObjectIsFrozen(value_t *args, value_t *thisVal) {
	value_t val, obj = *thisVal;

	if (obj.objvalue)
		obj = *obj.lval;

	val.bits = vt_undef;
	return val;
}

value_t fcnObjectIsSealed(value_t *args, value_t *thisVal) {
	value_t val, obj = *thisVal;

	if (obj.objvalue)
		obj = *obj.lval;

	val.bits = vt_undef;
	return val;
}

value_t fcnObjectCreate(value_t *args, value_t *thisVal) {
	value_t val, obj = *thisVal;

	if (obj.objvalue)
		obj = *obj.lval;

	val.bits = vt_undef;
	return val;
}

value_t fcnObjectEntries(value_t *args, value_t *thisVal) {
	value_t val, obj = *thisVal;

	if (obj.objvalue)
		obj = *obj.lval;

	val.bits = vt_undef;
	return val;
}

value_t fcnObjectFreeze(value_t *args, value_t *thisVal) {
	value_t val, obj = *thisVal;

	if (obj.objvalue)
		obj = *obj.lval;

	val.bits = vt_undef;
	return val;
}

value_t fcnObjectGetOwnPropDesc(value_t *args, value_t *thisVal) {
	value_t val, obj = *thisVal;

	if (obj.objvalue)
		obj = *obj.lval;

	val.bits = vt_undef;
	return val;
}

value_t fcnObjectGetOwnPropNames(value_t *args, value_t *thisVal) {
	value_t val, obj = *thisVal;

	if (obj.objvalue)
		obj = *obj.lval;

	val.bits = vt_undef;
	return val;
}

value_t fcnObjectGetOwnPropSymbols(value_t *args, value_t *thisVal) {
	value_t val, obj = *thisVal;

	if (obj.objvalue)
		obj = *obj.lval;

	val.bits = vt_undef;
	return val;
}

value_t fcnObjectDefineProp(value_t *args, value_t *thisVal) {
	value_t val, obj = *thisVal;

	if (obj.objvalue)
		obj = *obj.lval;

	val.bits = vt_undef;
	return val;
}

value_t fcnObjectDefineProps(value_t *args, value_t *thisVal) {
	value_t val, obj = *thisVal;

	if (obj.objvalue)
		obj = *obj.lval;

	val.bits = vt_undef;
	return val;
}


value_t fcnObjectSetBaseVal(value_t *args, value_t *thisVal) {
	value_t undef, obj = *thisVal;

	if (obj.objvalue)
		obj = *obj.lval;

	if (vec_count(args))
		undef = args[0];
	else
		undef.bits = vt_undef;

	return obj.oval->base = undef;
}

value_t fcnObjectHasOwnProperty(value_t *args, value_t *thisVal) {
	value_t val, obj = *thisVal;

	if (obj.objvalue)
		obj = *obj.lval;

	val.bits = vt_bool;

	if (vec_count(args))
		val.boolean = lookup(obj.oval, args[0], false, true) ? true : false;
	else
		val.boolean = false;

	return val;
}

value_t fcnObjectValueOf(value_t *args, value_t *thisVal) {
	value_t obj = *thisVal;

	if (obj.objvalue)
		obj = *obj.lval;

	if (obj.oval->base.type == vt_undef)
		return *thisVal;

	return obj.oval->base;
}

/*
value_t fcnObjectLock(value_t *args, value_t *thisVal) {
	value_t val, obj = *thisVal;

	if (obj.objvalue)
		obj = *obj.lval;

	if (vec_count(args) > 0)
		mode = conv2Int(args[0], false);
	else
		return val.bits = vt_undef, val;

	if (mode.type != vt_int)
		return val.bits = vt_undef, val;

	switch (mode.nval) {
	case 0:	readLock(obj.oval->lock); break;
	case 1:	writeLock(obj.oval->lock); break;
	}

	val.bits = vt_bool;
	val.boolean = true;
	return val;
}

value_t fcnObjectUnlock(value_t *args, value_t **thisVal) {
	value_t val, obj = *thisVal;

	if (obj.objvalue)
		obj = *obj.lval;

	rwUnlock(obj.oval->lock);
	val.bits = vt_bool;
	val.boolean = true;
	return val;
}
*/
value_t fcnObjectToString(value_t *args, value_t *thisVal) {
	value_t colon, ending, comma, ans[1];
	object_t *oval = thisVal->oval;
	uint32_t idx = 0;

	ans->bits = vt_string;
	ans->str = "{";
	ans->aux = 1;

	colon.bits = vt_string;
	colon.string = ":";
	colon.aux = 1;

	comma.bits = vt_string;
	comma.string = ",";
	comma.aux = 1;

	while (idx < vec_count(oval->pairs)) {
		valueCat(ans, conv2Str(oval->pairs[idx].name, false, true));
		valueCat(ans, colon);
		valueCat(ans, conv2Str(oval->pairs[idx].value, false, true));

		if (++idx < vec_count(oval->pairs))
			valueCat(ans, comma);
	}

	ending.bits = vt_string;
	ending.string = "}";
	ending.aux = 1;

	valueCat(ans, ending);
	return *ans;
}

value_t propObjProto(value_t val, bool lVal) {
	value_t ref, *proto = &val.oval->protoChain;

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

value_t fcnArraySlice(value_t *args, value_t *thisVal) {
	int idx, cnt = vec_count(thisVal->aval->values);
	value_t array = newArray(array_value);
	value_t slice, end;
	int start, count;

	if (vec_count(args) > 0)
		slice = conv2Int(args[0], false);
	else {
		slice.bits = vt_int;
		slice.nval = 0;
	}

	if (vec_count(args) > 1)
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
		value_t nxt = thisVal->aval->values[start + idx];
		vec_push(array.aval->values, nxt);
		incrRefCnt(nxt);
	}

	return array;
}

value_t fcnArrayConcat(value_t *args, value_t *thisVal) {
	value_t array = newArray(array_value);
	int idx;

	for (idx = 0; idx < vec_count(thisVal->aval->values); idx++) {
		value_t nxt = thisVal->aval->values[idx];
		vec_push(array.aval->values, nxt);
		incrRefCnt(nxt);
	}

	for (idx = 0; idx < vec_count(args); idx++) {
	  if (args[idx].type == vt_array) {
		for (int j = 0; j < vec_count(args[idx].aval->values); j++) {
		  vec_push(array.aval->values, args[idx].aval->values[j]);
		  incrRefCnt(args[idx].aval->values[j]);
		}
	  } else if (args[idx].type == vt_object) {
		vec_push (array.aval->values, args[idx]);
		incrRefCnt(args[idx]);
	  } else {
		vec_push (array.aval->values, args[idx]);
		incrRefCnt(args[idx]);
	  }
	}

	return array;
}

value_t fcnArrayValueOf(value_t *args, value_t *thisVal) {
	return *thisVal;
}

value_t fcnArrayJoin(value_t *args, value_t *thisVal) {
	uint32_t len = 0, off = 0;
	value_t delim, val, next;
	value_t *values = NULL;

	if (vec_count(args) > 0)
		delim = conv2Str(args[0], false, true);
	else {
		delim.bits = vt_string;
		delim.string = ",";
		delim.aux = 1;
	}

	for (int idx = 0; idx < vec_count(thisVal->aval->values); idx++) {
		value_t v = thisVal->aval->values[idx];

		switch (v.type) {
		case vt_null:	continue;
		case vt_undef:	continue;
		default: break;
		}

		val = conv2Str(v, false, true);
		vec_push(values, val);
		len += val.aux;

		if (idx < vec_count(thisVal->aval->values) - 1)
			len += delim.aux;
	}

	val.bits = vt_string;
	val.aux = len;
	val.str = js_alloc(len + 1, false);

	val.refcount = 1;
	val.str[len] = 0;

	for (int idx = 0; idx < vec_count(values); idx++) {
		next = values[idx];

		memcpy(val.str + off, next.str, next.aux);
		off += next.aux;

		if (idx < vec_count(values) - 1) {
			memcpy(val.str + off, delim.str, delim.aux);
			off += delim.aux;
		}

		abandonValue(next);
	}

	assert(off == len);

	abandonValue (delim);
	return val;
}

/*
value_t fcnArrayLock(value_t *args, value_t *thisVal) {
	value_t val, mode;

	if (vec_count(args) > 0)
		mode = conv2Int(args[0], false);
	else
		return val.bits = vt_undef, val;

	if (mode.type != vt_int)
		return val.bits = vt_undef, val;

	switch (mode.nval) {
	case 0:	readLock(thisVal->aval->lock); break;
	case 1:	writeLock(thisVal->aval->lock); break;
	}

	val.bits = vt_bool;
	val.boolean = true;
	return val;
}

value_t fcnArrayUnlock(value_t *args, value_t *thisVal) {
	array_t *array = thisVal->aval;
	value_t val;

	rwUnlock(array->lock);
	val.bits = vt_bool;
	val.boolean = true;
	return val;
}
*/
value_t fcnArraySetBaseVal(value_t *args, value_t *thisVal) {
	value_t undef;

	if (vec_count(args))
		undef = args[0];
	else
		undef.bits = vt_undef;

	return thisVal->aval->obj = undef;
}

value_t propArrayProto(value_t val, bool lVal) {
	value_t ref;

	if (!lVal)
		return val.aval->obj.oval->protoChain;

	ref.bits = vt_lval;
	ref.lval = &val.aval->obj.oval->protoChain;
	return ref;
}

value_t propArrayLength(value_t val, bool lVal) {
	value_t num;

	num.bits = vt_int;
	num.nval = vec_count(val.aval->values);
	return num;
}

struct PropVal builtinObjProp[] = {
	{ propObjLength, "length"},
	{ propObjProto, "prototype", true },
	{ NULL, NULL}
};

struct PropFcn builtinObjFcns[] = {
//	{ fcnObjectLock, "lock" },
//	{ fcnObjectUnlock, "unlock" },
	{ fcnObjectIs, "is", true },
	{ fcnObjectKeys, "keys", true },
	{ fcnObjectPreventExtensions, "preventExtensions", true },
	{ fcnObjectSeal, "seal", true },
	{ fcnObjectSetPrototypeOf, "setPrototypeOf", true },
	{ fcnObjectValues, "values", true },
	{ fcnObjectIsExtensible, "isExtensible", true },
	{ fcnObjectIsFrozen, "isFrozen", true },
	{ fcnObjectIsSealed, "isSealed", true },
	{ fcnObjectCreate, "create", true },
	{ fcnObjectEntries, "entries", true },
	{ fcnObjectFreeze, "freeze", true },
	{ fcnObjectGetOwnPropDesc, "GetOwnPropertyDescriptors", true },
	{ fcnObjectGetOwnPropNames, "GetOwnPropertyNames", true },
	{ fcnObjectGetOwnPropSymbols, "GetOwnPropertySymbols", true },
	{ fcnObjectDefineProp, "defineProperty", true },
	{ fcnObjectDefineProps, "defineProperties", true },
	{ fcnObjectValueOf, "valueOf" },
	{ fcnObjectToString, "toString" },
	{ fcnObjectSetBaseVal, "__setBaseVal" },
	{ fcnObjectHasOwnProperty, "hasOwnProperty" },
	{ NULL, NULL}
};

struct PropVal builtinArrayProp[] = {
	{ propArrayLength, "length" },
	{ propArrayProto, "prototype", true },
	{ NULL, NULL}
};

struct PropFcn builtinArrayFcns[] = {
//	{ fcnArrayLock, "lock" },
//	{ fcnArrayUnlock, "unlock" },
	{ fcnArrayToString, "toString" },
	{ fcnArraySetBaseVal, "__setBaseVal" },
	{ fcnArrayValueOf, "valueOf" },
	{ fcnArrayConcat, "concat" },
	{ fcnArraySlice, "slice" },
	{ fcnArrayJoin, "join" },
	{ NULL, NULL}
};


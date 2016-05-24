#include "jsdb.h"
#include "jsdb_object.h"

#define firstCapacity 10

value_t newObject() {
	value_t v;
	v.bits = vt_object;
	v.oval = jsdb_alloc(sizeof(object_t),true);
	v.refcount = 1;
	return v;
}

value_t newArray(enum ArrayType subType) {
	value_t v;
	v.bits = vt_array;
	v.aval = jsdb_alloc(sizeof(array_t), true);
	v.subType = subType;
	v.refcount = 1;
	return v;
}

value_t convArray2Value(void *val, enum ArrayType type) {
	value_t result;

	switch (type) {
	case array_value:
		return *(value_t *)val;
	case array_handle:
		return *(handle_t *)val;
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

value_t *lookup(object_t *obj, value_t name, bool addBit) {
	uint64_t hash = hashStr(name);
	uint32_t idx, h;
	pair_t pair;
	value_t v;
	
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

	if (!addBit) {
		if ((obj = obj->proto))
			goto retry;
		else
			return NULL;
	}

	pair.value.bits = vt_undef;
	pair.name = name;
	incrRefCnt(name);

	vec_push(obj->pairs, pair);

	//  is the hash table over filled?

	if (4*vec_count(obj->pairs) > 3*obj->capacity) {
		int entSize = sizeof(uint32_t);

		if (obj->hashTbl)
			jsdb_free(obj->hashTbl);

		if (obj->capacity)
			obj->capacity *= 2;
		else
			obj->capacity = firstCapacity;

		// rehash current entries

		if (obj->capacity + 1 < 256)
			entSize = sizeof(uint8_t);
		else if (obj->capacity + 1 < 65536)
			entSize = sizeof(uint16_t);

		obj->hashTbl = jsdb_alloc(obj->capacity * entSize, true);

		for (int i=0; i< vec_count(obj->pairs); i++) {
			uint32_t h = hashStr(obj->pairs[i].name) % obj->capacity;

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

value_t jsdb_objectOp (uint32_t args, environment_t *env) {
	value_t arglist, op, thisVal, s;
	int openum;

	arglist = eval_arg(&args, env);
	s.bits = vt_status;

	if (arglist.type != vt_array) {
		fprintf(stderr, "Error: mathop => expecting argument array => %s\n", strtype(arglist.type));
		return s.status = ERROR_script_internal, s;
	}

	thisVal = arglist.aval->values[0];

	if (thisVal.type != vt_object) {
		fprintf(stderr, "Error: mathop => expecting object argument => %s\n", strtype(thisVal.type));
		return s.status = ERROR_script_internal, s;
	}

	op = eval_arg(&args, env);

	if (op.type != vt_int) {
		fprintf(stderr, "Error: mathop => expecting integer argument => %s\n", strtype(op.type));
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
	}

	s.status = ERROR_script_internal;
	return s;
}

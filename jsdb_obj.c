#include <stdlib.h>
#include "jsdb.h"

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

value_t indexDoc(document_t *doc, uint32_t idx) {
	value_t v = doc->names[doc->count + idx - 1];

	if (v.rebaseptr)
		v.rebase = (uint8_t *)doc - doc->base + v.offset;

	return v;
}

value_t lookupDoc(document_t *doc, value_t name) {
	uint32_t *hash = (uint32_t *)(doc->names + doc->count * 2);
	uint8_t *rebase = (uint8_t *)doc;
	uint32_t h, start, idx;
	value_t v;
	
	h = start = hashStr(name) % doc->capacity;

	while ((idx = hash[h])) {
		value_t key = doc->names[idx - 1];

		if (key.rebaseptr)
			key.rebase = rebase - doc->base + key.offset;

		if (key.aux == name.aux)
			if (!memcmp(key.str, name.str, name.aux)) {
				v = doc->names[doc->count + idx - 1];
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
	value_t v;
	
retry:
	if (obj->capacity) {
	  uint32_t start = hash % obj->capacity;

	  h = start;

	  while ((idx = obj->hashmap[h])) {
		value_t *key = obj->names + idx - 1;

		if (key->aux == name.aux)
			if (!memcmp(key->str, name.str, name.aux))
				return obj->values + idx - 1;

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

	v.bits = vt_undef;

	incrRefCnt(name);
	vec_push(obj->names, name);
	vec_push(obj->values, v);

	//  is the hash table over filled?

	if (4*vec_count(obj->names) > 3*obj->capacity) {
		if (obj->hashmap)
			jsdb_free(obj->hashmap);

		if (obj->capacity)
			obj->capacity *= 2;
		else
			obj->capacity = firstCapacity;

		// rehash current entries

		obj->hashmap = jsdb_alloc(obj->capacity * sizeof(uint32_t), true);

		for (int i=0; i< vec_count(obj->names); i++) {
			uint32_t h = hashStr(obj->names[i]) % obj->capacity;
			
			while (obj->hashmap[h])
				h = (h+1) % obj->capacity;

			obj->hashmap[h] = i + 1;
		}
	} else
		obj->hashmap[h] = vec_count(obj->names);

	return &obj->values[vec_count(obj->values)-1];
}

// TODO -- remove the field from the name & value vectors

value_t *deleteField(object_t *obj, value_t name) {
	uint32_t idx, start, h = hashStr(name) % obj->capacity;
	start = h;

	do {
		if ((idx = obj->hashmap[h])) {
			value_t *key = obj->names + idx - 1;

			if (key->aux == name.aux)
				if (!memcmp(key->str, name.str, name.aux))
					return obj->values + idx - 1;
		}

		h = (h+1) % obj->capacity;
	} while (h != start);

	// not there
	return NULL;
}

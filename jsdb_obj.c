#include <stdlib.h>
#include "jsdb.h"

value_t newObject() {
	value_t v;
	v.bits = vt_object;
	v.oval = jsdb_alloc(sizeof(object_t),true);
	v.oval->hashmap = jsdb_alloc(10*sizeof(uint32_t), true);
	v.oval->capacity = 10;
	v.oval->values = NULL;
	v.oval->names = NULL;
	v.refcount = 1;
	return v;
}

value_t newArray() {
	value_t v;
	v.bits = vt_array;
	v.aval = jsdb_alloc(sizeof(array_t), true);
	v.refcount = 1;
	return v;
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
	uint32_t h, start, idx;
	value_t v;
	
	while (!obj->capacity)
	  if (!addBit) {
		if (obj->proto.type == vt_object) {
		  obj = obj->proto.oval;
		  continue;
		} else
		  return NULL;
	  } else {
		obj->hashmap = jsdb_alloc(10*sizeof(uint32_t), true);
		obj->capacity = 10;
		h = hash % 10;
		v.bits = vt_undef;

		incrRefCnt(name);
		obj->hashmap[hash % 10] = 1;
		vec_push(obj->names, name);
		vec_push(obj->values, v);
		return &obj->values[0];
	  }

	start = hash % obj->capacity;

retry:
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

	if (!addBit) {
		if (obj->proto.type == vt_object) {
			obj = obj->proto.oval;
			goto retry;
		}

		return NULL;
	}

	v.bits = vt_undef;

	incrRefCnt(name);
	vec_push(obj->names, name);
	vec_push(obj->values, v);

	obj->hashmap[h] = vec_count(obj->names);

	//  is the hash table over filled?

	if (4*vec_count(obj->names) > 3*obj->capacity) {
		jsdb_free(obj->hashmap);
		obj->capacity *= 2;

		// rehash current entries

		obj->hashmap = jsdb_alloc(obj->capacity * sizeof(uint32_t), true);

		for (int i=0; i< vec_count(obj->names); i++) {
			h = hashStr(obj->names[i]) % obj->capacity;
			
			while (obj->hashmap[h])
				h = (h+1) % obj->capacity;

			obj->hashmap[h] = i + 1;
		}
	}

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

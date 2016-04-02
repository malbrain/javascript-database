#include <stdlib.h>
#include "jsdb.h"

value_t newObject() {
	value_t v;
	v.bits = vt_object;
	v.oval = jsdb_alloc(sizeof(object_t),true);
	v.oval->names = vec_grow(NULL, 5, sizeof(value_t));
	v.oval->values = vec_grow(NULL, 5, sizeof(value_t));
	v.oval->hashmap = jsdb_alloc(10*sizeof(uint32_t), true);
	v.oval->capacity = 10;
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

value_t *lookup(value_t obj, value_t name, bool addBit) {
	uint64_t hash = hashStr(name);
	uint32_t h, start, idx;
	value_t v;
	
retry:
	h = start = hash % obj.oval->capacity;

	while ((idx = obj.oval->hashmap[h])) {
		value_t *key = obj.oval->names + idx - 1;

		if (key->aux == name.aux)
			if (!memcmp(key->str, name.str, name.aux))
				return obj.oval->values + idx - 1;

		h = (h+1) % obj.oval->capacity;

		if (h == start)
			break;
	}

	if (!addBit) {
		if (obj.oval->proto.type == vt_object) {
			obj = obj.oval->proto;
			goto retry;
		}

		return NULL;
	}

	v.bits = vt_undef;

	incrRefCnt(v);
	incrRefCnt(name);
	vec_push(obj.oval->names, name);
	vec_push(obj.oval->values, v);

	obj.oval->hashmap[h] = vec_count(obj.oval->names);

	//  is the hash table over filled?

	if (4*vec_count(obj.oval->names) > 3*obj.oval->capacity) {
		uint32_t *hash = calloc(1, 2*obj.oval->capacity * sizeof(uint32_t));
		uint32_t capacity = 2*obj.oval->capacity, i;

		jsdb_free(obj.oval->hashmap);
		obj.oval->hashmap = jsdb_alloc(2*obj.oval->capacity * sizeof(uint32_t), true);
		// rehash current entries

		for (i=0; i< vec_count(obj.oval->names); i++) {
			h = hashStr(obj.oval->names[i]) % capacity;
			
			while (obj.oval->hashmap[h])
				h = (h+1) % capacity;

			obj.oval->hashmap[h] = i + 1;
		}

		obj.oval->capacity = capacity;
	}

	return &obj.oval->values[vec_count(obj.oval->values)-1];
}

// TODO -- remove the field from the name & value vectors

value_t *deleteField(value_t obj, value_t name) {
	uint32_t idx, start, h = hashStr(name) % obj.oval->capacity;
	start = h;

	if (obj.type != vt_object)
	   return NULL;

	do {
		if ((idx = obj.oval->hashmap[h])) {
			value_t *key = obj.oval->names + idx - 1;

			if (key->aux == name.aux)
				if (!memcmp(key->str, name.str, name.aux))
					return obj.oval->values + idx - 1;
		}

		h = (h+1) % obj.oval->capacity;
	} while (h != start);

	// not there
	return NULL;
}

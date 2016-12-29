#include "js.h"

uint32_t calcSize (value_t doc);

uint32_t marshal_string (uint8_t *doc, uint32_t offset, value_t *where, value_t name) {
	string_t *str = (string_t *)(doc + offset);
	string_t *namestr = js_addr(name);

	str->len = namestr->len;

	where->type = name.type;
	where->offset = offset;
	where->marshaled = 1;

	memcpy(str->val + offset, namestr->val, namestr->len);
	return namestr->len + sizeof(string_t) + 1;
}

//  marshal a document object into the given storage

void marshal_doc(value_t document, uint8_t *doc, uint32_t offset, uint32_t docSize) {
	value_t obj[1024], *val = NULL, *loc;
	array_t *array[1024];
	object_t *docs[1024];
	int idx[1024];
	int depth;
	
	obj[0] = document;
	idx[0] = 0;

	depth = 1;
	idx[depth] = 0;

	while (depth > 0) {
		if (offset > docSize) {
			fprintf(stderr, "marshal_doc overflow\n");
			exit(1);
		}

		// find next value in parent object or array

		if (obj[depth - 1].type == vt_array) {
			array_t *aval = js_addr(obj[depth - 1]);
	  		value_t *values = obj[depth - 1].marshaled ? aval->valueArray : aval->valuePtr;
	  		uint32_t max = obj[depth - 1].marshaled ? aval->cnt : vec_cnt(aval->valuePtr);

			//  prepare to store array_t item itself

			if (!idx[depth]) {
				if (val) {
					val->bits = vt_array;
					val->offset = offset;
					val->marshaled = 1;
				}

				offset += sizeof(array_t) + sizeof(value_t) * max;
			}

			// advance to next array index,
			//	or pop array

			if (idx[depth] < max) {
				val = &array[depth]->valueArray[idx[depth]];
				obj[depth] = values[idx[depth]++];
			} else {
				depth -= 1;
				continue;
			}
		} else if (obj[depth - 1].type == vt_object) {
			object_t *oval = js_addr(obj[depth - 1]);
			pair_t *pairs = oval->marshaled ? oval->pairArray : oval->pairsPtr;
			uint32_t cnt = oval->marshaled ? oval->cnt : vec_cnt(pairs);
			string_t *namestr;
			uint64_t hash;
			value_t name;

			//  store next object_t item

			if (!idx[depth]) {
				int entSize = sizeof(uint32_t);

				docs[depth] = (object_t *)(doc + offset);
				docs[depth]->cap = cnt * 3 / 2;
				docs[depth]->cnt = cnt;

				if (val) {
					val->bits = vt_object;
					val->offset = offset;
					val->marshaled = 1;
				}

				//  marshal the hash table

				if (cnt < 256)
					entSize = sizeof(uint8_t);
				else if (cnt < 65536)
					entSize = sizeof(uint16_t);

				offset += sizeof(object_t) + sizeof(pair_t) * cnt + entSize * cnt * 3 / 2;
			}

			// advance to next object item
			//	or pop object

			if (idx[depth] < cnt) {
				void *hashTbl = docs[depth]->pairArray + cnt;

				name = pairs[idx[depth]].name;
				namestr = js_addr(name);
				hash = hashStr(namestr->val, namestr->len) % cnt;

	  			while (hashEntry(hashTbl, cnt, hash))
					hash = (hash+1) % cnt;

				hashStore(hashTbl, cnt, hash, idx[depth] + 1);

				val = &docs[depth]->pairArray[idx[depth]].value;
				loc = &docs[depth]->pairArray[idx[depth]].name;
				obj[depth] = pairs[idx[depth]++].value;

			} else {
				depth -= 1;
				continue;
			}

			//  marshal the name string

			offset += marshal_string (doc, offset, loc, name);
		} else {
			depth -= 1;
			continue;
		}

		switch (obj[depth].type) {
		case vt_objId:
		case vt_string: {
			offset += marshal_string(doc, offset, val, obj[depth]);
			break;
		}
		case vt_dbl:
		case vt_int: {
			*val = obj[depth];
			break;
		}
		case vt_array:
		case vt_object: {
			idx[++depth] = 0;
			break;
		}
		default:;
		}
	}
}

uint32_t calcSize (value_t doc) {
	uint32_t doclen[1024];
	value_t obj[1024];
	int idx[1024];
	int depth;
	
	doclen[0] = 0;
	obj[0] = doc;
	idx[0] = 0;
	depth = 1;

	doclen[depth] = 0;
	idx[depth] = 0;

	while (depth > 0) {

		// find next value in parent object or array

		if (obj[depth - 1].type == vt_array) {
			array_t *aval = js_addr(obj[depth - 1]);
	  		value_t *values = obj[depth - 1].marshaled ? aval->valueArray : aval->valuePtr;
	  		uint32_t max = obj[depth - 1].marshaled ? aval->cnt : vec_cnt(aval->valuePtr);

			if (!idx[depth])
				doclen[depth] = sizeof(array_t) + max * sizeof(value_t);

			if (idx[depth] < max) {
				obj[depth] = values[idx[depth]++];
			} else {
				doclen[depth-1] += doclen[depth];
				depth -= 1;
				continue;
			}
		} else if (obj[depth - 1].type == vt_object) {
			object_t *oval = js_addr(obj[depth - 1]);
			pair_t *pairs = oval->marshaled ? oval->pairArray : oval->pairsPtr;
			uint32_t max = oval->marshaled ? oval->cap : vec_max(pairs);
			int entSize = sizeof(uint32_t);

			if (max < 256)
				entSize = sizeof(uint8_t);
			else if (max < 65536)
				entSize = sizeof(uint16_t);

			if (!idx[depth])
				doclen[depth] = sizeof(object_t) + max * entSize + max * sizeof(pair_t);

			if (idx[depth] < max) {
				string_t *str = js_addr(pairs[idx[depth]].name);
				doclen[depth] += str->len + sizeof(string_t) + 1;
				obj[depth] = pairs[idx[depth]++].value;
			} else {
				doclen[depth-1] += doclen[depth];
				depth -= 1;
				continue;
			}
		} else {
			depth -= 1;
			continue;
		}

		switch (obj[depth].type) {
		case vt_objId: {
			doclen[depth] += 12;
			break;
		}
		case vt_string: {
			string_t *str = js_addr(obj[depth]);
			doclen[depth] += str->len + sizeof(string_t) + 1;
			break;
		}
		case vt_dbl:
		case vt_int: {
			break;
		}
		case vt_array:
		case vt_object: {
			doclen[++depth] = 0;
			idx[depth] = 0;
			break;
		}
		default:
			break;
		}
	}
	return doclen[0];
}


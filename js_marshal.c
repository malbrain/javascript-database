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
	  		uint32_t cnt = obj[depth - 1].marshaled ? aval->cnt : vec_cnt(aval->valuePtr);

			//  prepare to store array_t item itself

			if (!idx[depth]) {
				if (val) {
					val->bits = vt_array;
					val->offset = offset;
					val->marshaled = 1;
				}

				offset += sizeof(array_t) + sizeof(value_t) * cnt;
			}

			// advance to next array index,
			//	or pop array

			if (idx[depth] < cnt) {
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
			uint32_t hashEnt, hashMod = 3 * cnt / 2;
			string_t *namestr;
			uint64_t hash;
			value_t name;

			if (cnt < 255)
				hashEnt = sizeof(uint8_t);
			else if (cnt < 65535)
				hashEnt = sizeof(uint16_t);
			else
				hashEnt = sizeof(uint32_t);

			//  store object_t structure

			if (!idx[depth]) {
				docs[depth] = (object_t *)(doc + offset);
				docs[depth]->cap = cnt;
				docs[depth]->cnt = cnt;

				if (val) {
					val->bits = vt_object;
					val->offset = offset;
					val->marshaled = 1;
				}

				offset += sizeof(object_t) + cnt * sizeof(pair_t) + hashMod * hashEnt;
			}

			// advance to next object item
			//	or pop object

			if (idx[depth] < cnt) {
				void *hashTbl = docs[depth]->pairArray + cnt;

				name = pairs[idx[depth]].name;
				namestr = js_addr(name);
				hash = hashStr(namestr->val, namestr->len) % hashMod;

	  			while (hashEntry(hashTbl, hashEnt, hash))
				  if (++hash == hashMod)
					hash = 0;

				hashStore(hashTbl, hashEnt, hash, idx[depth] + 1);

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

//	calculate marshaled size

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
	  		uint32_t cnt = obj[depth - 1].marshaled ? aval->cnt : vec_cnt(aval->valuePtr);

			// structure size

			if (!idx[depth])
				doclen[depth] = sizeof(array_t) + cnt * sizeof(value_t);

			// next element size

			if (idx[depth] < cnt) {
				obj[depth] = values[idx[depth]++];
			} else {
				doclen[depth-1] += doclen[depth];
				depth -= 1;
				continue;
			}
		} else if (obj[depth - 1].type == vt_object) {
			object_t *oval = js_addr(obj[depth - 1]);
			pair_t *pairs = oval->marshaled ? oval->pairArray : oval->pairsPtr;
			uint32_t cnt = oval->marshaled ? oval->cnt : vec_cnt(pairs);
			uint32_t hashEnt, hashMod = 3 * cnt / 2;

			if (cnt < 255)
				hashEnt = sizeof(uint8_t);
			else if (cnt < 65535)
				hashEnt = sizeof(uint16_t);
			else
				hashEnt = sizeof(uint32_t);

			//  structure size

			if (!idx[depth])
				doclen[depth] = sizeof(object_t) + cnt * sizeof(pair_t) + hashMod * hashEnt;

			//  add next pair

			if (idx[depth] < cnt) {
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


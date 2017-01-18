#include "js.h"

uint32_t marshalString (uint8_t *doc, uint32_t offset, DbAddr addr, value_t *where, value_t name) {
	string_t *str = (string_t *)(doc + offset);
	string_t *namestr = js_addr(name);

	str->len = namestr->len;

	where->type = name.type;
	where->arenaAddr = addr.bits;
	where->offset = offset;
	where->marshaled = 1;

	memcpy(str->val, namestr->val, namestr->len);
	return namestr->len + sizeof(string_t) + 1;
}

//  marshal a document into the given storage

void marshalDoc(value_t document, uint8_t *doc, uint32_t base, DbAddr addr, uint32_t docSize) {
	value_t obj[1024], *val = (value_t *)(doc + base), *loc;
	uint32_t offset = base;
	void *item[1024];
	int idx[1024];
	int depth;
	
	offset += sizeof(value_t);
	obj[0] = document;
	idx[0] = 0;

	depth = 1;
	idx[depth] = 0;

	while (depth > 0) {
		if (offset - base > docSize) {
			fprintf(stderr, "marshal_doc overflow\n");
			exit(1);
		}

		// find next value in parent object or array

		if (obj[depth - 1].type == vt_array) {
			array_t *aval = js_addr(obj[depth - 1]);
	  		value_t *values = obj[depth - 1].marshaled ? aval->valueArray : aval->valuePtr;
	  		uint32_t cnt = obj[depth - 1].marshaled ? aval->cnt : vec_cnt(aval->valuePtr);
			array_t *array;

			//  prepare to store array_t item itself

			if (!idx[depth]) {
				item[depth] = doc + offset;
				val->bits = vt_array;
				val->marshaled = 1;
				val->offset = offset;
				val->arenaAddr = addr.bits;
				offset += sizeof(array_t) + sizeof(value_t) * cnt;
			}

			array = item[depth];
			array->marshaled = 1;

			// advance to next array index,
			//	or pop array

			if (idx[depth] < cnt) {
				val = &array->valueArray[idx[depth]];
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
				object_t *object = item[depth] = doc + offset;
				object->marshaled = 1;
				object->cap = cnt;
				object->cnt = cnt;

				val->bits = vt_object;
				val->arenaAddr = addr.bits;
				val->offset = offset;
				val->marshaled = 1;

				offset += sizeof(object_t) + cnt * sizeof(pair_t) + hashMod * hashEnt;
			}

			// advance to next object item
			//	or pop object

			if (idx[depth] < cnt) {
				object_t *object = item[depth];
				void *hashTbl = object->pairArray + cnt;

				name = pairs[idx[depth]].name;
				namestr = js_addr(name);
				hash = hashStr(namestr->val, namestr->len) % hashMod;

	  			while (hashEntry(hashTbl, hashEnt, hash))
				  if (++hash == hashMod)
					hash = 0;

				hashStore(hashTbl, hashEnt, hash, idx[depth] + 1);

				val = &object->pairArray[idx[depth]].value;
				loc = &object->pairArray[idx[depth]].name;
				obj[depth] = pairs[idx[depth]++].value;
			} else {
				depth -= 1;
				continue;
			}

			//  marshal the name string

			offset += marshalString (doc, offset, addr, loc, name);
		} else
		  depth -= 1;

		switch (obj[depth].type) {
		case vt_md5:
		case vt_uuid:
		case vt_objId:
		case vt_string: {	// string types
			offset += marshalString(doc, offset, addr, val, obj[depth]);
			break;
		}
		case vt_store:
		case vt_index:
		case vt_cursor:
		case vt_iter:
		case vt_txn:
		case vt_txnId:
		case vt_docId:
		case vt_bool:
		case vt_date:
		case vt_dbl:
		case vt_int:		// immediate types
		default:	{
			*val = obj[depth];
			break;
		}
		case vt_array:
		case vt_object:
		case vt_document: {
			idx[++depth] = 0;
			break;
		}
		}
	}
}

//	calculate marshaled size

uint32_t calcSize (value_t doc) {
	value_t obj[1024];
	uint32_t docLen;
	int idx[1024];
	int depth;
	
	docLen = sizeof(value_t);
	obj[0] = doc;
	idx[0] = 0;

	depth = 1;
	idx[depth] = 0;

	while (depth > 0) {

		// find next value in parent object or array

		if (obj[depth - 1].type == vt_array) {
			array_t *aval = js_addr(obj[depth - 1]);
	  		value_t *values = obj[depth - 1].marshaled ? aval->valueArray : aval->valuePtr;
	  		uint32_t cnt = obj[depth - 1].marshaled ? aval->cnt : vec_cnt(aval->valuePtr);

			// structure size

			if (!idx[depth])
				docLen += sizeof(array_t) + cnt * sizeof(value_t);

			// next element size

			if (idx[depth] < cnt) {
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

			if (cnt < 255)
				hashEnt = sizeof(uint8_t);
			else if (cnt < 65535)
				hashEnt = sizeof(uint16_t);
			else
				hashEnt = sizeof(uint32_t);

			//  structure size

			if (!idx[depth])
				docLen += sizeof(object_t) + cnt * sizeof(pair_t) + hashMod * hashEnt;

			//  add next pair

			if (idx[depth] < cnt) {
				string_t *str = js_addr(pairs[idx[depth]].name);
				docLen += str->len + sizeof(string_t) + 1;
				obj[depth] = pairs[idx[depth]++].value;
			} else {
				depth -= 1;
				continue;
			}
		} else
			depth -= 1;

		switch (obj[depth].type) {
		case vt_md5:
		case vt_uuid:
		case vt_objId:
		case vt_string: {		// string types
			string_t *str = js_addr(obj[depth]);
			docLen += str->len + sizeof(string_t) + 1;
			break;
		}
		case vt_array:
		case vt_object:
		case vt_document: {
			idx[++depth] = 0;
			break;
		}
		case vt_store:
		case vt_index:
		case vt_cursor:
		case vt_iter:
		case vt_txn:
		case vt_txnId:
		case vt_docId:
		case vt_bool:
		case vt_date:
		case vt_dbl:
		case vt_int:		// immediate values 
		default:
			break;
		}
	}
	return docLen;
}


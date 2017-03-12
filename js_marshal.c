#include "js.h"
#include "js_db.h"

uint32_t marshalString (uint8_t *doc, uint32_t offset, DbAddr addr, value_t *where, value_t name) {
	string_t *str = (string_t *)(doc + offset);
	string_t *namestr = js_addr(name);

	str->len = namestr->len;

	where->addrBits = addr.bits;
	where->type = name.type;
	where->offset = offset;
	where->marshaled = 1;

	memcpy(str->val, namestr->val, namestr->len);
	return namestr->len + sizeof(string_t) + 1;
}

//  marshal a document into the given document storage

void marshalDoc(value_t doc, uint8_t *rec, uint32_t base, DbAddr addr, uint32_t docSize, value_t *val, bool fullClone) {
	value_t obj[1024], *loc, *top;
	uint32_t offset = base;
	void *item[1024];
	int idx[1024];
	int depth;
	bool go;
	
	obj[0] = doc;
	idx[0] = 0;

	depth = 1;
	idx[depth] = 0;

	if (!doc.marshaled || fullClone)
	  while (depth > 0) {
		value_t *top = &obj[depth - 1];

		if (offset - base > docSize) {
			fprintf(stderr, "marshal_doc overflow\n");
			exit(1);
		}

		// find next value in parent object or array

		if (top->type == vt_array)
		  if (top->marshaled) {
			dbarray_t *dbaval = js_addr(*top);
	  		value_t *values = dbaval->valueArray;
	  		uint32_t cnt = dbaval->cnt;

			//  prepare to store array_t item itself

			if (!idx[depth]) {
				item[depth] = rec + offset;
				val->bits = vt_array;
				val->marshaled = 1;
				val->offset = offset;
				val->addrBits = addr.bits;
				offset += sizeof(dbarray_t) + sizeof(value_t) * cnt;
			}

			// advance to next array index,
			//	or pop array

			if (idx[depth] < cnt) {
				dbarray_t *array = item[depth];
				val = &array->valueArray[idx[depth]];
				obj[depth] = values[idx[depth]++];
			} else {
				depth -= 1;
				continue;
			}
		  } else {
	  		value_t *values = top->aval->valuePtr;
	  		uint32_t cnt = vec_cnt(values);

			//  prepare to store array_t item itself

			if (!idx[depth]) {
				item[depth] = rec + offset;
				val->bits = vt_array;
				val->marshaled = 1;
				val->offset = offset;
				val->addrBits = addr.bits;
				offset += sizeof(dbarray_t) + sizeof(value_t) * cnt;
			}

			// advance to next array index,
			//	or pop array

			if (idx[depth] < cnt) {
				dbarray_t *array = item[depth];
				val = &array->valueArray[idx[depth]];
				obj[depth] = values[idx[depth]++];
			} else {
				depth -= 1;
				continue;
			}
		  }
		else if (top->type == vt_object)
		  if (top->marshaled) {
			dbobject_t *dboval = js_addr(*top);
			pair_t *pairs = dboval->pairs;
			uint32_t cnt = dboval->cnt;
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

			//  store marshaled_t structure

			if (!idx[depth]) {
				item[depth] = rec + offset;

				val->bits = vt_object;
				val->addrBits = addr.bits;
				val->offset = offset;
				val->marshaled = 1;

				offset += sizeof(dbobject_t) + cnt * sizeof(pair_t) + hashMod * hashEnt;
			}

			// advance to next object item
			//	or pop object

			if (idx[depth] < cnt) {
				dbobject_t *object = item[depth];
				void *hashTbl = object->pairs + cnt;

				object->cnt = cnt;

				name = pairs[idx[depth]].name;
				namestr = js_addr(name);

				hash = hashStr(namestr->val, namestr->len) % hashMod;

	  			while (hashEntry(hashTbl, hashEnt, hash))
				  if (++hash == hashMod)
					hash = 0;

				hashStore(hashTbl, hashEnt, hash, idx[depth] + 1);

				val = &object->pairs[idx[depth]].value;
				object->pairs[idx[depth]].name = name;
				obj[depth] = pairs[idx[depth]++].value;
			} else {
				depth -= 1;
				continue;
			}
		  } else {
			pair_t *pairs = top->oval->pairsPtr;
			uint32_t cnt = vec_cnt(pairs);
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

			//  store marshaled_t structure

			if (!idx[depth]) {
				item[depth] = rec + offset;

				val->bits = vt_object;
				val->addrBits = addr.bits;
				val->offset = offset;
				val->marshaled = 1;

				offset += sizeof(dbobject_t) + cnt * sizeof(pair_t) + hashMod * hashEnt;
			}

			// advance to next object item
			//	or pop object

			if (idx[depth] < cnt) {
				dbobject_t *object = item[depth];
				void *hashTbl = object->pairs + cnt;
				object->cnt = cnt;

				name = pairs[idx[depth]].name;
				namestr = js_addr(name);

				hash = hashStr(namestr->val, namestr->len) % hashMod;

	  			while (hashEntry(hashTbl, hashEnt, hash))
				  if (++hash == hashMod)
					hash = 0;

				hashStore(hashTbl, hashEnt, hash, idx[depth] + 1);

				val = &object->pairs[idx[depth]].value;
				loc = &object->pairs[idx[depth]].name;
				obj[depth] = pairs[idx[depth]++].value;

				//  marshal the name string

				if (!name.marshaled || fullClone)
					offset += marshalString (rec, offset, addr, loc, name);
				else
					*loc = name;
			} else {
				depth -= 1;
				continue;
			}
		  }
		else
		  depth -= 1;

		do switch ((go = false, obj[depth].type)) {
		  case vt_md5:
		  case vt_uuid:
		  case vt_string: {	// string types
			if (obj[depth].marshaled && !fullClone)
				*val = obj[depth];
			else
				offset += marshalString(rec, offset, addr, val, obj[depth]);

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
		  default:
			*val = obj[depth];
			break;

		  case vt_document:
			obj[depth] = convDocument(obj[depth], false);
			go = true;
			continue;

		  case vt_array:
		  case vt_object:
			if (obj[depth].marshaled && !fullClone)
				*val = obj[depth];
			else
				idx[++depth] = 0;
			break;
		  }
		while (go);
	}
}

//	calculate marshaled size

uint32_t calcSize (value_t doc, bool fullClone) {
	value_t obj[1024], *top;
	uint32_t docSize = 0;
	int idx[1024];
	int depth;
	bool go;
	
	obj[0] = doc;
	idx[0] = 0;

	depth = 1;
	idx[depth] = 0;

	if (!doc.marshaled || fullClone)
	  while (depth > 0) {
		value_t *top = &obj[depth - 1];

		// find next value in parent object or array

		if (top->type == vt_array)
		  if (top->marshaled) {
			dbarray_t *dbaval = js_addr(*top);
	  		value_t *values = dbaval->valueArray;
	  		uint32_t cnt = dbaval->cnt;

			// structure size

			if (!idx[depth])
				docSize += sizeof(dbarray_t) + cnt * sizeof(value_t);

			// next element size

			if (idx[depth] < cnt) {
				obj[depth] = values[idx[depth]++];
			} else {
				depth -= 1;
				continue;
			}
		  } else {
	  		value_t *values = top->aval->valuePtr;
	  		uint32_t cnt = vec_cnt(values);

			// structure size

			if (!idx[depth])
				docSize += sizeof(dbarray_t) + cnt * sizeof(value_t);

			// next element size

			if (idx[depth] < cnt) {
				obj[depth] = values[idx[depth]++];
			} else {
				depth -= 1;
				continue;
			}
		} else if (top->type == vt_object)
		  if (top->marshaled) {
			dbobject_t *dboval = js_addr(*top);
			pair_t *pairs = dboval->pairs;
			uint32_t cnt = dboval->cnt;
			uint32_t hashEnt, hashMod = 3 * cnt / 2;

			if (cnt < 255)
				hashEnt = sizeof(uint8_t);
			else if (cnt < 65535)
				hashEnt = sizeof(uint16_t);
			else
				hashEnt = sizeof(uint32_t);

			//  structure size

			if (!idx[depth])
				docSize += sizeof(dbobject_t) + cnt * sizeof(pair_t) + hashMod * hashEnt;

			//  add next pair

			if (idx[depth] < cnt) {
				pair_t *pair = &pairs[idx[depth]++];

				if (fullClone) {
					string_t *str = js_addr(pair->name);
					docSize += str->len + sizeof(string_t) + 1;
				}

				obj[depth] = pair->value;
			} else {
				depth -= 1;
				continue;
			}
		  } else {
			pair_t *pairs = top->oval->pairsPtr;
			uint32_t cnt = vec_cnt(pairs);
			uint32_t hashEnt, hashMod = 3 * cnt / 2;

			if (cnt < 255)
				hashEnt = sizeof(uint8_t);
			else if (cnt < 65535)
				hashEnt = sizeof(uint16_t);
			else
				hashEnt = sizeof(uint32_t);

			//  structure size

			if (!idx[depth])
				docSize += sizeof(dbobject_t) + cnt * sizeof(pair_t) + hashMod * hashEnt;

			//  add next pair

			if (idx[depth] < cnt) {
				pair_t *pair = &pairs[idx[depth]++];

				if (!pair->name.marshaled || fullClone) {
					string_t *str = js_addr(pair->name);
					docSize += str->len + sizeof(string_t) + 1;
				}

				obj[depth] = pair->value;
			} else {
				depth -= 1;
				continue;
			}
		  }
		else
		  depth -= 1;

		do switch ((go = false, obj[depth].type)) {
		  case vt_md5:
		  case vt_uuid:
		  case vt_string: {		// string types
			if (!obj[depth].marshaled || fullClone) {
				string_t *str = js_addr(obj[depth]);
				docSize += str->len + sizeof(string_t) + 1;
			}

			break;
		  }
		  case vt_document:
			obj[depth] = convDocument(obj[depth], false);
			go = true;
			continue;

		  case vt_array:
		  case vt_object:
			if (!obj[depth].marshaled || fullClone)
				idx[++depth] = 0;
			break;

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
	  while (go);
	}
	return docSize;
}

#include "jsdb.h"
#include "jsdb_db.h"

static bool debug = false;

void *lockHandle(value_t val);
void unlockHandle(value_t val);

Status storeVal(DbMap *map, DbAddr docAddr, DocId *docId, DocId txnId);
uint32_t calcSize (value_t doc);

uint32_t marshal_string (uint8_t *doc, uint32_t offset, value_t *where, value_t name) {
	where->type = name.type;
	where->offset = offset;
	where->aux = name.aux;
	where->rebaseptr = 1;

	memcpy(doc + offset, name.str, name.aux);
	return name.aux;
}

//   marshal the document into the DocumentStore

uint64_t marshal_doc(DbMap *map, value_t document) {
	uint32_t docSize = calcSize(document);
	value_t obj[1024], *val = NULL, *loc;
	uint32_t set = getSet(map);
	docarray_t *array[1024];
	document_t *docs[1024];
	uint32_t offset = 0;
	DbAddr docAddr;
	int idx[1024];
	int depth;
	uint8_t *doc;
	
	if ( !(doc = allocateDoc(map, docSize, &docAddr, set))) {
		fprintf(stderr, "Error: marsal_doc out of memory");
		exit(1);
	}

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
			int max = vec_count(obj[depth - 1].aval->values);

			//  store array_t item

			if (!idx[depth]) {
				array[depth] = (docarray_t *)(doc + offset);
				array[depth]->base = offset;
				array[depth]->count = max;

				if (val) {
					val->bits = vt_docarray;
					val->offset = offset;
					val->rebaseptr = 1;
				}

				offset += sizeof(docarray_t) + sizeof(value_t) * max;
			}

			if (idx[depth] < max) {
				val = &array[depth]->values[idx[depth]];
				obj[depth] = (obj[depth - 1].aval->values)[idx[depth]++];
			} else {
				depth -= 1;
				continue;
			}
		} else if (obj[depth - 1].type == vt_object) {
			struct Object *scan = obj[depth - 1].oval;
			int max = vec_count(scan->pairs);
			value_t name;

			//  store document_t item

			if (!idx[depth]) {
				int entSize = sizeof(uint32_t);

				docs[depth] = (document_t *)(doc + offset);
				docs[depth]->capacity = scan->capacity;
				docs[depth]->base = offset;
				docs[depth]->count = max;

				if (val) {
					val->bits = vt_document;
					val->offset = offset;
					val->rebaseptr = 1;
				}

				if (scan->capacity + 1 < 256)
					entSize = sizeof(uint8_t);
				else if (scan->capacity + 1 < 65536)
					entSize = sizeof(uint16_t);

				memcpy ((void *)&docs[depth]->pairs[max], scan->hashTbl, entSize * scan->capacity);
				offset += sizeof(document_t) + sizeof(pair_t) * max + entSize * scan->capacity;
			}

			if (idx[depth] < max) {
				name = scan->pairs[idx[depth]].name;
				val = &docs[depth]->pairs[idx[depth]].value;
				loc = &docs[depth]->pairs[idx[depth]].name;
				obj[depth] = scan->pairs[idx[depth]++].value;
			} else {
				depth -= 1;
				continue;
			}
			offset += marshal_string (doc, offset, loc, name);
		} else if (obj[depth - 1].type == vt_document) {
			document_t *scan = obj[depth - 1].document;
			uint32_t max = scan->count;
			value_t name;

			//  store document_t item

			if (!idx[depth]) {
				int entSize = sizeof(uint32_t);

				docs[depth] = (document_t *)(doc + offset);
				docs[depth]->capacity = scan->capacity;
				docs[depth]->base = offset;
				docs[depth]->count = max;

				if (val) {
					val->bits = vt_document;
					val->offset = offset;
					val->rebaseptr = 1;
				}

				if (scan->capacity + 1 < 256)
					entSize = sizeof(uint8_t);
				else if (scan->capacity + 1 < 65536)
					entSize = sizeof(uint16_t);

				offset += sizeof(document_t) + sizeof(pair_t) * max + entSize * scan->capacity;
			}

			if (idx[depth] < max) {
				name = scan->pairs[idx[depth]].name;
				val = &docs[depth]->pairs[idx[depth]].value;
				loc = &docs[depth]->pairs[idx[depth]].name;
				obj[depth] = scan->pairs[idx[depth]].value;

				if (obj[depth].rebaseptr)
					obj[depth].rebase = obj[depth - 1].rebase - scan->base + obj[depth].offset;

			} else {
				depth -= 1;
				continue;
			}

			offset += marshal_string (doc, offset, &docs[depth]->pairs[idx[depth]].name, name);
		} else if (obj[depth - 1].type == vt_docarray) {
			docarray_t *scan = obj[depth - 1].docarray;
			uint32_t max = scan->count;

			//  store docarray item

			if (!idx[depth]) {
				array[depth] = (docarray_t *)(doc + offset);
				array[depth]->base = offset;
				array[depth]->count = max;

				if (val) {
					val->bits = vt_docarray;
					val->offset = offset;
					val->rebaseptr = 1;
				}

				offset += sizeof(docarray_t) + sizeof(value_t) * max;
			}

			if (idx[depth] < max) {
				val = &array[depth]->values[idx[depth]];
				obj[depth] = scan->values[idx[depth]++];

				if (obj[depth].rebaseptr)
					obj[depth].rebase = obj[depth - 1].rebase - scan->base + obj[depth].offset;
			} else {
				depth -= 1;
				continue;
			}
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
		case vt_document:
		case vt_docarray:
		case vt_array:
		case vt_object: {
			idx[++depth] = 0;
			break;
		}
		default:;
		}
	}

	return docAddr.bits;
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
			int max = vec_count(obj[depth - 1].aval->values);
			if (!idx[depth])
				doclen[depth] = sizeof(docarray_t) + max * sizeof(value_t);

			if (idx[depth] < max) {
				obj[depth] = obj[depth - 1].aval->values[idx[depth]++];
			} else {
				doclen[depth-1] += doclen[depth];
				depth -= 1;
				continue;
			}
		} else if (obj[depth - 1].type == vt_object) {
			struct Object *scan = obj[depth - 1].oval;
			int max = vec_count(scan->pairs);
			int entSize = sizeof(uint32_t);

			if (scan->capacity + 1 < 256)
				entSize = sizeof(uint8_t);
			else if (scan->capacity + 1 < 65536)
				entSize = sizeof(uint16_t);

			if (!idx[depth])
				doclen[depth] = sizeof(document_t) + scan->capacity * entSize + max * sizeof(pair_t);

			if (idx[depth] < max) {
				doclen[depth] += scan->pairs[idx[depth]].name.aux;
				obj[depth] = scan->pairs[idx[depth]++].value;
			} else {
				doclen[depth-1] += doclen[depth];
				depth -= 1;
				continue;
			}
		} else if (obj[depth - 1].type == vt_docarray) {
			int max = obj[depth - 1].docarray->count;
			if (!idx[depth])
				doclen[depth] = sizeof(docarray_t) + max * sizeof(value_t);

			if (idx[depth] < max) {
				obj[depth] = obj[depth - 1].docarray->values[idx[depth]++];

				if (obj[depth].rebaseptr)
					obj[depth].rebase = obj[depth - 1].rebase - obj[depth - 1].docarray->base + obj[depth].offset;
			} else {
				doclen[depth-1] += doclen[depth];
				depth -= 1;
				continue;
			}
		} else if (obj[depth - 1].type == vt_document) {
			struct Document *scan = obj[depth - 1].document;
			int entSize = sizeof(uint32_t);
			int max = scan->count;

			if (scan->capacity + 1 < 256)
				entSize = sizeof(uint8_t);
			else if (scan->capacity + 1 < 65536)
				entSize = sizeof(uint16_t);

			if (!idx[depth])
				doclen[depth] = sizeof(document_t) + scan->capacity * entSize + 2 * max * sizeof(value_t);

			if (idx[depth] < max) {
				doclen[depth] += scan->pairs[idx[depth]].name.aux;
				obj[depth] = scan->pairs[idx[depth]++].value;

				if (obj[depth].rebaseptr)
					obj[depth].rebase = obj[depth - 1].rebase - obj[depth - 1].document->base + obj[depth].offset;

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
			doclen[depth] += obj[depth].aux;
			break;
		}
		case vt_dbl:
		case vt_int: {
			break;
		}
		case vt_document:
		case vt_docarray:
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

//  insertDocs (docStore, docArray, &docIdArray, &docCount, dbtxn)

value_t jsdb_insertDocs(uint32_t args, environment_t *env) {
	value_t v, slot, slot2, docs, docStore, dbtxn;
	DbAddr docAddr;
	value_t array;
	int i, count;
	DocId docId;
	value_t s;
	void *obj;

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : InsertDocs\n");

	docStore = eval_arg (&args, env);

	if (vt_handle != docStore.type || Hndl_docStore != docStore.subType) {
		fprintf(stderr, "Error: createIndex => expecting docStore:handle => %s\n", strtype(docStore.type));
		return s.status = ERROR_script_internal, s;
	}

	//  insert an array of documents

	array = eval_arg(&args, env);

	if (vt_array != array.type && vt_object != array.type && vt_document != array.type) {
		fprintf(stderr, "Error: insertDocs => expecting docs:Array => %s\n", strtype(array.type));
		return s.status = ERROR_script_internal, s;
	}

	if (array.type == vt_array)
		count = vec_count(array.aval->values);
	else
		count = 1;

	//  return an array of DocId

	slot = eval_arg(&args, env);

	if (vt_lval != slot.type) {
		fprintf(stderr, "Error: insertDocs => expecting DocId:Symbol => %s\n", strtype(slot.type));
		return s.status = ERROR_script_internal, s;
	}

	//  return the size of the array

	slot2 = eval_arg(&args, env);

	if (vt_lval != slot2.type) {
		fprintf(stderr, "Error: insertDocs => expecting Count:Symbol => %s\n", strtype(slot2.type));
		return s.status = ERROR_script_internal, s;
	}

	//  insert txn

	dbtxn = eval_arg(&args, env);

	if (vt_docId != dbtxn.type) {
		fprintf(stderr, "Error: insertDocs => expecting Txn:docId => %s\n", strtype(dbtxn.type));
		return s.status = ERROR_script_internal, s;
	}

	if ((obj = lockHandle(docStore)))
		docs = newArray(array_value);
	else
		return s.status = ERROR_handleclosed, s;

	//  insert the documents

	for( i = 0; i < count; i++ ) {
	  value_t nxtdoc;

	  if (array.type == vt_array)
		  nxtdoc = array.aval->values[i];
	  else
		  nxtdoc = array;

	  // marshall the document

	  docAddr.bits = marshal_doc (obj, nxtdoc);

	  // add the document and index keys to the documentStore

	  s.status = storeVal(obj, docAddr, &docId, dbtxn.docId);

	  if (OK != s.status) {
		fprintf(stderr, "Error: insertDocs => %s\n", strstatus(s.status));
		return s;
	  }

	  //  add the docId to the result Array

	  v.bits = vt_docId;
	  v.docId = docId;
	  vec_push(docs.aval->values, v);
	}

	replaceValue(slot, docs);

	v.bits = vt_int;
	v.nval = count;
	replaceValue(slot2, v);
	abandonValue(array);
	return s.status = OK, s;
}


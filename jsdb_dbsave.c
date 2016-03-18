#include "jsdb.h"
#include "jsdb_db.h"
#include "jsdb_docs.h"

static bool debug = false;

uint32_t marshal_string (uint8_t *doc, uint32_t offset, value_t *where, value_t name) {
    where->type = name.type;
    where->offset = offset;
    where->aux = name.aux;
    where->rebaseptr = 1;

    memcpy(doc + offset, name.str, name.aux);
    return name.aux;
}

//   store the document into the DocumentStore

void marshal_doc(uint8_t *doc, uint32_t docSize, value_t document) {
    value_t obj[1024], *val = NULL, *loc;
    docarray_t *array[1024];
    document_t *docs[1024];
    uint32_t offset = 0;
    int idx[1024];
    int i, depth;
    
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
            int max = vec_count(obj[depth - 1].aval->array);

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
                val = &array[depth]->array[idx[depth]];
                obj[depth] = (obj[depth - 1].aval->array)[idx[depth]++];
            } else {
                depth -= 1;
                continue;
            }
        } else if (obj[depth - 1].type == vt_object) {
            struct Object *scan = obj[depth - 1].oval;
            int max = vec_count(scan->names);
            value_t name;

            //  store document_t item

            if (!idx[depth]) {
                docs[depth] = (document_t *)(doc + offset);
                docs[depth]->capacity = scan->capacity;
                docs[depth]->base = offset;
                docs[depth]->count = max;

                if (val) {
                    val->bits = vt_document;
                    val->offset = offset;
                    val->rebaseptr = 1;
                }

                memcpy ((void *)&docs[depth]->names[max * 2], scan->table, sizeof(uint32_t) * scan->capacity);
                offset += sizeof(document_t) + sizeof(value_t) * max * 2 + sizeof(uint32_t) * scan->capacity;
            }

            if (idx[depth] < max) {
                name = scan->names[idx[depth]];
                val = &docs[depth]->names[max + idx[depth]];
                loc = &docs[depth]->names[idx[depth]];
                obj[depth] = scan->values[idx[depth]++];
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
                docs[depth] = (document_t *)(doc + offset);
                docs[depth]->capacity = scan->capacity;
                docs[depth]->base = offset;
                docs[depth]->count = max;

                if (val) {
                    val->bits = vt_document;
                    val->offset = offset;
                    val->rebaseptr = 1;
                }

                offset += sizeof(document_t) + sizeof(value_t) * max * 2 + sizeof(uint32_t) * scan->capacity;
            }

            if (idx[depth] < max) {
                name = scan->names[idx[depth]];
                val = &docs[depth]->names[docs[depth]->count + idx[depth]];
                loc = &docs[depth]->names[idx[depth]];
                obj[depth] = scan->names[idx[depth] + max];

                if (obj[depth].rebaseptr)
                    obj[depth].rebase = obj[depth - 1].rebase - scan->base + obj[depth].offset;

            } else {
                depth -= 1;
                continue;
            }

            offset += marshal_string (doc, offset, &docs[depth]->names[idx[depth]], name);
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
                val = &array[depth]->array[idx[depth]];
                obj[depth] = scan->array[idx[depth]++];

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
        case vt_objid:
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
}

uint32_t calcSize (value_t doc) {
    uint32_t doclen[1024];
    value_t obj[1024];
    int idx[1024];
    int i, depth;
    
    doclen[0] = 0;
    obj[0] = doc;
    idx[0] = 0;
    depth = 1;

    doclen[depth] = 0;
    idx[depth] = 0;

    while (depth > 0) {

        // find next value in parent object or array

        if (obj[depth - 1].type == vt_array) {
            int max = vec_count(obj[depth - 1].aval->array);
            if (!idx[depth])
                doclen[depth] = sizeof(docarray_t) + max * sizeof(value_t);

            if (idx[depth] < max) {
                obj[depth] = obj[depth - 1].aval->array[idx[depth]++];
            } else {
                doclen[depth-1] += doclen[depth];
                depth -= 1;
                continue;
            }
        } else if (obj[depth - 1].type == vt_object) {
            struct Object *scan = obj[depth - 1].oval;
            int max = vec_count(scan->names);

            if (!idx[depth])
                doclen[depth] = sizeof(document_t) + scan->capacity * sizeof(uint32_t) + 2 * max * sizeof(value_t);

            if (idx[depth] < max) {
                doclen[depth] += scan->names[idx[depth]].aux;
                obj[depth] = scan->values[idx[depth]++];
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
                obj[depth] = obj[depth - 1].docarray->array[idx[depth]++];

                if (obj[depth].rebaseptr)
                    obj[depth].rebase = obj[depth - 1].rebase - obj[depth - 1].docarray->base + obj[depth].offset;
            } else {
                doclen[depth-1] += doclen[depth];
                depth -= 1;
                continue;
            }
        } else if (obj[depth - 1].type == vt_document) {
            struct Document *scan = obj[depth - 1].document;
            int max = scan->count;

            if (!idx[depth])
                doclen[depth] = sizeof(document_t) + scan->capacity * sizeof(uint32_t) + 2 * max * sizeof(value_t);

            if (idx[depth] < max) {
                doclen[depth] += scan->names[idx[depth]].aux;
                obj[depth] = scan->names[max + idx[depth]++];

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
        case vt_objid: {
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

//  insertDocs (docStore, docArray, &docIdArray, &docCount)

Status jsdb_insertDocs(uint32_t args, environment_t *env) {
    value_t a, r, v, *slot, *slot2, docs, store;
    DbAddr docAddr;
    value_t array;
	uint32_t set;
    int i, count;
    DocId docId;
    void *val;
    Status s;

    if (debug) fprintf(stderr, "funcall : InsertDocs\n");

	store = eval_arg(&args, env);

    if (vt_handle != store.type || hndl_docStore != store.aux) {
        fprintf(stderr, "Error: insertDocs => expecting recstore:Handle => %s\n", strtype(store.type));
        return ERROR_script_internal;
    }

	set = getSet(store.h);

    //  insert an array of documents

	v = eval_arg(&args, env);
    array = v;

    if (vt_array != v.type && vt_object != v.type && vt_document != v.type) {
        fprintf(stderr, "Error: insertDocs => expecting docs:Array => %s\n", strtype(v.type));
        return ERROR_script_internal;
    }

    if (array.type == vt_array)
        count = vec_count(array.aval->array);
    else
        count = 1;

    //  return an array of DocId

	v = eval_arg(&args, env);
    slot = v.ref;

    if (vt_ref != v.type) {
        fprintf(stderr, "Error: insertDocs => expecting DocId:Symbol => %s\n", strtype(v.type));
        return ERROR_script_internal;
    }

    //  return the size of the array

	v = eval_arg(&args, env);
    slot2 = v.ref;

    if (vt_ref != v.type) {
        fprintf(stderr, "Error: insertDocs => expecting Count:Symbol => %s\n", strtype(v.type));
        return ERROR_script_internal;
    }

    //  insert the documents

    docs = newArray();

    for( i = 0; i < count; i++ ) {
      value_t nxtdoc;

      if (array.type == vt_array)
          nxtdoc = array.aval->array[i];
      else
          nxtdoc = array;

      uint32_t docSize = calcSize(nxtdoc);

      if ( !(val = allocateDoc(store.h, docSize, &docAddr, set))) {
        fprintf(stderr, "Error: insertDocs => %s\n", strstatus(s));
        return s;
      }
     
      // marshall the document

      marshal_doc (val, docSize, nxtdoc);

      // add the document to the documentStore

      s = storeVal(store.h, docAddr, &docId, set);

      if (OK != s) {
        fprintf(stderr, "Error: insertDocs => %s\n", strstatus(s));
        return s;
      }

      //  add the docId to the result Array

      v.bits = vt_docId;
      v.docId = docId;
      vec_push(docs.aval->array, v);
    }

    replaceSlotValue(slot, &docs);

    v.bits = vt_int;
    v.nval = count;
    replaceSlotValue(slot2, &v);

    return OK;
}


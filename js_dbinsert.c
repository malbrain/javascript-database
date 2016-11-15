#include "js.h"
#include "database/db.h"
#include "database/db_api.h"

static bool debug = false;

extern uint32_t calcSize (value_t doc);
extern void marshal_doc(value_t document, uint8_t *doc, uint32_t docSize);

//  insertDocs (docStore, docArray, &docIdArray, &docCount, dbTxn)

value_t js_insertDocs(uint32_t args, environment_t *env) {
	value_t v, slot, slot2, docs, docStore, dbTxn;
	value_t array;
	uint32_t size;
	int i, count;
	value_t s;
	Doc *doc;

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

	dbTxn = eval_arg(&args, env);

	if (vt_txnId != dbTxn.type && vt_undef != dbTxn.type) {
		fprintf(stderr, "Error: insertDocs => expecting Txn:docId => %s\n", strtype(dbTxn.type));
		return s.status = ERROR_script_internal, s;
	}

	docs = newArray(array_value);

	//  insert the documents

	for( i = 0; i < count; i++ ) {
	  value_t nxtDoc;

	  if (array.type == vt_array)
		  nxtDoc = array.aval->values[i];
	  else
		  nxtDoc = array;

	  // marshall the document

	  size = calcSize(nxtDoc);

	  if ((s.status = (int)allocDoc((DbHandle *)docStore.handle, &doc, size)))
		return fprintf(stderr, "Error: insertDocs => %s\n", strstatus(s.status)), s;

	  marshal_doc(nxtDoc, (uint8_t*)(doc + 1), size);

	  // add the document and index keys to the documentStore

	  s.status = (int)assignDoc((DbHandle *)docStore.handle, doc, dbTxn.txnBits);

	  if (s.status) {
		fprintf(stderr, "Error: insertDocs => %s\n", strstatus(s.status));
		return s;
	  }

	  //  add the docId to the result Array

	  v.bits = vt_docId;
	  v.docBits = doc->docId.bits;
	  vec_push(docs.aval->values, v);
	}

	replaceValue(slot, docs);

	v.bits = vt_int;
	v.nval = count;
	replaceValue(slot2, v);
	abandonValue(array);
	return s.status = OK, s;
}


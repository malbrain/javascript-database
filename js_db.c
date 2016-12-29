#include "js.h"
#include "js_props.h"
#include "js_dbindex.h"
#include "database/db.h"
#include "database/db_api.h"

void marshal_doc(value_t document, uint8_t *doc, uint32_t offset, uint32_t docSize);
uint32_t calcSize (value_t doc);

void js_deleteHandle(value_t val) {
	deleteHandle((DbHandle *)&val.handle);
}

value_t fcnCollInsert(value_t *args, value_t *thisVal) {
	object_t *oval = js_addr(*thisVal);
	value_t v, s, resp = newArray(array_value);
	array_t *respval = resp.addr;
	Doc *doc;

	s.bits = vt_status;

	if (args[0].type == vt_array) {
	  array_t *aval = js_addr(args[0]);
	  value_t *values = args[0].marshaled ? aval->valueArray : aval->valuePtr;
	  uint32_t cnt = args[0].marshaled ? aval->cnt : vec_cnt(aval->valuePtr);

	  for (int idx = 0; idx < cnt; idx++) {
		int size = calcSize(values[idx]);

		if ((s.status = allocDoc((DbHandle *)oval->base->handle, &doc, 0)))
			return s;

		marshal_doc(values[idx], (uint8_t*)doc, sizeof(Doc), size);
		  
		if ((s.status = assignDoc((DbHandle *)oval->base->handle, doc, 0)))
			return s;

		v.bits = vt_docId;
		v.docBits = doc->ver->docId.bits;
		vec_push(respval->valuePtr, v);
	  }

	}

	return resp;
}

PropFcn builtinDbFcns[] = {
//	{ fcnDbOnDisk, "onDisk" },
	{ NULL, NULL}
};

PropVal builtinDbProp[] = {
//	{ propDbOnDisk, "onDisk" },
	{ NULL, NULL}
};

PropFcn builtinCollFcns[] = {
	{ fcnCollInsert, "insert" },
	{ NULL, NULL}
};

PropVal builtinCollProp[] = {
//	{ propCollOnDisk, "onDisk" },
	{ NULL, NULL}
};

PropFcn builtinIterFcns[] = {
//	{ fcnIterOnDisk, "onDisk" },
	{ NULL, NULL}
};

PropVal builtinIterProp[] = {
//	{ propIterOnDisk, "onDisk" },
	{ NULL, NULL}
};

PropVal builtinIdxFcns[] = {
//	{ fcnIdxOnDisk, "onDisk" },
	{ NULL, NULL}
};

PropVal builtinIdxProp[] = {
//	{ propIdxOnDisk, "onDisk" },
	{ NULL, NULL}
};

PropVal builtinCursorFcns[] = {
//	{ fcnIdxOnDisk, "onDisk" },
	{ NULL, NULL}
};

PropVal builtinCursorProp[] = {
//	{ propIdxOnDisk, "onDisk" },
	{ NULL, NULL}
};

PropFcn builtinTxnFcns[] = {
//	{ fcnTxnBeginTxn, "beginTxn" },
	{ NULL, NULL}
};

PropFcn builtinTxnProp[] = {
//	{ propTxnBeginTxn, "beginTxn" },
	{ NULL, NULL}
};

//	openDatabase(dbname, options)

value_t js_openDatabase(uint32_t args, environment_t *env) {
	Params params[MaxParam];
	value_t v, dbname;
	DbHandle idx[1];
	string_t *dbstr;
	value_t s;

	memset (params, 0, sizeof(params));

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : openDatabase\n");

	dbname = eval_arg (&args, env);
	dbstr = js_addr(dbname);

	if (vt_string != dbname.type) {
		fprintf(stderr, "Error: openDatabase => expecting Name:string => %s\n", strtype(dbname.type));
		return s.status = ERROR_script_internal, s;
	}

	v = eval_arg (&args, env);
	params[OnDisk].boolVal = conv2Bool(v, true).boolean;

	if ((s.status = (int)openDatabase(idx, dbstr->val, dbstr->len, params)))
		return s;

	v.bits = vt_db;
	v.subType = Hndl_database;
	*v.handle = idx->hndlBits;

	abandonValue(dbname);
	return v;
}

//  createIndex(docStore, keys, idxname, type, size, onDisk, unique, sparse, partial) 

value_t js_createIndex(uint32_t args, environment_t *env) {
	value_t v, name, docStore, keys, type;
	uint8_t *spec = NULL, *partial = NULL;
	string_t *namestr, *typestr;
	Params params[MaxParam];
	HandleType idxType;
	DbHandle idx[1];
	object_t *oval;
	uint32_t size;
	pair_t *pairs;
	int i, cnt;
	value_t s;

	memset (params, 0, sizeof(params));

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : CreateIndex\n");

	docStore = eval_arg (&args, env);

	if (vt_handle != docStore.type || Hndl_docStore != docStore.subType) {
		fprintf(stderr, "Error: createIndex => expecting docStore:handle => %s\n", strtype(docStore.type));
		return s.status = ERROR_script_internal, s;
	}

	keys = eval_arg (&args, env);
	oval = js_addr(keys);

	if (vt_object != keys.type) {
		fprintf(stderr, "Error: createIndex => expecting Object:keys => %s\n", strtype(keys.type));
		return s.status = ERROR_script_internal, s;
	}

	pairs = oval->marshaled ? oval->pairArray : oval->pairsPtr;
	cnt = oval->marshaled ? oval->cnt : vec_cnt(pairs);

	size = 0;

	for (i = 0; i < cnt; i++) {
		namestr = js_addr(pairs[i].name);
		size += namestr->len + sizeof(IndexKey);
	}

	spec = js_alloc(size, false);
	params[IdxKeySpecLen].intVal = size;
	params[IdxKeySpec].obj = spec;

	compileKeys(spec, size, oval);

	name = eval_arg (&args, env);
	namestr = js_addr(name);

	if (vt_string != name.type) {
		fprintf(stderr, "Error: createIndex => expecting Name:string => %s\n", strtype(name.type));
		return s.status = ERROR_script_internal, s;
	}

	type = eval_arg (&args, env);
	typestr = js_addr(type);

	if (vt_string != type.type) {
		fprintf(stderr, "Error: createIndex => expecting Type:string => %s\n", strtype(type.type));
		return s.status = ERROR_script_internal, s;
	}

	if (!strncmp(typestr->val, "artree", typestr->len))
		idxType = Hndl_artIndex;
	else if (!strncmp(typestr->val, "btree1", typestr->len))
		idxType = Hndl_btree1Index;
	else {
		fprintf(stderr, "Error: createIndex => expecting artree or btree1 => %.*s\n", typestr->len, typestr->val);
		return s.status = ERROR_script_internal, s;
	}

	v = eval_arg (&args, env);
	params[InitSize].intVal = conv2Int(v, true).nval;

	v = eval_arg (&args, env);
	params[OnDisk].boolVal = conv2Bool(v, true).boolean;

	v = eval_arg (&args, env);
	params[IdxKeyUnique].boolVal = conv2Bool(v, true).boolean;

	v = eval_arg (&args, env);
	params[IdxKeySparse].boolVal = conv2Bool(v, true).boolean;

	v = eval_arg (&args, env);

	if (v.type == vt_object) {
		size = calcSize(v);
		partial = js_alloc(size, false);
		params[IdxKeyPartialLen].intVal = size;
		params[IdxKeyPartial].obj = partial;

		marshal_doc(v, partial, 0, size);
		params[IdxKeyPartial].obj = partial;
	}

	abandonValue(v);

	if ((s.status = (int)createIndex(idx, (DbHandle *)docStore.handle, idxType, namestr->val, namestr->len, params)))
		return s;

	if (spec)
		js_free(spec);
	if (partial)
		js_free(partial);

	s.bits = vt_handle;
	s.subType = idxType;
	*s.handle = idx->hndlBits;

	abandonValue(type);
	abandonValue(keys);
	abandonValue(name);
	return s;
}

//  createCursor(index, txnId, start, limit)

value_t js_createCursor(uint32_t args, environment_t *env) {
	value_t start, index, limits, txn;
	Params params[MaxParam];
	DbHandle idx[1];
	ObjId txnId;
	value_t s;

	memset (params, 0, sizeof(params));

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : createCursor\n");

	index = eval_arg (&args, env);

	if (vt_handle != index.type || (Hndl_btree1Index != index.subType && Hndl_artIndex != index.subType)) {
		fprintf(stderr, "Error: createCursor => expecting index:handle => %s\n", strtype(index.type));
		return s.status = ERROR_script_internal, s;
	}

	txn = eval_arg (&args, env);

	if (txn.type == vt_txnId)
		txnId.bits = txn.txnBits;
	else
		txnId.bits = 0;

	start = eval_arg (&args, env);

	if (vt_document != start.type && vt_endlist != start.type) {
		fprintf(stderr, "Error: createCursor => expecting start:Object => %s\n", strtype(start.type));
		return s.status = ERROR_script_internal, s;
	}

	limits = eval_arg (&args, env);

	if (vt_document != limits.type && vt_endlist != limits.type) {
		fprintf(stderr, "Error: createCursor => expecting limits:Object => %s\n", strtype(limits.type));
		return s.status = ERROR_script_internal, s;
	}

	if ((s.status = (int)createCursor(idx, (DbHandle *)index.handle, txnId, params)))
		return s;

	//	TODO:  set min & max keys

	s.bits = vt_handle;
	s.subType = Hndl_cursor;
	*s.handle = idx->hndlBits;
	return s;
}

//	openDocStore(database, name, size, onDisk)

value_t js_openDocStore(uint32_t args, environment_t *env) {
	value_t v, name, docStore, database;
	Params params[MaxParam];
	string_t *namestr;
	DbHandle idx[1];
	value_t s;

	memset (params, 0, sizeof(params));
	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : CreateDocStore\n");

	database = eval_arg (&args, env);

	if (vt_handle != database.type || Hndl_database != database.subType) {
		fprintf(stderr, "Error: openDocStore => expecting Database handle => %s\n", strtype(database.type));
		return s.status = ERROR_script_internal, s;
	}

	name = eval_arg (&args, env);
	namestr = js_addr(name);

	if (vt_string != name.type) {
		fprintf(stderr, "Error: openDocStore => expecting Name => %s\n", strtype(name.type));
		return s.status = ERROR_script_internal, s;
	}

	v = eval_arg(&args, env);
	params[InitSize].intVal = conv2Int(v, true).nval;

	v = eval_arg(&args, env);
	params[OnDisk].boolVal = conv2Bool(v, true).boolean;

	if ((s.status = (int)openDocStore(idx, (DbHandle *)database.handle, namestr->val, namestr->len, params)))
		return s;

	abandonValue(name);

	docStore.bits = vt_handle;
	docStore.subType = Hndl_docStore;
	*docStore.handle = idx->hndlBits;
	return docStore;
}

//  js_createIterator(docStore, txnId)

value_t js_createIterator(uint32_t args, environment_t *env) {
	value_t iter, docStore, txnId;
	DbHandle idx[1];
	value_t s;

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : CreateIterator\n");

	docStore = eval_arg (&args, env);

	if (vt_handle != docStore.type || Hndl_docStore != docStore.subType) {
		fprintf(stderr, "Error: createIterator => expecting docStore:Handle => %s\n", strtype(docStore.type));
		return s.status = ERROR_script_internal, s;
	}

	txnId = eval_arg (&args, env);

	if (vt_txnId != txnId.type && vt_endlist != txnId.type) {
		fprintf(stderr, "Error: createIterator => expecting txnId => %s\n", strtype(txnId.type));
		return s.status = ERROR_script_internal, s;
	}

	if (txnId.type == vt_endlist)
		txnId.txnBits = 0;

	if ((s.status = (int)createIterator(idx, (DbHandle *)docStore.handle, txnId.txnBits)))
		return s;

	iter.bits = vt_handle;
	iter.subType = Hndl_iterator;
	*iter.handle = idx->hndlBits;
	return iter;
}

//	beginTxn(db)

value_t js_beginTxn(uint32_t args, environment_t *env) {
	value_t db, txnId;
	value_t s;

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : beginTxn\n");

	db = eval_arg (&args, env);

	if (vt_handle != db.type || Hndl_database != db.subType) {
		fprintf(stderr, "Error: beginTxn => expecting Db:handle => %s\n", strtype(db.type));
		return s.status = ERROR_script_internal, s;
	}

	txnId.bits = vt_txnId;

	if((txnId.txnBits = beginTxn((DbHandle *)db.handle)))
		return txnId;
	else
		s.status = ERROR_outofmemory;

	return s;
}

//	commitTxn(db, txnId)

value_t js_commitTxn(uint32_t args, environment_t *env) {
	value_t db, txnId;
	value_t s;

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : commitTxn\n");

	db = eval_arg (&args, env);

	if (vt_handle != db.type || Hndl_database != db.subType) {
		fprintf(stderr, "Error: commitTxn => expecting Db:handle => %s\n", strtype(db.type));
		return s.status = ERROR_script_internal, s;
	}

	txnId = eval_arg (&args, env);

	if (vt_txnId != txnId.type) {
		fprintf(stderr, "Error: beginTxn => expecting Db:txnId => %s\n", strtype(txnId.type));
		return s.status = ERROR_script_internal, s;
	}

	s.status = (int)commitTxn((DbHandle*)db.handle, txnId.txnBits);
	return s;
}

//	rollbackTxn(db, txnId)

value_t js_rollbackTxn(uint32_t args, environment_t *env) {
	value_t db, txnId;
	value_t s;

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : rollbackTxn\n");

	db = eval_arg (&args, env);

	if (vt_handle != db.type || Hndl_database != db.subType) {
		fprintf(stderr, "Error: rollbackTxn => expecting Db:handle => %s\n", strtype(db.type));
		return s.status = ERROR_script_internal, s;
	}

	txnId = eval_arg (&args, env);

	if (vt_txnId != txnId.type) {
		fprintf(stderr, "Error: beginTxn => expecting Db:txnId => %s\n", strtype(txnId.type));
		return s.status = ERROR_script_internal, s;
	}

	s.status = (int)rollbackTxn((DbHandle *)db.handle, txnId.txnBits);
	return s;
}


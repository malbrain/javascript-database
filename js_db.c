#include "js.h"
#include "js_props.h"
#include "js_dbindex.h"
#include "database/db.h"
#include "database/db_object.h"
#include "database/db_handle.h"
#include "database/db_arena.h"
#include "database/db_index.h"

Handle **arenaHandles = NULL;

//	convert Database Addr reference

void *js_addr(value_t val) {
DbAddr addr;
char *base;

	if (!val.marshaled)
		return val.addr;

	if (val.arenaAddr.storeId < vec_cnt(arenaHandles)) {
		Handle *arena = arenaHandles[val.arenaAddr.storeId];
		addr.bits = val.arenaAddr.bits;
		base = getObj(arena->map, addr);
		return base + val.offset;
	}

	fprintf (stderr, "error: js_addr: invalid docStore ID number %d\n", (int)val.arenaAddr.storeId);
	exit(0);
}

void js_deleteHandle(value_t val) {
	if (val.ishandle) {
		*val.handle = 0;
		js_free(val.raw);
		return;
	}

	fprintf (stderr, "error: js_deleteHandle: not handle: %s\n", strtype(val.type));
	exit(0);
}

uint32_t sizeOption(value_t val) {
	if (val.type == vt_string) {
		string_t *str = js_addr(val);
		return str->len + sizeof(string_t);
	}

	if (val.type == vt_object)
		return calcSize(val) + sizeof(value_t);

	return 0;
}

Params *processOptions(value_t options) {
	array_t *aval = js_addr(options);
	value_t *values = options.marshaled ? aval->valueArray : aval->valuePtr;
	uint32_t cnt = options.marshaled ? aval->cnt : vec_cnt(aval->valuePtr);
	Params *params;
	uint32_t size;

	size = sizeof(Params) * (MaxParam + 1);

	if (cnt > MaxParam + 1)
		cnt = MaxParam + 1;

	if (!(params = js_alloc(size, true))) {
		fprintf (stderr, "processOptions: out of memory!\n");
		exit(1);
	}

	params[Size].intVal = size;

	//	process the passed params array

	for (int idx = 0; idx < cnt; idx++) {
	  switch (idx) {
		case OnDisk:
			params[idx].boolVal = conv2Bool(values[idx], false).boolean;
			break;

		case InitSize:
			params[idx].intVal = conv2Int(values[idx], false).nval;
			break;

		case UseTxn:
			params[idx].boolVal = conv2Bool(values[idx], false).boolean;
			break;

		case NoDocs:
			params[idx].boolVal = conv2Bool(values[idx], false).boolean;
			break;

		case DropDb:
			params[idx].boolVal = conv2Bool(values[idx], false).boolean;
			break;

		case IdxKeyUnique:
			params[idx].boolVal = conv2Bool(values[idx], false).boolean;
			break;

		case IdxBinary:
			params[idx].boolVal = conv2Bool(values[idx], false).boolean;
			break;

		case IdxType:
			params[idx].intVal += conv2Int(values[idx], false).nval;
			break;

		case IdxKeySparse:
			params[idx].boolVal = conv2Bool(values[idx], false).boolean;
			break;

		case Btree1Bits:
			params[idx].intVal = conv2Int(values[idx], false).nval;
			break;

		case Btree1Xtra:
			params[idx].intVal = conv2Int(values[idx], false).nval;
			break;

		case CursorTxn:
			params[idx].intVal = conv2Int(values[idx], false).nval;
			break;
	  }
	}

	return params;
}

PropFcn builtinDbFcns[] = {
//	{ fcnDbOnDisk, "onDisk" },
	{ NULL, NULL}
};

PropVal builtinDbProp[] = {
//	{ propDbOnDisk, "onDisk" },
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

PropFcn builtinTxnFcns[] = {
//	{ fcnTxnBeginTxn, "beginTxn" },
	{ NULL, NULL}
};

PropFcn builtinTxnProp[] = {
//	{ propTxnBeginTxn, "beginTxn" },
	{ NULL, NULL}
};

//	closeHandle (handle)

value_t js_closeHandle(uint32_t args, environment_t *env) {
	value_t hndl, s;

	s.bits = vt_status;
	hndl = eval_arg (&args, env);

	if (!hndl.ishandle) {
		fprintf(stderr, "Error: closeHandle => expecting Handle type => %s\n", strtype(hndl.type));
		return s.status = ERROR_script_internal, s;
	}

	return s.status = closeHandle(hndl.addr), s;
}

//	openDatabase(name, options)

value_t js_openDatabase(uint32_t args, environment_t *env) {
	value_t v, opts, name;
	string_t *namestr;
	Params *params;
	DbHandle db[1];
	value_t s;

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : openDatabase\n");

	name = eval_arg (&args, env);

	if (name.type == vt_string)
		namestr = js_addr(name);
	else {
		fprintf(stderr, "Error: openDatabase => expecting dbname:string => %s\n", strtype(name.type));
		return s.status = ERROR_script_internal, s;
	}

	// process options array

	opts = eval_arg (&args, env);

	if (opts.type == vt_array)
		params = processOptions(opts);
	else {
		fprintf(stderr, "Error: openDatabase => expecting options:array => %s\n", strtype(opts.type));
		return s.status = ERROR_script_internal, s;
	}

	abandonValue(opts);

	if ((s.status = (int)openDatabase(db, namestr->val, namestr->len, params)))
		return s;

	v.bits = vt_db;
	v.ishandle = 1;
	v.refcount = 1;
	v.subType = Hndl_database;
	v.handle = js_alloc(sizeof(DbHandle), false);
	*v.handle = db->hndlBits;

	abandonValue(name);
	js_free(params);
	return v;
}

//  createIndex(docStore, name, options, keySpec)

value_t js_createIndex(uint32_t args, environment_t *env) {
	value_t docStore, opts, name, spec;
	Handle *idxHndl, *docHndl;
	string_t *namestr;
	DbHandle idx[1];
	DbIndex *index;
	Params *params;
	value_t s;

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : CreateIndex\n");

	docStore = eval_arg (&args, env);

	if (vt_store != docStore.type || Hndl_docStore != docStore.subType) {
		fprintf(stderr, "Error: createIndex => expecting docStore:handle => %s\n", strtype(docStore.type));
		return s.status = ERROR_script_internal, s;
	}

	name = eval_arg (&args, env);

	if (name.type == vt_string)
		namestr = js_addr(name);
	else {
		fprintf(stderr, "Error: createIndex => expecting name:string => %s\n", strtype(name.type));
		return s.status = ERROR_script_internal, s;
	}

	// process options array

	opts = eval_arg (&args, env);

	if (opts.type == vt_array)
		params = processOptions(opts);
	else {
		fprintf(stderr, "Error: createIndex => expecting options:array => %s\n", strtype(opts.type));
		return s.status = ERROR_script_internal, s;
	}

	abandonValue(opts);

	//	process keyspec object

	spec = eval_arg (&args, env);

	if (spec.type != vt_object) {
		fprintf(stderr, "Error: createIndex => expecting keyspec:object => %s\n", strtype(spec.type));
		return s.status = ERROR_script_internal, s;
	}

	//  create the index arena

	if ((s.status = (int)createIndex(idx, (DbHandle *)docStore.handle, namestr->val, namestr->len, params)))
		return s;

	idxHndl = bindHandle(idx);
	index = dbindex(idxHndl->map);

	// compile our key definition object

	index->keys = compileKeys(idxHndl->map, spec);

	s.bits = vt_index;
	s.subType = params[IdxType].intVal;
	s.ishandle = 1;
	s.refcount = 1;

	s.handle = js_alloc(sizeof(DbHandle), false);
	*s.handle = idx->hndlBits;

	docHndl = bindHandle((DbHandle *)docStore.handle);
	dbInstallIndexes(docHndl);
	releaseHandle(docHndl, (DbHandle *)docStore.handle);

	releaseHandle(idxHndl, idx);
	abandonValue(name);
	js_free(params);
	return s;
}

//  createCursor(index, txnId, options)

value_t js_createCursor(uint32_t args, environment_t *env) {
	value_t index, opts;
	DbHandle cursor[1];
	Params *params;
	value_t s, v;
	ObjId txnId;

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : createCursor\n");

	index = eval_arg (&args, env);

	if (vt_index != index.type) {
		fprintf(stderr, "Error: createCursor => expecting index:handle => %s\n", strtype(index.type));
		return s.status = ERROR_script_internal, s;
	}

	v = eval_arg(&args, env);

	if (vt_txnId != v.type && vt_undef != v.type && vt_null != v.type) {
		fprintf(stderr, "Error: createCursor => expecting TxnId => %s\n", strtype(v.type));
		return s.status = ERROR_script_internal, s;
	}

	if (vt_txnId == v.type)
		txnId.bits = v.txnBits;
	else
		txnId.bits = 0;

	abandonValue(v);

	// process options array

	opts = eval_arg (&args, env);

	if (opts.type == vt_array)
		params = processOptions(opts);
	else if (opts.type != vt_undef) {
		fprintf(stderr, "Error: createCursor => expecting options:array => %s\n", strtype(opts.type));
		return s.status = ERROR_script_internal, s;
	}

	abandonValue(opts);

	if ((s.status = (int)createCursor(cursor, (DbHandle *)index.handle, params, txnId)))
		return s;

	s.bits = vt_cursor;
	s.subType = Hndl_cursor;
	s.ishandle = 1;
	s.refcount = 1;
	s.handle = js_alloc(sizeof(DbHandle), false);
	*s.handle = cursor->hndlBits;

	js_free(params);
	return s;
}

//	openDocStore(database, options)

value_t js_openDocStore(uint32_t args, environment_t *env) {
	value_t database, opts, name;
	DbHandle docStore[1];
	string_t *namestr;
	Params *params;
	value_t s;
	int diff;

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : openDocStore\n");

	database = eval_arg (&args, env);

	if (vt_db != database.type || Hndl_database != database.subType) {
		fprintf(stderr, "Error: openDocStore => expecting Database handle => %s\n", strtype(database.type));
		return s.status = ERROR_script_internal, s;
	}

	name = eval_arg (&args, env);

	if (name.type == vt_string)
		namestr = js_addr(name);
	else {
		fprintf(stderr, "Error: openDocStore => expecting name:string => %s\n", strtype(name.type));
		return s.status = ERROR_script_internal, s;
	}

	// process options array

	opts = eval_arg (&args, env);

	if (opts.type == vt_array)
		params = processOptions(opts);
	else {
		fprintf(stderr, "Error: openDocStore => expecting options:array => %s\n", strtype(opts.type));
		return s.status = ERROR_script_internal, s;
	}

	abandonValue(opts);

	if ((s.status = (int)openDocStore(docStore, (DbHandle *)database.handle, namestr->val, namestr->len, params)))
		return s;

	diff = params[DocStoreId].intVal - vec_cnt(arenaHandles) + 1;

	if (diff > 0)
		vec_add(arenaHandles, diff);

	arenaHandles[params[DocStoreId].intVal] = bindHandle(docStore);

	s.bits = vt_store;
	s.subType = Hndl_docStore;
	s.ishandle = 1;
	s.refcount = 1;
	s.handle = js_alloc(sizeof(DbHandle), false);
	*s.handle = docStore->hndlBits;

	abandonValue(name);
	js_free(params);
	return s;
}

//  js_createIterator(docStore, txnId, options)

value_t js_createIterator(uint32_t args, environment_t *env) {
	value_t docStore, opts;
	DbHandle iter[1];
	Params *params;
	value_t s, v;
	ObjId txnId;

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : CreateIterator\n");

	docStore = eval_arg (&args, env);

	if (vt_store != docStore.type || Hndl_docStore != docStore.subType) {
		fprintf(stderr, "Error: createIterator => expecting docStore:Handle => %s\n", strtype(docStore.type));
		return s.status = ERROR_script_internal, s;
	}

	v = eval_arg(&args, env);

	if (vt_txnId != v.type && vt_undef != v.type && vt_null != v.type) {
		fprintf(stderr, "Error: createIterator => expecting TxnId => %s\n", strtype(v.type));
		return s.status = ERROR_script_internal, s;
	}

	if (vt_txnId == v.type)
		txnId.bits = v.txnBits;
	else
		txnId.bits = 0;

	abandonValue(v);

	// process options array

	opts = eval_arg (&args, env);

	if (opts.type == vt_array)
		params = processOptions(opts);
	else {
		fprintf(stderr, "Error: createIterator => expecting options:array => %s\n", strtype(opts.type));
		return s.status = ERROR_script_internal, s;
	}

	abandonValue(opts);

	if ((s.status = (int)createIterator(iter, (DbHandle *)docStore.handle, txnId, params)))
		return s;

	s.bits = vt_iter;
	s.subType = Hndl_iterator;
	s.ishandle = 1;
	s.refcount = 1;
	s.handle = js_alloc(sizeof(DbHandle), false);
	*s.handle = iter->hndlBits;

	js_free(params);
	return s;
}

//	beginTxn(db, options)

value_t js_beginTxn(uint32_t args, environment_t *env) {
	value_t db, txnId, opts;
	Params *params;
	value_t s;

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : beginTxn\n");

	db = eval_arg (&args, env);

	if (vt_db != db.type || Hndl_database != db.subType) {
		fprintf(stderr, "Error: beginTxn => expecting Db:handle => %s\n", strtype(db.type));
		return s.status = ERROR_script_internal, s;
	}

	// process options array

	opts = eval_arg (&args, env);

	if (opts.type == vt_array)
		params = processOptions(opts);
	else {
		fprintf(stderr, "Error: createIterator => expecting options:array => %s\n", strtype(opts.type));
		return s.status = ERROR_script_internal, s;
	}

	abandonValue(opts);

	txnId.bits = vt_txnId;

	if(!(txnId.txnBits = beginTxn((DbHandle *)db.handle, params).bits))
		s.status = ERROR_outofmemory;

	js_free(params);
	return s;
}

//	commitTxn(db, txnId)

value_t js_commitTxn(uint32_t args, environment_t *env) {
	value_t db, txn, s;
	ObjId txnId;
	
	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : commitTxn\n");

	db = eval_arg (&args, env);

	if (vt_db != db.type || Hndl_database != db.subType) {
		fprintf(stderr, "Error: commitTxn => expecting Db:handle => %s\n", strtype(db.type));
		return s.status = ERROR_script_internal, s;
	}

	txn = eval_arg (&args, env);
	txnId.bits = txn.txnBits;

	if (vt_txnId != txn.type) {
		fprintf(stderr, "Error: commitTxn => expecting Db:txnId => %s\n", strtype(txn.type));
		return s.status = ERROR_script_internal, s;
	}

	s.status = (int)commitTxn((DbHandle*)db.handle, txnId);
	return s;
}

//	rollbackTxn(db, txnId)

value_t js_rollbackTxn(uint32_t args, environment_t *env) {
	value_t db, txn, s;
	ObjId txnId;

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : rollbackTxn\n");

	db = eval_arg (&args, env);

	if (vt_db != db.type || Hndl_database != db.subType) {
		fprintf(stderr, "Error: rollbackTxn => expecting Db:handle => %s\n", strtype(db.type));
		return s.status = ERROR_script_internal, s;
	}

	txn = eval_arg (&args, env);
	txnId.bits = txn.txnBits;

	if (vt_txnId != txn.type) {
		fprintf(stderr, "Error: rollbackTxn => expecting Db:txnId => %s\n", strtype(txn.type));
		return s.status = ERROR_script_internal, s;
	}

	s.status = (int)rollbackTxn((DbHandle *)db.handle, txnId);
	return s;
}


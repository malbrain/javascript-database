#include "js.h"
#include "js_db.h"
#include "js_dbindex.h"
#include "js_props.h"

extern value_t newDate(value_t *args);

//	vector of handles to all docStore for all db

extern char hndlInit[1];
extern DbMap *hndlMap;
extern Catalog *catalog;
// extern CcMethod *cc;

void js_deleteHandle(value_t val) {
	if (val.ishandle) {
		val.hndl->hndlId.bits = 0;
		return;
	}

	fprintf (stderr, "error: js_deleteHandle: not handle: %s\n", strtype(val.type));
	exit(0);
}

void processOptions(Params *params, value_t options) {
	uint32_t size = sizeof(Params) * (MaxParam + 1);
	dbarray_t *dbaval = js_addr(options);
	uint32_t cnt, idx;
	value_t *values;

	memset (params, 0, size);
	params[Size].intVal = size;

	if (options.type == vt_endlist)
		return;

	if (options.type != vt_array) {
		fprintf(stderr, "Error: processOptions => expecting options:array => %s\n", strtype(options.type));
		return;
	}

	values = options.marshaled ? dbaval->valueArray : options.aval->valuePtr;
	cnt = options.marshaled ? dbaval->cnt : vec_cnt(values);

	if (cnt > MaxParam + 1)
		cnt = MaxParam + 1;

	//	process the passed params array

	for (idx = 0; idx < cnt; idx++) {
	  switch (idx) {
		case OnDisk:
			params[idx].boolVal = conv2Bool(values[idx], false).boolean;
			break;

		case InitSize:
			params[idx].intVal = conv2Int(values[idx], false).nval;
			break;

		case IdxKeyUnique:
			params[idx].boolVal = conv2Bool(values[idx], false).boolean;
			break;

		case IdxKeyFlds:
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

		case CursorDeDup:
			params[idx].boolVal = conv2Bool(values[idx], false).boolean;
			break;

		case Concurrency:
			params[idx].intVal = conv2Int(values[idx], false).nval;
			break;
	  }
	}
}

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

//	openCatalog(path, name, txnisolation)

value_t js_openCatalog(uint32_t args, environment_t *env) {
	value_t s, path, name, v, dbs;
	string_t *namestr, *pathstr;
	PathStk pathStk[1];
	RedBlack *entry;

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : openCatalog\n");

	path = eval_arg(&args, env);

	if (vt_string == path.type)
		pathstr = js_addr(path);
	else {
		fprintf(stderr, "Error: openCatalog => expecting path:string => %s\n", strtype(path.type));
		return s.status = ERROR_script_internal, s;
	}

	name = eval_arg(&args, env);

	if (vt_string == name.type)
		namestr = js_addr(name);
	else {
		fprintf(stderr, "Error: openCatalog => expecting name:string => %s\n", strtype(name.type));
		return s.status = ERROR_script_internal, s;
	}

	if (!*hndlInit)
		initHndlMap((char *)pathstr->val, pathstr->len, (char *)namestr->val, namestr->len, namestr->len, sizeof(CcMethod));

	catalog =(Catalog *)(hndlMap->arena + 1);
//	cc = (CcMethod *)(catalog + 1);

	v = eval_arg(&args, env);
/*
	if (v.type == vt_int)
		cc->isolation = v.nval;
*/
	abandonValue(name);
	abandonValue(path);

	dbs = newObject(vt_object);

	lockLatch(hndlMap->arenaDef->nameTree->latch);

	if ((entry = rbStart (hndlMap, pathStk, hndlMap->arenaDef->nameTree)))
	  do {
		ArenaDef *arenaDef = (ArenaDef *)(entry + 1);
		if (*arenaDef->dead & KILL_BIT)
			continue;
		value_t dbname = newString(rbkey(entry), entry->keyLen);
		v = lookup(dbs, dbname, true, 0);
		v.lval->bits = vt_date;
		v.lval->date = arenaDef->creation * 1000ULL;
	  } while ((entry = rbNext(hndlMap, pathStk)));

	unlockLatch(hndlMap->arenaDef->nameTree->latch);
	return dbs;
}

//	openDatabase(name, options)

value_t js_openDatabase(uint32_t args, environment_t *env) {
	Params params[MaxParam + 1];
	value_t v, opts, name;
	string_t *namestr;
	DbHandle hndl[1];
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
	processOptions(params, opts);
	abandonValue(opts);

	if ((s.status = (int)openDatabase(hndl, (char *)namestr->val, namestr->len, params)))
		return s;

	v.bits = vt_db;
    v.ishandle = 1;
	v.subType = Hndl_database;
	v.hndl->hndlId.bits = hndl->hndlId.bits;

	abandonValue(name);
	return v;
}

//  createIndex(docStore, name, options , keySpec)

value_t js_createIndex(uint32_t args, environment_t *env) {
	DbHandle idxDbHndl[1], docDbHndl[1], newIdxHndl[1];
	value_t store, opts, name, spec;
	Params params[MaxParam + 1];
    DbMap *docMap, *idxMap;
	string_t *namestr;
	Handle *docHndl;
    uint16_t idx;
	value_t s;

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : CreateIndex\n");

	store = eval_arg (&args, env);

	if (store.type == vt_object)
		store = *baseObject(store);

	if (vt_store != store.type || Hndl_docStore != store.subType) {
		fprintf(stderr, "Error: createIndex => expecting store:handle => %s\n", strtype(store.type));
		return s.status = ERROR_script_internal, s;
	}

	docDbHndl->hndlId.bits = store.hndl->hndlId.bits;

	if (!(docHndl = bindHandle(docDbHndl, Hndl_docStore)))
		return s.status = DB_ERROR_arenadropped, s;

	docMap = MapAddr(docHndl);
	name = eval_arg (&args, env);

	if (name.type == vt_string)
		namestr = js_addr(name);
	else {
		fprintf(stderr, "Error: createIndex => expecting name:string => %s\n", strtype(name.type));
		return s.status = ERROR_script_internal, s;
	}

	// process options array

	opts = eval_arg (&args, env);
	processOptions(params, opts);
	abandonValue(opts);

	//	process keyspec object

	spec = eval_arg (&args, env);

	if (spec.type == vt_object)
		params[IdxKeyAddr].addr = compileKeys(docDbHndl, spec);

	abandonValue(spec);

	//  create the index arena

	if ((s.status = (int)createIndex(idxDbHndl, docDbHndl, (char *)namestr->val, namestr->len, params)))
		return s;

	s.bits = vt_index;
	s.subType = (uint32_t)params[IdxType].intVal;
	s.ishandle = 1;
    s.hndl->hndlId.bits = idxDbHndl->hndlId.bits;

	//  install the new index in the docStore

//	idx = arrayAlloc(mapAddr(docHndl), docIdx->idxHndls, sizeof(DbHandle));
//	idxHndlAddr = arrayEntry(mapAddr(docHndl), docIdx->idxHndls, idx);
//    newDbHndl->hndlBits = idxDbHndl->hndlBits;

//	if (idx >= docStore->idxMax)
//		docStore->idxMax = idx + 1;

//	abandonValue(name);
	return s;
}

//  createCursor(docStore, index, options)

value_t js_createCursor(uint32_t args, environment_t *env) {
	value_t index, opts, docStore;
	Params params[MaxParam + 1];
	DbHandle cursor[1];
	DbCursor *dbCursor;
	Handle *idxHndl;
	value_t s;

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : createCursor\n");

	docStore = eval_arg (&args, env);

	if (docStore.type == vt_object)
		docStore = *baseObject(docStore);

	if (vt_store != docStore.type || Hndl_docStore != docStore.subType) {
		fprintf(stderr, "Error: createCursor => expecting docStore:handle => %s\
n", strtype(docStore.type));
		return s.status = ERROR_script_internal, s;
	}

	index = eval_arg (&args, env);

	if (index.type == vt_object)
		index = *baseObject(index);

	if (vt_index != index.type) {
		fprintf(stderr, "Error: createCursor => expecting index:handle => %s\n", strtype(index.type));
		return s.status = ERROR_script_internal, s;
	}

	// process options array

	opts = eval_arg (&args, env);
	processOptions(params, opts);
	abandonValue(opts);

	params[ClntSize].intVal = sizeof(DbMvcc);

	if ((s.status = (int)createCursor(cursor, (DbHandle *)index.hndl, params)))
		return s;

	if ((idxHndl = bindHandle(cursor, Hndl_anyIdx)))
		dbCursor = ClntAddr(idxHndl);
	else {
		fprintf(stderr, "Error: createCursor => unable to bind index:Handle\n");
		return s.status = ERROR_script_internal, s;
	}

	s.bits = vt_cursor;
	s.subType = Hndl_cursor;
	s.ishandle = 1;
    s.hndl->hndlId.bits = cursor->hndlId.bits;

	releaseHandle(idxHndl, (DbHandle *)s.hndl);
	return s;
}

//	openDocStore(database, options)

value_t js_openDocStore(uint32_t args, environment_t *env) {
	value_t database, opts, name;
	Params params[MaxParam + 1];
	string_t *namestr;
	Handle *docHndl;
	DbHandle hndl[1];
	PathStk pathStk[1];
	DocStore *docStore;
	RedBlack *entry;
    DbMap *map;
	value_t s;

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : openDocStore\n");

	database = eval_arg (&args, env);

	if (database.type == vt_object)
		database = *baseObject(database);

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
	processOptions(params, opts);
	abandonValue(opts);

	params[ClntXtra].intVal = sizeof(DocStore);

	if ((s.status = (int)openDocStore(hndl, (DbHandle *)database.hndl, (char *)namestr->val, namestr->len, params)))
		return s;

	if (!(docHndl = bindHandle(hndl, Hndl_docStore)))
		return s.status = DB_ERROR_arenadropped, s;
/*
	docStore = ClntAddr(docHndl);

	//	open indexes

	map = MapAddr(docHndl);
	lockLatch(map->arenaDef->nameTree->latch);

	if ((entry = rbStart (map, pathStk, map->arenaDef->nameTree)))
	  do {
		DbHandle *idxHndl;
		Handle *handle;
		uint16_t idx;
		DbMap *childMap;

		if ((childMap = arenaRbMap(map, entry))) {
    	  if ((handle = makeHandle(childMap, 0, *map->arena->type, params[HndlXtra].intVal))) {
			idx = arrayAlloc(docHndl->map, docStore->idxHndls, sizeof(DbHandle));
			idxHndl = arrayEntry(docHndl->map, docStore->idxHndls, idx);
        	idxHndl->hndlBits = handle->hndlId.bits;

			if (idx >= docStore->idxMax)
				docStore->idxMax = idx + 1;
		  }
		}
	  } while ((entry = rbNext(docHndl->map, pathStk)));

	unlockLatch(map->arenaDef->nameTree->latch);
*/
	s.bits = vt_store;
	s.subType = Hndl_docStore;
	s.ishandle = 1;
    s.hndl->hndlId.bits = hndl->hndlId.bits;

	releaseHandle(docHndl, (DbHandle *)s.hndl);
	abandonValue(name);
	return s;
}

//  js_createIterator(docStore, options)

value_t js_createIterator(uint32_t args, environment_t *env) {
	Params params[MaxParam + 1];
	value_t docStore, opts;
	Iterator *iterator;
	DbHandle iter[1];
	Handle *docHndl;
	value_t s;

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : CreateIterator\n");

	docStore = eval_arg (&args, env);

	if (docStore.type == vt_object)
		docStore = *baseObject(docStore);

	if (vt_store != docStore.type || Hndl_docStore != docStore.subType) {
		fprintf(stderr, "Error: createIterator => expecting docStore:Handle => %s\n", strtype(docStore.type));
		return s.status = ERROR_script_internal, s;
	}

	// process options array

	opts = eval_arg (&args, env);
	processOptions(params, opts);
	abandonValue(opts);

	params[ClntSize].intVal = sizeof(DbMvcc);

	if ((s.status = (int)createIterator(iter, (DbHandle *)docStore.hndl, params)))
		return s;

	docHndl = bindHandle(iter, Hndl_iterator);
	iterator = ClntAddr(docHndl);

	s.bits = vt_iter;
	s.subType = Hndl_iterator;
	s.ishandle = 1;
	s.hndl->hndlId.bits = iter->hndlId.bits;

	releaseHandle(docHndl, (DbHandle *)s.hndl);
	return s;
}

value_t fcnDbDrop(value_t *args, value_t thisVal, environment_t *env) {
	bool dropDefs = false;
	DbHandle *hndl;
	value_t s;

	s.bits = vt_status;

	if( (hndl = (DbHandle *)baseObject(thisVal)->hndl) )
	  if (vec_cnt(args) && args->type == vt_bool)
		dropDefs = args->boolean;
		
	s.status = (int)dropArena (hndl, dropDefs);
	return s;
}

//	beginTxn(options)

value_t js_beginTxn(uint32_t args, environment_t *env) {
	Params params[MaxParam + 1];
	value_t opts;
	value_t v;
/*
	if (debug) fprintf(stderr, "funcall : beginTxn\n");

	// process options array

	opts = eval_arg (&args, env);
	processOptions(params, opts);
	abandonValue(opts);
*/
	v.bits = vt_txn;
//	v.idBits = beginTxn(params, env->txnBits, env->timestamp);
	return v;
}

//	commitTxn(options)

value_t js_commitTxn(uint32_t args, environment_t *env) {
	Params params[MaxParam + 1];
	uint64_t txnBits;
	value_t opts, v;
	
	v.bits = vt_status;
/*
	if (debug) fprintf(stderr, "funcall : commitTxn\n");

	// process options array

	opts = eval_arg (&args, env);
	processOptions(params, opts);
	abandonValue(opts);

	txnBits = *env->txnBits;

	if (!(v.status = (Status)commitTxn(params, env->txnBits, env->timestamp))) {
		v.bits = vt_txn;
		v.idBits = txnBits;
	}
*/
	return v;
}

//	rollbackTxn()

value_t js_rollbackTxn(uint32_t args, environment_t *env) {
	Params params[MaxParam + 1];
	uint64_t txnBits;
	value_t opts, v;

	v.bits = vt_status;
/*
	if (debug) fprintf(stderr, "funcall : rollbackTxn\n");

	// process options array

	opts = eval_arg (&args, env);
	processOptions(params, opts);

	txnBits = *env->txnBits;

	if (!(v.status = (Status)rollbackTxn(params, env->txnBits))) {
		v.bits = vt_txn;
		v.idBits = txnBits;
	}
*/
	return v;
}

PropFcn builtinDbFcns[] = {
	{ fcnDbDrop, "drop" },
	{ NULL, NULL}
};

PropVal builtinDbProp[] = {
//	{ propDbOnDisk, "onDisk" },
	{ NULL, NULL}
};

PropFcn builtinCatalogFcns[] = {
	{ NULL, NULL}
};

PropVal builtinCatalogProp[] = {
	{ NULL, NULL}
};

PropFcn builtinTxnFcns[] = {
//	{ fcnTxnToString, "toString" },
	{ NULL, NULL}
};

PropVal builtinTxnProp[] = {
//	{ propTxnCount, "count" },
	{ NULL, NULL}
};


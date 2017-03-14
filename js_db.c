#include "js.h"
#include "js_db.h"
#include "js_dbindex.h"
#include "js_props.h"

//	vector of handles to all docStore for all db

Handle **arenaHandles = NULL;
extern CcMethod *cc;
DbMap *hndlMap;

//	convert Database Addr reference

void *js_dbaddr(value_t val) {
uint8_t *docBase;
DbAddr addr;

	addr.bits = val.addrBits;

	if (addr.xtra < vec_cnt(arenaHandles)) {
		Handle *arena = arenaHandles[addr.xtra];
		docBase = getObj(arena->map, addr);
		return docBase + val.offset;
	}

	fprintf (stderr, "error: js_addr: invalid docStore ID number %d\n", (int)addr.xtra);
	exit(0);
}

void js_deleteHandle(value_t val) {
	if (val.ishandle) {
		*val.hndl = 0;
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
		return calcSize(val, false) + sizeof(value_t);

	return 0;
}

Params *processOptions(value_t options) {
	dbarray_t *dbaval = js_addr(options);
	value_t *values;
	Params *params;
	uint32_t size;
	uint32_t cnt;

	size = sizeof(Params) * (MaxParam + 1);

	if (!(params = js_alloc(size, true))) {
		fprintf (stderr, "processOptions: out of memory!\n");
		exit(1);
	}

	params[Size].intVal = size;

	if (options.type == vt_undef)
		return params;

	if (options.type != vt_array) {
		fprintf(stderr, "Error: createIndex => expecting options:array => %s\n", strtype(options.type));
		return params;
	}

	values = options.marshaled ? dbaval->valueArray : options.aval->valuePtr;
	cnt = options.marshaled ? dbaval->cnt : vec_cnt(values);

	if (cnt > MaxParam + 1)
		cnt = MaxParam + 1;

	//	process the passed params array

	for (int idx = 0; idx < cnt; idx++) {
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
	string_t *namestr, *pathstr;
	value_t s, path, name, v;
	DbHandle hndl[1];

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : openCatalog\n");

	path = eval_arg(&args, env);

	if (vt_string != path.type) {
		fprintf(stderr, "Error: openCatalog => expecting path:string => %s\n", strtype(path.type));
		return s.status = ERROR_script_internal, s;
	}

	pathstr = js_addr(path);

	name = eval_arg(&args, env);

	if (vt_string != name.type) {
		fprintf(stderr, "Error: openCatalog => expecting name:string => %s\n", strtype(name.type));
		return s.status = ERROR_script_internal, s;
	}

	namestr = js_addr(name);

	cc = initHndlMap((char *)pathstr->val, pathstr->len, (char *)namestr->val, namestr->len, namestr->len, sizeof(CcMethod));

	abandonValue(name);
	abandonValue(path);

	v = eval_arg(&args, env);

	if (v.type == vt_int)
		cc->isolation = v.nval;

	hndl->hndlBits = makeHandle(hndlMap, 0, Hndl_catalog)->hndlId.bits;

	v.bits = vt_catalog;
	v.ishandle = 1;
	v.refcount = 1;
	v.subType = Hndl_catalog;
	v.hndl = js_alloc(sizeof(DbHandle), false);
	*v.hndl = hndl->hndlBits;
	return v;
}

//	openDatabase(name, options)

value_t js_openDatabase(uint32_t args, environment_t *env) {
	value_t v, opts, name;
	string_t *namestr;
	DbHandle hndl[1];
	Params *params;
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
	params = processOptions(opts);
	abandonValue(opts);

	if ((s.status = (int)openDatabase(hndl, (char *)namestr->val, namestr->len, params)))
		return s;

	v.bits = vt_db;
	v.ishandle = 1;
	v.refcount = 1;
	v.subType = Hndl_database;
	v.hndl = js_alloc(sizeof(DbHandle), false);
	*v.hndl = hndl->hndlBits;

	abandonValue(name);
	js_free(params);
	return v;
}

//  createIndex(docStore, name, options, keySpec)

value_t js_createIndex(uint32_t args, environment_t *env) {
	value_t docStore, opts, name, spec;
	string_t *namestr;
	DbHandle idx[1];
	Params *params;
	value_t s;

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : CreateIndex\n");

	docStore = eval_arg (&args, env);

	if (docStore.type == vt_object)
		docStore = *baseObject(docStore);

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
	params = processOptions(opts);
	abandonValue(opts);

	//	process keyspec object

	spec = eval_arg (&args, env);

	if (spec.type == vt_object)
		params[IdxKeyAddr].addr = compileKeys((DbHandle *)docStore.hndl, spec);

	abandonValue(spec);

	//  create the index arena

	if ((s.status = (int)createIndex(idx, (DbHandle *)docStore.hndl, (char *)namestr->val, namestr->len, params)))
		return s;

	s.bits = vt_index;
	s.subType = params[IdxType].intVal;
	s.ishandle = 1;
	s.refcount = 1;

	s.hndl = js_alloc(sizeof(DbHandle), false);
	*s.hndl = idx->hndlBits;

	abandonValue(name);
	js_free(params);
	return s;
}

//  createCursor(docStore, index, options)

value_t js_createCursor(uint32_t args, environment_t *env) {
	value_t index, opts, docStore;
	DbHandle cursor[1];
	DbCursor *dbCursor;
	Handle *idxHndl;
	JsMvcc *jsMvcc;
	Params *params;
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
	params = processOptions(opts);
	abandonValue(opts);

	params[HndlXtra].intVal = sizeof(JsMvcc);

	if ((s.status = (int)createCursor(cursor, (DbHandle *)index.hndl, params)))
		return s;

	idxHndl = bindHandle(cursor);
	dbCursor = (DbCursor *)(idxHndl + 1);

	jsMvcc = (JsMvcc *)(dbCursor + 1);
	jsMvcc->hndl->hndlBits = *docStore.hndl;

	getSnapshotTimestamp(jsMvcc, false);

	s.bits = vt_cursor;
	s.subType = Hndl_cursor;
	s.ishandle = 1;
	s.refcount = 1;
	s.hndl = js_alloc(sizeof(DbHandle), false);
	*s.hndl = cursor->hndlBits;

	releaseHandle(idxHndl, cursor);
	js_free(params);
	return s;
}

//	openDocStore(database, options)

value_t js_openDocStore(uint32_t args, environment_t *env) {
	value_t database, opts, name;
	string_t *namestr;
	Handle *docHndl;
	DbHandle hndl[1];
	Params *params;
	value_t s;
	int diff;

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
	params = processOptions(opts);
	abandonValue(opts);

	params[HndlXtra].intVal = sizeof(DocStore);

	if ((s.status = (int)openDocStore(hndl, (DbHandle *)database.hndl, (char *)namestr->val, namestr->len, params)))
		return s;

	if (!(docHndl = bindHandle(hndl)))
		return s.status = DB_ERROR_arenadropped, s;

	if (!docHndl->map->arenaDef->storeId)
		return s.status = ERROR_toomany_local_docstores, s;

	diff = docHndl->map->arenaDef->storeId - vec_cnt(arenaHandles) + 1;

	if (diff > 0)
		vec_add(arenaHandles, diff);

	arenaHandles[docHndl->map->arenaDef->storeId] = docHndl;

	s.bits = vt_store;
	s.subType = Hndl_docStore;
	s.ishandle = 1;
	s.refcount = 1;
	s.hndl = js_alloc(sizeof(DbHandle), false);
	*s.hndl = hndl->hndlBits;

	abandonValue(name);
	js_free(params);
	return s;
}

//  js_createIterator(docStore, options)

value_t js_createIterator(uint32_t args, environment_t *env) {
	value_t docStore, opts;
	Iterator *iterator;
	DbHandle iter[1];
	Handle *docHndl;
	JsMvcc *jsMvcc;
	Params *params;
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
	params = processOptions(opts);
	abandonValue(opts);

	params[HndlXtra].intVal = sizeof(JsMvcc);

	if ((s.status = (int)createIterator(iter, (DbHandle *)docStore.hndl, params)))
		return s;

	docHndl = bindHandle(iter);
	iterator = (Iterator *)(docHndl + 1);

	jsMvcc = (JsMvcc *)(iterator + 1);
	jsMvcc->txnId.bits = *env->txnBits;

	getSnapshotTimestamp(jsMvcc, false);

	s.bits = vt_iter;
	s.subType = Hndl_iterator;
	s.ishandle = 1;
	s.refcount = 1;
	s.hndl = js_alloc(sizeof(DbHandle), false);
	*s.hndl = iter->hndlBits;

	js_free(params);
	return s;
}

extern uint64_t beginTxn(Params *params, uint64_t *txnBits);
extern JsStatus commitTxn(Params *params, uint64_t *txnBits);
extern JsStatus rollbackTxn(Params *params, uint64_t *txnBits);

//	beginTxn(options)

value_t js_beginTxn(uint32_t args, environment_t *env) {
	Params *params;
	value_t opts;
	value_t v;

	if (debug) fprintf(stderr, "funcall : beginTxn\n");

	// process options array

	opts = eval_arg (&args, env);
	params = processOptions(opts);
	abandonValue(opts);

	v.bits = vt_txn;
	v.idBits = beginTxn(params, env->txnBits);
	js_free(params);
	return v;
}

//	commitTxn(options)

value_t js_commitTxn(uint32_t args, environment_t *env) {
	uint64_t txnBits;
	value_t opts, v;
	Params *params;
	
	v.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : commitTxn\n");

	// process options array

	opts = eval_arg (&args, env);
	params = processOptions(opts);
	abandonValue(opts);

	txnBits = *env->txnBits;

	if (!(v.status = (Status)commitTxn(params, env->txnBits))) {
		v.bits = vt_txn;
		v.idBits = txnBits;
	}

	js_free(params);
	return v;
}

//	rollbackTxn()

value_t js_rollbackTxn(uint32_t args, environment_t *env) {
	uint64_t txnBits;
	value_t opts, v;
	Params *params;

	v.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : rollbackTxn\n");

	// process options array

	opts = eval_arg (&args, env);
	params = processOptions(opts);

	txnBits = *env->txnBits;

	if (!(v.status = (Status)rollbackTxn(params, env->txnBits))) {
		v.bits = vt_txn;
		v.idBits = txnBits;
	}

	js_free(params);
	return v;
}


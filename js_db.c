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
	uint32_t offsets[MaxParam + 1];
	uint32_t sizes[MaxParam + 1];
	Params *params;
	DbAddr addr;

	addr.bits = 0;

	memset (offsets, 0, sizeof(offsets));
	memset (sizes, 0, sizeof(sizes));

	sizes[Size] = sizeof(Params) * (MaxParam + 1);

	if (cnt > MaxParam + 1)
		cnt = MaxParam + 1;

	offsets[IdxKeySpec] = sizes[Size];
	sizes[Size] += sizes[IdxKeySpec] = sizeOption(values[IdxKeySpec]);

	offsets[IdxKeyPartial] = sizes[Size];
	sizes[Size] += sizes[IdxKeyPartial] = sizeOption(values[IdxKeyPartial]);

	if (!(params = js_alloc(sizes[Size], true))) {
		fprintf (stderr, "processOptions: out of memory!\n");
		exit(1);
	}

	params[Size].intVal = sizes[Size];

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

		case IdxKeyPartial:
		case IdxKeySpec:
			value_t *val;
			val = (value_t *)((uint8_t *)params + offsets[idx]);
			marshalDoc(values[idx], (uint8_t *)params, offsets[idx] + sizeof(value_t), addr, sizes[idx] - sizeof(value_t), val);

			params[idx].offset = offsets[idx];
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

value_t fcnIterNext(value_t *args, value_t *thisVal) {
	object_t *oval = js_addr(*thisVal);
	document_t *document;
	value_t next, s;
	DbHandle *hndl;
	Doc *doc;
	Ver *ver;

	s.bits = vt_status;

	hndl = (DbHandle *)oval->base->handle;

	if (!(ver = iteratorNext(hndl)))
		return s.status = ERROR_endoffile, s;
		
	doc = (Doc *)((uint8_t *)ver - ver->offset);

	next.bits = vt_document;
	next.addr = js_alloc(sizeof(document_t), true);
	next.refcount = true;

	document = next.addr;
	*document->handle = hndl->hndlBits;
	*document->addr = doc->addr.bits;
	document->ver = ver;
	return next;
}

value_t fcnIterPrev(value_t *args, value_t *thisVal) {
	object_t *oval = js_addr(*thisVal);
	document_t *document;
	value_t next, s;
	DbHandle *hndl;
	Doc *doc;
	Ver *ver;

	s.bits = vt_status;

	hndl = (DbHandle *)oval->base->handle;

	if (!(ver = iteratorPrev(hndl)))
		return s.status = ERROR_endoffile, s;
		
	doc = (Doc *)((uint8_t *)ver - ver->offset);

	next.bits = vt_document;
	next.addr = js_alloc(sizeof(document_t), true);
	next.refcount = true;

	document = next.addr;
	*document->handle = hndl->hndlBits;
	*document->addr = doc->addr.bits;
	document->ver = ver;
	return next;
}

value_t fcnIterSeek(value_t *args, value_t *thisVal) {
	object_t *oval = js_addr(*thisVal);
	IteratorPos pos = PosAt;
	document_t *document;
	value_t next, s;
	DbHandle *hndl;
	ObjId docId;
	Doc *doc;
	Ver *ver;

	s.bits = vt_status;

	hndl = (DbHandle *)oval->base->handle;

	if (args->type == vt_docId)
		docId.bits = args->docBits;
	else if (args->type == vt_int) {
		docId.bits = 0;
		pos = args->nval;
		iteratorSeek(hndl, pos, docId);
		return s.status = DB_OK, s;
	} else
		return s.status = ERROR_not_docid, s;

	 if (!(ver = iteratorSeek(hndl, pos, docId)))
		return s.status = ERROR_not_found, s;
		
	doc = (Doc *)((uint8_t *)ver - ver->offset);

	next.bits = vt_document;
	next.addr = js_alloc(sizeof(document_t), true);
	next.refcount = true;

	document = next.addr;
	*document->handle = hndl->hndlBits;
	*document->addr = doc->addr.bits;
	document->ver = ver;
	return next;
}

value_t fcnCursorMove(value_t *args, value_t *thisVal) {
	object_t *oval = js_addr(*thisVal);
	DbHandle *hndl;
	value_t op, s;

	s.bits = vt_status;
	hndl = (DbHandle *)oval->base->handle;

	op = conv2Int(args[0], false);
	s.status = moveCursor(hndl, op.nval);
	return s;
}

value_t fcnCursorPos(value_t *args, value_t *thisVal) {
	object_t *oval = js_addr(*thisVal);
	value_t op, s, key;
	DbHandle *hndl;
	string_t *str;

	s.bits = vt_status;
	hndl = (DbHandle *)oval->base->handle;

	op = conv2Int(args[0], false);
	key = conv2Str(args[1], false, false);
	str = js_addr(key);

	s.status = positionCursor(hndl, op.nval, str->val, str->len);
	return s;
}

value_t fcnCursorKeyAt(value_t *args, value_t *thisVal) {
	object_t *oval = js_addr(*thisVal);
	value_t op, s, key;
	uint32_t keyLen;
	DbHandle *hndl;
	char *keyStr;

	s.bits = vt_status;
	hndl = (DbHandle *)oval->base->handle;

	if ((s.status = keyAtCursor(hndl, &keyStr, &keyLen)))
		return s;

	return newString(keyStr, keyLen);
}

value_t fcnCursorDocAt(value_t *args, value_t *thisVal) {
	value_t s;

	s.bits = vt_status;
	return s;
}

value_t fcnCursorNextDoc(value_t *args, value_t *thisVal) {
	value_t s;

	s.bits = vt_status;
	return s;
}

value_t fcnCursorPrevDoc(value_t *args, value_t *thisVal) {
	value_t s;

	s.bits = vt_status;
	return s;
}

PropFcn builtinDbFcns[] = {
//	{ fcnDbOnDisk, "onDisk" },
	{ NULL, NULL}
};

PropVal builtinDbProp[] = {
//	{ propDbOnDisk, "onDisk" },
	{ NULL, NULL}
};

PropFcn builtinIterFcns[] = {
	{ fcnIterNext, "next" },
	{ fcnIterPrev, "prev" },
	{ fcnIterSeek, "seek" },
	{ NULL, NULL}
};

PropFcn builtinCursorFcns[] = {
	{ fcnCursorPos, "pos" },
	{ fcnCursorMove, "move" },
	{ fcnCursorKeyAt, "keyAt" },
	{ fcnCursorDocAt, "docAt" },
	{ fcnCursorNextDoc, "nextDoc" },
	{ fcnCursorPrevDoc, "prevDoc" },
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
	value_t docStore, opts, name;
	string_t *namestr;
	Handle *idxHndl;
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

	//  create the index arena

	if ((s.status = (int)createIndex(idx, (DbHandle *)docStore.handle, namestr->val, namestr->len, params)))
		return s;

	idxHndl = bindHandle(idx);
	index = dbindex(idxHndl->map);

	// compile our key definition object

	index->keys = compileKeys(idxHndl->map, idxHndl->map->arenaDef->params);

	s.bits = vt_index;
	s.subType = params[IdxType].intVal;
	s.ishandle = 1;
	s.refcount = 1;

	s.handle = js_alloc(sizeof(DbHandle), false);
	*s.handle = idx->hndlBits;

	dbInstallIndexes(idxHndl);

	releaseHandle(idxHndl);
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


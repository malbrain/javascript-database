#include "js.h"
#include "js_dbindex.h"
#include "database/db_api.h"
#include "database/db_error.h"

static bool debug = false;

//	types of handles/arenas

extern char *hndlNames[];

//	closeHandle

value_t js_closeHandle(uint32_t args, environment_t *env) {
	value_t hndl, s;

	s.bits = vt_status;

	hndl = eval_arg (&args, env);

	if (vt_handle != hndl.type) {
		fprintf(stderr, "Error: closeHandle => expecting Handle => %s\n", strtype(hndl.type));
		return s.status = ERROR_script_internal, s;
	}

	switch (hndl.subType) {
	case Hndl_database:
	case Hndl_docStore:
	case Hndl_btree1Index:
	case Hndl_artIndex:
	case Hndl_colIndex:
	case Hndl_iterator:
	case Hndl_cursor:
		break;

	default:
		fprintf(stderr, "Error: closeHandle => database/collection/index handle expected => %s\n", hndlNames[hndl.subType]);
		return s.status = ERROR_script_internal, s;
	}

	s.status = DB_OK;
	return s;
}

//	dropArena(Handle, dropChildDefinitions)

value_t js_dropArena(uint32_t args, environment_t *env) {
	value_t arena, s, v;
	bool dropDefs;

	s.bits = vt_status;

	arena = eval_arg (&args, env);

	if (vt_handle != arena.type) {
		fprintf(stderr, "Error: dropArena => expecting Handle => %s\n", strtype(arena.type));
		return s.status = ERROR_script_internal, s;
	}

	v = eval_arg (&args, env);
	dropDefs = conv2Bool(v, true).boolean;

	switch (arena.subType) {
	case Hndl_database:
	case Hndl_docStore:
	case Hndl_artIndex:
	case Hndl_btree1Index:
		break;

	default:
		fprintf(stderr, "Error: dropArena => database/collection/index handle expected => %s\n", hndlNames[arena.subType]);
		return s.status = ERROR_script_internal, s;
	}

	s.status = dropArena((DbHandle *)arena.handle, dropDefs);
	return s;
}

value_t js_openDatabase(uint32_t args, environment_t *env) {
	Params params[MaxParam];
	value_t v, dbname, db;
	DbHandle idx[1];
	value_t s;

	memset (params, 0, sizeof(params));

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : openDatabase\n");

	dbname = eval_arg (&args, env);

	if (vt_string != dbname.type) {
		fprintf(stderr, "Error: openDatabase => expecting Name:string => %s\n", strtype(dbname.type));
		return s.status = ERROR_script_internal, s;
	}

	v = eval_arg (&args, env);
	params[OnDisk].boolVal = conv2Bool(v, true).boolean;

	if ((s.status = openDatabase(idx, dbname.str, dbname.aux, params)))
		return s;

	s.bits = vt_handle;
	s.subType = Hndl_database;
	*s.handle = idx->hndlBits;

	abandonValue(dbname);
	return s;
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

	s.status = commitTxn((DbHandle*)db.handle, txnId.txnBits);
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

	s.status = rollbackTxn((DbHandle *)db.handle, txnId.txnBits);
	return s;
}

//  createIndex(docStore, keys, idxname, type, size, onDisk, unique, sparse, partial) 

value_t js_createIndex(uint32_t args, environment_t *env) {
	value_t v, name, docStore, keys, type;
	uint8_t *spec = NULL, *partial = NULL;
	Params params[MaxParam];
	HandleType idxType;
	DbHandle idx[1];
	uint64_t addr;
	uint32_t size;
	value_t s;
	int i;

	memset (params, 0, sizeof(params));

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : CreateIndex\n");

	docStore = eval_arg (&args, env);

	if (vt_handle != docStore.type || Hndl_docStore != docStore.subType) {
		fprintf(stderr, "Error: createIndex => expecting docStore:handle => %s\n", strtype(docStore.type));
		return s.status = ERROR_script_internal, s;
	}

	keys = eval_arg (&args, env);

	if (vt_object != keys.type) {
		fprintf(stderr, "Error: createIndex => expecting Object:keys => %s\n", strtype(keys.type));
		return s.status = ERROR_script_internal, s;
	}

	size = 0;

	for (i = 0; i < vec_count(keys.oval->pairs); i++)
		size += keys.oval->pairs[i].name.aux + sizeof(IndexKey);

	spec = js_alloc(size, false);
	params[IdxKeySpecLen].intVal = size;
	params[IdxKeySpec].obj = spec;

	compileKeys(spec, size, keys.oval);

	name = eval_arg (&args, env);

	if (vt_string != name.type) {
		fprintf(stderr, "Error: createIndex => expecting Name:string => %s\n", strtype(name.type));
		return s.status = ERROR_script_internal, s;
	}

	type = eval_arg (&args, env);

	if (vt_string != type.type) {
		fprintf(stderr, "Error: createIndex => expecting Type:string => %s\n", strtype(type.type));
		return s.status = ERROR_script_internal, s;
	}

	if (!strncmp(type.str, "artree", type.aux))
		idxType = Hndl_artIndex;
	else if (!strncmp(type.str, "btree1", type.aux))
		idxType = Hndl_btree1Index;
	else {
		fprintf(stderr, "Error: createIndex => expecting artree or btree1 => %.*s\n", type.aux, type.str);
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

		compileKeys(partial, size, keys.oval);

		marshal_doc(v, partial, size);
		params[IdxKeyPartial].obj = partial;
	}

	abandonValue(v);

	if ((s.status = createIndex(idx, (DbHandle *)docStore.handle, idxType, name.str, name.aux, params)))
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
	value_t start, index, rev, limits, txn;
	Params params[MaxParam];
	DbHandle idx[1];
	ObjId txnId;
	value_t s;
	void *obj;

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

	if ((s.status = createCursor(idx, (DbHandle *)index.handle, txnId, params)))
		return s;

	//	TODO:  set min & max keys

	s.bits = vt_handle;
	s.subType = Hndl_cursor;
	*s.handle = idx->hndlBits;
	return s;
}

// nextKey(cursor)

value_t js_nextKey(uint32_t args, environment_t *env) {
	value_t v, slot;
	value_t s;
	int len;

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : NextKey\n");

	v = eval_arg (&args, env);

	if (vt_handle != v.type || Hndl_cursor != v.subType) {
		fprintf(stderr, "Error: nextKey => expecting cursor:Handle => %s\n", strtype(v.type));
		return s.status = ERROR_script_internal, s;
	}

	if ((s.status = moveCursor((DbHandle *)v.handle, OpNext)))
		return s;

	slot.bits = vt_string;

	if ((s.status = keyAtCursor((DbHandle *)v.handle, &slot.str, &len)))
		return s;

	slot.aux = len;
	return slot;
}

// prevKey(cursor)

value_t js_prevKey(uint32_t args, environment_t *env) {
	value_t v, slot;
	value_t s;
	int len;

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : PrevKey\n");

	v = eval_arg (&args, env);

	if (vt_handle != v.type || Hndl_cursor != v.subType) {
		fprintf(stderr, "Error: nextKey => expecting cursor:Handle => %s\n", strtype(v.type));
		return s.status = ERROR_script_internal, s;
	}

	if ((s.status = moveCursor((DbHandle *)v.handle, OpPrev)))
		return s;

	slot.bits = vt_string;

	if ((s.status = keyAtCursor((DbHandle *)v.handle, &slot.str, &len)))
		return s;

	slot.aux = len;
	return slot;
}

//	js_getKey(cursor);

value_t js_getKey(uint32_t args, environment_t *env) {
	value_t v, slot;
	value_t s;
	int len;

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : GetKey\n");

	v = eval_arg (&args, env);

	if (vt_handle != v.type || Hndl_cursor != v.subType) {
		fprintf(stderr, "Error: getKey => expecting cursor:Handle => %s\n", strtype(v.type));
		return s.status = ERROR_script_internal, s;
	}

	slot.bits = vt_string;

	if ((s.status = keyAtCursor((DbHandle *)v.handle, &slot.str, &len)))
		return s;

	slot.aux = len;
	return slot;
}

//	openDocStore(database, name, size, onDisk)

value_t js_openDocStore(uint32_t args, environment_t *env) {
	value_t v, name, docStore, database;
	Params params[MaxParam];
	DbHandle idx[1];
	uint64_t size;
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

	if (vt_string != name.type) {
		fprintf(stderr, "Error: openDocStore => expecting Name => %s\n", strtype(name.type));
		return s.status = ERROR_script_internal, s;
	}

	v = eval_arg(&args, env);
	params[InitSize].intVal = conv2Int(v, true).nval;

	v = eval_arg(&args, env);
	params[OnDisk].boolVal = conv2Bool(v, true).boolean;

	if ((s.status = openDocStore(idx, (DbHandle *)database.handle, name.str, name.aux, params)))
		return s;

	abandonValue(name);

	docStore.bits = vt_handle;
	docStore.subType = Hndl_docStore;
	*docStore.handle = idx->hndlBits;
	return docStore;
}

//	js_deleteDoc(docStore, verId, txnId)

value_t js_deleteDoc(uint32_t args, environment_t *env) {
	value_t v, docStore, s, txnId;
	void *obj;

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : DeleteDoc\n");

	docStore = eval_arg (&args, env);

	if (vt_handle != docStore.type || Hndl_docStore != docStore.subType) {
		fprintf(stderr, "Error: deleteDoc => expecting Handle => %s\n", strtype(docStore.type));
		return s.status = ERROR_script_internal, s;
	}

	v = eval_arg (&args, env);

	if (vt_docId != v.type) {
		fprintf(stderr, "Error: deleteDoc => expecting DocId => %s\n", strtype(v.type));
		return s.status = ERROR_script_internal, s;
	}

	txnId = eval_arg (&args, env);

	if (vt_txnId != txnId.type) {
		fprintf(stderr, "Error: beginTxn => expecting txnId => %s\n", strtype(txnId.type));
		return s.status = ERROR_script_internal, s;
	}

	s.status = deleteDoc((DbHandle *)docStore.handle, v.docBits, txnId.txnBits);
	return s;
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

	if ((s.status = createIterator(idx, (DbHandle *)docStore.handle, txnId.txnBits)))
		return s;

	iter.bits = vt_handle;
	iter.subType = Hndl_iterator;
	*iter.handle = idx->hndlBits;
	return iter;
}

//	js_seekDoc(iterator, docId)

value_t js_seekDoc(uint32_t args, environment_t *env) {
	value_t v, iter, s;

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : SeekDoc\n");

	iter = eval_arg (&args, env);

	if (vt_handle != iter.type || Hndl_iterator != iter.subType) {
		fprintf(stderr, "Error: seekDoc => expecting iter:Handle => %s\n", strtype(iter.type));
		return s.status = ERROR_script_internal, s;
	}

	v = eval_arg (&args, env);

	if (vt_docId != v.type) {
		fprintf(stderr, "Error: seekDoc => expecting DocId:Symbol => %s\n", strtype(v.type));
		return s.status = ERROR_script_internal, s;
	}

	s.status = iteratorSeek((DbHandle *)iter.handle, v.docBits);
	return s;
}

// nextDoc(iterator)

value_t js_nextDoc(uint32_t args, environment_t *env) {
	value_t slot, iter, s, b, v;
	Doc *doc;

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : NextDoc\n");

	iter = eval_arg (&args, env);

	if (vt_handle != iter.type || Hndl_iterator != iter.subType) {
		fprintf(stderr, "Error: nextDoc => expecting iter:Handle => %s\n", strtype(iter.type));
		return s.status = ERROR_script_internal, s;
	}

	slot = eval_arg (&args, env);

	if (slot.type != vt_lval) {
		fprintf(stderr, "Error: nextDoc => expecting lval => %s\n", strtype(slot.type));
		return s.status = ERROR_script_internal, s;
	}

	b.bits = vt_bool;

	if ((doc = iteratorNext((DbHandle *)iter.handle)))
		b.boolean = true;
	else
		b.boolean = false;

	v.bits = vt_document;
	v.document = (document_t *)(doc + 1);
	replaceValue(slot, v);
	return b;
}

// prevDoc(iterator)

value_t js_prevDoc(uint32_t args, environment_t *env) {
	value_t slot, iter, s;
	Doc *doc;

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : NextDoc\n");

	iter = eval_arg (&args, env);

	if (vt_handle != iter.type || Hndl_iterator != iter.subType) {
		fprintf(stderr, "Error: nextDoc => expecting iter:Handle => %s\n", strtype(iter.type));
		return s.status = ERROR_script_internal, s;
	}

	if (!(doc = iteratorPrev((DbHandle *)iter.handle)))
		return s.status = DB_ITER_eof, s;

	slot.bits = vt_document;
	slot.document = (document_t *)(doc + 1);
	return s;
}

//  updateDoc (docStore, docArray, &docIdArray, &docCount)

value_t js_updateDoc(uint32_t args, environment_t *env) {
	value_t s;

	s.bits = vt_status;
	return s.status = OK, s;
}

void js_deleteHandle(value_t val) {
	deleteHandle((DbHandle *)val.handle);
}

#include "jsdb.h"
#include "jsdb_db.h"

static bool debug = false;
extern Status deleteDoc(DbMap *map, uint64_t docId, uint64_t txnBits);
extern value_t createIndex(DbMap *map, value_t type, value_t keys, value_t name, uint32_t size, bool unique, bool sparse, value_t partial);
extern value_t makeCursor(value_t val, DbMap *index, bool rev, value_t start, value_t limits);
extern value_t createDatabase (value_t dbname, bool onDisk);
extern value_t createDocStore(DbMap *map, value_t name, uint64_t size, bool onDisk, bool *created);
extern value_t createIterator(value_t docStore, DbMap *map, bool atEnd);

extern uint64_t txnBegin (DbMap *db);
extern Status txnRollback (DbMap *db, uint64_t txnBits);
extern Status txnCommit (DbMap *db, uint64_t txnBits);

void *lockHandle(value_t val) {
	Handle *hndl = val.handle;

	if (!hndl->object)
		return NULL;

	atomicAdd64(hndl->entryCnt, 1);

	if (!hndl->object)
		atomicAdd64(hndl->entryCnt, -1ULL);

	return hndl->object;
}

void unlockHandle(value_t val) {
	Handle *hndl = val.handle;

	atomicAdd64(hndl->entryCnt, -1ULL);
}

//	closeHandle

void jsdb_closeHandle(value_t hndl) {
	switch (hndl.aux) {
	case Hndl_newarena:
	case Hndl_database:
	case Hndl_docStore:
	case Hndl_btreeIndex:
	case Hndl_artIndex:
	case Hndl_colIndex:
	case Hndl_iterator:
	case Hndl_btreeCursor:
	case Hndl_artCursor:
	case Hndl_docVersion:
		break;
	}
}

//	openDatabase(dbname)

value_t jsdb_openDatabase(uint32_t args, environment_t *env) {
	value_t v, dbname, onDisk, db;
	value_t s;

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : openDatabase\n");

	dbname = eval_arg (&args, env);

	if (vt_string != dbname.type) {
		fprintf(stderr, "Error: openDatabase => expecting Name:string => %s\n", strtype(dbname.type));
		return s.status = ERROR_script_internal, s;
	}

	v = eval_arg (&args, env);
	onDisk = conv2Bool(v, true);

	db = createDatabase(dbname, onDisk.boolean);
	abandonValue(dbname);

	return db;
}

//	beginTxn(db)

value_t jsdb_beginTxn(uint32_t args, environment_t *env) {
	value_t db, txnId;
	value_t s;
	void *obj;

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : beginTxn\n");

	db = eval_arg (&args, env);

	if (vt_handle != db.type || Hndl_database != db.subType) {
		fprintf(stderr, "Error: beginTxn => expecting Db:handle => %s\n", strtype(db.type));
		return s.status = ERROR_script_internal, s;
	}

	txnId.bits = vt_txnId;

	if ((obj = lockHandle(db))) {
	  if((txnId.txnBits = txnBegin(obj)))
		return txnId;
	  else
		s.status = ERROR_outofmemory;
	} else
		s.status = ERROR_handleclosed;

	return s;
}

//	commitTxn(db, txnId)

value_t jsdb_commitTxn(uint32_t args, environment_t *env) {
	value_t db, txnId;
	value_t s;
	void *obj;

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

	if ((obj = lockHandle(db)))
		s.status = txnCommit(obj, txnId.txnBits);
	else
		s.status = ERROR_handleclosed;

	return s;
}

//	rollbackTxn(db, txnId)

value_t jsdb_rollbackTxn(uint32_t args, environment_t *env) {
	value_t db, txnId;
	value_t s;
	void *obj;

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

	if ((obj = lockHandle(db)))
		s.status = txnRollback(obj, txnId.txnBits);
	else
		s.status = ERROR_handleclosed;

	return s;
}

//  createIndex(docStore, keys, idxname, type, size, onDisk, unique, partial) 

value_t jsdb_createIndex(uint32_t args, environment_t *env) {
	value_t v, name, docStore, unique, partial, keys, type, sparse;
	uint64_t size;
	value_t s;
	void *obj;

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

	v = eval_arg (&args, env);
	size = conv2Int(v, true).nval;

	v = eval_arg (&args, env);
	unique = conv2Bool(v, true);

	v = eval_arg (&args, env);
	sparse = conv2Bool(v, true);

	partial = eval_arg (&args, env);

	if ((obj = lockHandle(docStore)))
		s = createIndex(obj, type, keys, name, size, unique.boolean, sparse.boolean, partial);
	else
		s.status = ERROR_handleclosed;

	abandonValue(type);
	abandonValue(keys);
	abandonValue(name);
	abandonValue(partial);
	return s;
}

//	jdsd_drop(Handle)

value_t jsdb_drop(uint32_t args, environment_t *env) {
	value_t v;
	value_t s;

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : drop\n");

	v = eval_arg (&args, env);

	if (vt_handle != v.type) {
		fprintf(stderr, "Error: drop => expecting Handle => %s\n", strtype(v.type));
		return s.status = ERROR_script_internal, s;
	}

	switch (v.subType) {
	case Hndl_database:
	case Hndl_docStore:
	case Hndl_btreeIndex:
	case Hndl_artIndex:
	case Hndl_colIndex:
	case Hndl_iterator:
	case Hndl_btreeCursor:
	case Hndl_artCursor:
	case Hndl_docVersion:
	default:
		fprintf(stderr, "Error: drop => unsupported handle type => %d\n", v.subType);
		return s.status = ERROR_script_internal, s;
	}

	return s.status = OK, s;
}

//  createCursor(index, rev, start, limit)

value_t jsdb_createCursor(uint32_t args, environment_t *env) {
	value_t start, index, rev, limits;
	value_t s;
	void *obj;

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : createCursor\n");

	index = eval_arg (&args, env);

	if (vt_handle != index.type || (Hndl_btreeIndex != index.subType && Hndl_artIndex != index.subType)) {
		fprintf(stderr, "Error: createCursor => expecting index:handle => %s\n", strtype(index.type));
		return s.status = ERROR_script_internal, s;
	}

	rev = eval_arg (&args, env);

	if (vt_bool != rev.type) {
		fprintf(stderr, "Error: createCursor => expecting reverse:bool => %s\n", strtype(rev.type));
		return s.status = ERROR_script_internal, s;
	}

	start = eval_arg (&args, env);

	if (vt_array != start.type && vt_undef != start.type) {
		fprintf(stderr, "Error: createCursor => expecting start:Object => %s\n", strtype(start.type));
		return s.status = ERROR_script_internal, s;
	}

	limits = eval_arg (&args, env);

	if (vt_array != limits.type && vt_undef != limits.type) {
		fprintf(stderr, "Error: createCursor => expecting limits:Object => %s\n", strtype(limits.type));
		return s.status = ERROR_script_internal, s;
	}

	if ((obj = lockHandle(index)))
		s = makeCursor(index, obj, rev.boolean, start, limits);
	else
		s.status = ERROR_handleclosed;

	return s;
}

// nextKey(cursor)

value_t jsdb_nextKey(uint32_t args, environment_t *env) {
	DbCursor *cursor;
	value_t v, slot;
	value_t s;
	void *obj;

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : NextKey\n");

	v = eval_arg (&args, env);

	if (vt_handle != v.type || (Hndl_btreeCursor != v.subType && Hndl_artCursor != v.subType)) {
		fprintf(stderr, "Error: nextKey => expecting cursor:Handle => %s\n", strtype(v.type));
		return s.status = ERROR_script_internal, s;
	}

	cursor = v.handle;

	if ((obj = lockHandle(cursor->hndl)))
		slot = cursorNext(cursor, obj);
	else
		return s.status = ERROR_handleclosed, s;

	return slot;
}

// prevKey(cursor)

value_t jsdb_prevKey(uint32_t args, environment_t *env) {
	DbCursor *cursor;
	value_t v, slot;
	value_t s;
	void *obj;

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : PrevKey\n");

	v = eval_arg (&args, env);

	if (vt_handle != v.type || (Hndl_btreeCursor != v.subType && Hndl_artCursor != v.subType)) {
		fprintf(stderr, "Error: nextKey => expecting cursor:Handle => %s\n", strtype(v.type));
		return s.status = ERROR_script_internal, s;
	}

	cursor = v.handle;

	if ((obj = lockHandle(cursor->hndl)))
		slot = cursorPrev(cursor, obj);
	else
		return s.status = ERROR_handleclosed, s;

	return slot;
}

//	jsdb_getKey(cursor);

value_t jsdb_getKey(uint32_t args, environment_t *env) {
	value_t s, cursor;
	void *obj;

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : GetKey\n");

	cursor = eval_arg (&args, env);

	if (vt_handle != cursor.type) {
		fprintf(stderr, "Error: getKey => expecting cursor:Handle => %s\n", strtype(cursor.type));
		return s.status = ERROR_script_internal, s;
	}

	if ((obj = lockHandle(cursor))) {
	  switch (cursor.subType) {
	  case Hndl_artCursor:
		s = artCursorKey(obj);
		break;
	  case Hndl_btreeCursor:
		s = btreeCursorKey(obj);
		break;
	  default:
		fprintf(stderr, "Error: getKey => unsupported handle type => %d\n", cursor.subType);
		return s.status = ERROR_script_internal, s;
	  }
	} else
		s.status = ERROR_handleclosed;

	return s;
}

//	createDocStore(database, name, size, onDisk, created)

value_t jsdb_createDocStore(uint32_t args, environment_t *env) {
	value_t v, name, slot, onDisk, docStore, database;
	uint64_t size;
	value_t s;
	void *obj;

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : CreateDocStore\n");

	database = eval_arg (&args, env);

	if (vt_handle != database.type) {
		fprintf(stderr, "Error: createDocStore => expecting Database handle => %s\n", strtype(database.type));
		return s.status = ERROR_script_internal, s;
	}

	name = eval_arg (&args, env);

	if (vt_string != name.type) {
		fprintf(stderr, "Error: createDocStore => expecting Name => %s\n", strtype(name.type));
		return s.status = ERROR_script_internal, s;
	}

	v = eval_arg(&args, env);
	size = conv2Int(v, true).nval;

	v = eval_arg(&args, env);
	onDisk = conv2Bool(v, true);

	slot = eval_arg (&args, env);

	if (vt_lval != slot.type) {
		fprintf(stderr, "Error: createDocStore => expecting created:Symbol => %s\n", strtype(slot.type));
		return s.status = ERROR_script_internal, s;
	}

	v.bits = vt_bool;

	if ((obj = lockHandle(database)))
		docStore = createDocStore(obj, name, size, onDisk.boolean, &v.boolean);
	else
		return s.status = ERROR_handleclosed, s;

	replaceValue(slot, v);

	abandonValue(name);
	return docStore;
}

//	jsdb_deleteDoc(docStore, verId, txnId)

value_t jsdb_deleteDoc(uint32_t args, environment_t *env) {
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

	if ((obj = lockHandle(docStore)))
		s.status = deleteDoc(obj, v.docBits, txnId.txnBits);
	else
		s.status = ERROR_handleclosed;

	return s;
}

//  jsdb_createIterator(docStore)

value_t jsdb_createIterator(uint32_t args, environment_t *env) {
	value_t iter, docStore;
	value_t s;
	void *obj;

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : CreateIterator\n");

	docStore = eval_arg (&args, env);

	if (vt_handle != docStore.type || Hndl_docStore != docStore.subType) {
		fprintf(stderr, "Error: createIterator => expecting docStore:Handle => %s\n", strtype(docStore.type));
		return s.status = ERROR_script_internal, s;
	}

	if ((obj = lockHandle(docStore)))
		iter = createIterator(docStore, obj, true);
	else
		return s.status = ERROR_handleclosed, s;

	return iter;
}

//	jsdb_seekDoc(iterator, docId)

value_t jsdb_seekDoc(uint32_t args, environment_t *env) {
	value_t v, iter, s;
	Iterator *it;
	void *obj;

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : SeekDoc\n");

	iter = eval_arg (&args, env);

	if (vt_handle != iter.type || Hndl_iterator != iter.subType) {
		fprintf(stderr, "Error: seekDoc => expecting iter:Handle => %s\n", strtype(iter.type));
		return s.status = ERROR_script_internal, s;
	}

	it = iter.handle;

	v = eval_arg (&args, env);

	if (vt_docId != v.type) {
		fprintf(stderr, "Error: seekDoc => expecting DocId:Symbol => %s\n", strtype(v.type));
		return s.status = ERROR_script_internal, s;
	}

	if ((obj = lockHandle(it->docStore)))
		v = iteratorSeek(it, obj, v.docBits);
	else
		return s.status = ERROR_handleclosed, s;

	return v;
}

// nextDoc(iterator)

value_t jsdb_nextDoc(uint32_t args, environment_t *env) {
	value_t slot, iter, s;
	Iterator *it;
	void *obj;

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : NextDoc\n");

	iter = eval_arg (&args, env);

	if (vt_handle != iter.type || Hndl_iterator != iter.subType) {
		fprintf(stderr, "Error: nextDoc => expecting iter:Handle => %s\n", strtype(iter.type));
		return s.status = ERROR_script_internal, s;
	}

	it = iter.handle;

	if ((obj = lockHandle(it->docStore)))
		slot = iteratorNext(it, obj);
	else
		return s.status = ERROR_handleclosed, s;

	return slot;
}

// prevDoc(iterator)

value_t jsdb_prevDoc(uint32_t args, environment_t *env) {
	value_t slot, iter, s;
	Iterator *it;
	void *obj;

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : PrevDoc\n");

	iter = eval_arg (&args, env);

	if (vt_handle != iter.type || Hndl_iterator != iter.subType) {
		fprintf(stderr, "Error: prevDoc => expecting iter:Handle => %s\n", strtype(iter.type));
		return s.status = ERROR_script_internal, s;
	}

	it = iter.handle;

	if ((obj = lockHandle(it->docStore)))
		slot = iteratorPrev(it, obj);
	else
		return s.status = ERROR_handleclosed, s;

	return slot;
}

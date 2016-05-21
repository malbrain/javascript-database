#include "jsdb.h"
#include "jsdb_db.h"

static bool debug = false;
extern Status deleteDoc(object_t *docStore, DocId docId, DocId txnId);
extern value_t createIndex(DbMap *docStore, value_t type, value_t keys, value_t name, uint32_t size, bool unique, bool sparse, value_t partial);
extern value_t makeCursor( value_t index, bool rev, value_t start, value_t limits);
extern value_t createDatabase (value_t dbname, bool onDisk);
extern value_t createDocStore(value_t database, value_t name, uint64_t size, bool onDisk);
extern value_t createIterator(DbMap *map, bool atEnd);
extern uint64_t txnBegin (DbMap *db);
extern Status txnRollback (DbMap *db, DocId txnId);
extern Status txnCommit (DbMap *db, DocId txnId);

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
	value_t v, db, txnId;
	value_t s;

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : beginTxn\n");

	db = eval_arg (&args, env);

	if (vt_object != db.type) {
		fprintf(stderr, "Error: beginTxn => expecting Db:object => %s\n", strtype(db.type));
		return s.status = ERROR_script_internal, s;
	}

	txnId.bits = vt_docId;

	if((txnId.docId.bits = txnBegin(db.oval->pairs->value.hndl)))
		return txnId;

	s.status = ERROR_outofmemory;
	return s;
}

//	commitTxn(db)

value_t jsdb_commitTxn(uint32_t args, environment_t *env) {
	value_t v, db, txnId;
	value_t s;

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : commitTxn\n");

	db = eval_arg (&args, env);

	if (vt_object != db.type) {
		fprintf(stderr, "Error: commitTxn => expecting Db:object => %s\n", strtype(db.type));
		return s.status = ERROR_script_internal, s;
	}

	txnId = eval_arg (&args, env);

	if (vt_docId != txnId.type) {
		fprintf(stderr, "Error: beginTxn => expecting Db:object => %s\n", strtype(txnId.type));
		return s.status = ERROR_script_internal, s;
	}

	s.status = txnCommit(db.oval->pairs->value.hndl, txnId.docId);
	return s;
}

//	rollbackTxn(db)

value_t jsdb_rollbackTxn(uint32_t args, environment_t *env) {
	value_t v, db, txnId;
	value_t s;

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : rollbackTxn\n");

	db = eval_arg (&args, env);

	if (vt_object != db.type) {
		fprintf(stderr, "Error: rollbackTxn => expecting Db:object => %s\n", strtype(db.type));
		return s.status = ERROR_script_internal, s;
	}

	txnId = eval_arg (&args, env);

	if (vt_docId != txnId.type) {
		fprintf(stderr, "Error: beginTxn => expecting Db:object => %s\n", strtype(txnId.type));
		return s.status = ERROR_script_internal, s;
	}

	s.status = txnRollback(db.oval->pairs->value.hndl, txnId.docId);
	return s;
}

//  createIndex(docStore, keys, idxname, type, size, onDisk, unique, partial) 

value_t jsdb_createIndex(uint32_t args, environment_t *env) {
	value_t v, name, docStore, unique, partial, keys, type, sparse;
	uint64_t size;
	value_t s;

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : CreateIndex\n");

	docStore = eval_arg (&args, env);

	if (vt_object != docStore.type || Hndl_docStore != docStore.oval->pairs[0].value.aux) {
		fprintf(stderr, "Error: createIndex => expecting Handle:docStore => %s\n", strtype(docStore.type));
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

	v = createIndex(docStore.oval->pairs[0].value.hndl, type, keys, name, size, unique.boolean, sparse.boolean, partial);

	abandonValue(type);
	abandonValue(keys);
	abandonValue(name);
	abandonValue(partial);
	return v;
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

	if (Hndl_artIndex != v.aux || Hndl_btreeIndex != v.aux || Hndl_docStore != v.aux) {
		fprintf(stderr, "Error: drop => unsupported handle type => %s\n", strtype(v.type));
		return s.status = ERROR_script_internal, s;
	}

	((DbMap *)v.hndl)->arena->drop = 1;
	return s.status = OK, s;
}

//  createCursor(index, rev, start, limit)

value_t jsdb_createCursor(uint32_t args, environment_t *env) {
	value_t v, start, result, index, rev, limits;
	value_t s;

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : createCursor\n");

	index = eval_arg (&args, env);

	if (vt_handle != index.type) {
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

	return makeCursor(index, rev.boolean, start, limits);
}

// nextKey(cursor, docStore, document)

value_t jsdb_nextKey(uint32_t args, environment_t *env) {
	value_t v, cursor, slot, docStore;
	DocId docId;
	value_t s;

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : NextKey\n");

	cursor = eval_arg (&args, env);

	if (vt_handle != cursor.type) {
		fprintf(stderr, "Error: nextKey => expecting cursor:Handle => %s\n", strtype(cursor.type));
		return s.status = ERROR_script_internal, s;
	}

	docStore = eval_arg (&args, env);

	if (vt_handle != docStore.type) {
		fprintf(stderr, "Error: nextKey => expecting docstore:Handle => %s\n", strtype(docStore.type));
		return s.status = ERROR_script_internal, s;
	}

	switch (cursor.aux) {
	case Hndl_artCursor:
		if ((artNextKey(cursor.hndl)))
			docId.bits = artDocId(cursor.hndl);
		else
			docId.bits = 0;
		break;
	case Hndl_btreeCursor:
		if ((btreeNextKey(cursor.hndl)))
			docId.bits = btreeDocId(cursor.hndl);
		else
			docId.bits = 0;
		break;
	}

	slot = eval_arg (&args, env);

	if (vt_lval != slot.type) {
		fprintf(stderr, "Error: nextKey => expecting document:Symbol => %s\n", strtype(slot.type));
		return s.status = ERROR_script_internal, s;
	}

	if (docId.bits) {
		slot.lval->bits = vt_document;
		slot.lval->document = findDoc(docStore.hndl, docId);
	} else
		slot.lval->bits = vt_undef;

	v.bits = vt_docId;
	v.docId.bits = docId.bits;
	return v;
}

// prevKey(cursor)

value_t jsdb_prevKey(uint32_t args, environment_t *env) {
	value_t v, cursor;
	DocId docId;
	value_t s;

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : PrevKey\n");

	cursor = eval_arg (&args, env);

	if (vt_handle != cursor.type) {
		fprintf(stderr, "Error: prevKey => expecting cursor:Handle => %s\n", strtype(cursor.type));
		return s.status = ERROR_script_internal, s;
	}

	switch (cursor.aux) {
	case Hndl_artCursor:
		if (artPrevKey(cursor.hndl))
			docId.bits = artDocId(cursor.hndl);
		else
			docId.bits = 0;
		break;
	case Hndl_btreeCursor:
		if (btreePrevKey(cursor.hndl))
			docId.bits = artDocId(cursor.hndl);
		else
			docId.bits = 0;
		break;
	}

	v.bits = vt_docId;
	v.docId = docId;
	return v;
}

//	jsdb_getKey(handle);

value_t jsdb_getKey(uint32_t args, environment_t *env) {
	value_t v, slot, cursor;
	value_t s;

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : GetKey\n");

	cursor = eval_arg (&args, env);

	if (vt_handle != cursor.type) {
		fprintf(stderr, "Error: getKey => expecting cursor:Handle => %s\n", strtype(cursor.type));
		return s.status = ERROR_script_internal, s;
	}

	switch (cursor.aux) {
	case Hndl_artCursor:
		v = artCursorKey(cursor.hndl);
		break;
	case Hndl_btreeCursor:
		v = btreeCursorKey(cursor.hndl);
		break;
	default:
		fprintf(stderr, "Error: getKey => expecting cursor:Handle => %s\n", strtype(cursor.type));
		return s.status = ERROR_script_internal, s;
	}

	return v;
}

//	createDocStore(database, name, size, onDisk, created)

value_t jsdb_createDocStore(uint32_t args, environment_t *env) {
	value_t v, name, slot, onDisk, created, docStore, database;
	uint64_t size;
	value_t s;

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

	docStore = createDocStore(database, name, size, onDisk.boolean);

	slot = eval_arg (&args, env);

	if (vt_lval != slot.type) {
		fprintf(stderr, "Error: createDocStore => expecting created:Symbol => %s\n", strtype(slot.type));
		return s.status = ERROR_script_internal, s;
	}

	v.bits = vt_bool;
	v.boolean = ((DbMap *)docStore.aval->values[0].hndl)->created;
	replaceValue(slot, v);

	abandonValue(name);
	return docStore;
}

//  findDoc (docStore, docId)

value_t jsdb_findDoc(uint32_t args, environment_t *env) {
	value_t v, docStore;
	DocId docId;
	value_t s;

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : FindDoc\n");

	docStore = eval_arg (&args, env);

	if (vt_handle != docStore.type) {
		fprintf(stderr, "Error: findDoc => expecting store:Handle => %s\n", strtype(docStore.type));
		return s.status = ERROR_script_internal, s;
	}

	v = eval_arg (&args, env);
	docId.bits = v.docId.bits;

	if (vt_docId != v.type) {
		fprintf(stderr, "Error: findDoc => expecting id:docid => %s\n", strtype(v.type));
		return s.status = ERROR_script_internal, s;
	}

	v.bits = vt_document;
	v.document = findDoc(docStore.hndl, docId);

	return v;
}

//	jsdb_deleteDoc(docStore, docId, txnId)

value_t jsdb_deleteDoc(uint32_t args, environment_t *env) {
	value_t v, docStore, s, txnId;
	Status stat;

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : DeleteDoc\n");

	docStore = eval_arg (&args, env);

	if (vt_handle != docStore.type) {
		fprintf(stderr, "Error: deleteDoc => expecting Handle => %s\n", strtype(docStore.type));
		return s.status = ERROR_script_internal, s;
	}

	v = eval_arg (&args, env);

	if (vt_docId != v.type) {
		fprintf(stderr, "Error: deleteDoc => expecting Number => %s\n", strtype(v.type));
		return s.status = ERROR_script_internal, s;
	}

	txnId = eval_arg (&args, env);

	if (vt_docId != txnId.type) {
		fprintf(stderr, "Error: beginTxn => expecting Db:object => %s\n", strtype(txnId.type));
		return s.status = ERROR_script_internal, s;
	}

	s.status = deleteDoc(docStore.oval, v.docId, txnId.docId);
	return s;
}

//  jsdb_createIterator(docStore)

value_t jsdb_createIterator(uint32_t args, environment_t *env) {
	value_t v, docStore, iter;
	value_t s;

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : CreateIterator\n");

	docStore = eval_arg (&args, env);

	if (vt_handle != docStore.type) {
		fprintf(stderr, "Error: createIterator => expecting store:Handle => %s\n", strtype(docStore.type));
		return s.status = ERROR_script_internal, s;
	}

	return createIterator(docStore.hndl, true);
}

//	jsdb_seekDoc(iterator, docId)

value_t jsdb_seekDoc(uint32_t args, environment_t *env) {
	value_t v, iter, s;

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : SeekDoc\n");

	iter = eval_arg (&args, env);

	if (vt_handle != iter.type || Hndl_iterator != iter.aux) {
		fprintf(stderr, "Error: seekDoc => expecting iter:Handle => %s\n", strtype(iter.type));
		return s.status = ERROR_script_internal, s;
	}

	v = eval_arg (&args, env);

	if (vt_docId != v.type) {
		fprintf(stderr, "Error: seekDoc => expecting DocId:Symbol => %s\n", strtype(v.type));
		return s.status = ERROR_script_internal, s;
	}

	v.document = iteratorSeek(iter.hndl, v.docId);

	if (v.document)
 		v.bits = vt_document;
	else
		v.bits = vt_null;

	return v;
}

// nextDoc(iterator, &document)

value_t jsdb_nextDoc(uint32_t args, environment_t *env) {
	value_t v, slot, iter, s;
	DocId docId;

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : NextDoc\n");

	iter = eval_arg (&args, env);

	if (vt_handle != iter.type) {
		fprintf(stderr, "Error: nextDoc => expecting iter:Handle => %s\n", strtype(iter.type));
		return s.status = ERROR_script_internal, s;
	}

	slot = eval_arg (&args, env);

	if (vt_lval != slot.type) {
		fprintf(stderr, "Error: nextDoc => expecting document:Symbol => %s\n", strtype(slot.type));
		return s.status = ERROR_script_internal, s;
	}

	slot.lval->bits = vt_document;
	slot.lval->document = iteratorNext(iter.hndl, &docId);

	v.bits = vt_docId;
	v.docId.bits = docId.bits;
	return v;
}

// prevDoc(iterator, &docId, &document)

value_t jsdb_prevDoc(uint32_t args, environment_t *env) {
	value_t v, slot, iter, s;
	DocId docId;

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : PrevDoc\n");

	iter = eval_arg (&args, env);

	if (vt_handle != iter.type || Hndl_iterator != iter.aux) {
		fprintf(stderr, "Error: prevDoc => expecting iter:Handle => %s\n", strtype(iter.type));
		return s.status = ERROR_script_internal, s;
	}

	slot = eval_arg (&args, env);

	if (vt_lval != slot.type) {
		fprintf(stderr, "Error: prevDoc => expecting Document:Symbol => %s\n", strtype(slot.type));
		return s.status = ERROR_script_internal, s;
	}

	slot.lval->bits = vt_document;
	slot.lval->document = iteratorPrev(iter.hndl, &docId);

	v.bits = vt_docId;
	v.docId.bits = docId.bits;
	return v;
}

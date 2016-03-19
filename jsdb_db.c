#include "jsdb.h"
#include "jsdb_db.h"

#ifdef _WIN32
#define strcasecmp _strnicmp
#endif

static bool debug = false;

//	jsdb_initDatabase(handle, name, size, onDisk)

Status jsdb_initDatabase(uint32_t args, environment_t *env) {
	value_t v, name, onDisk, *slot;
	uint64_t size;
	Status s;

	if (debug) fprintf(stderr, "funcall : InitDatabase\n");

	v = eval_arg (&args, env);
	slot = v.ref;

	if (vt_ref != v.type) {
		fprintf(stderr, "Error: initDatabase => expecting Handle:Ref => %s\n", strtype(v.type));
		return ERROR_script_internal;
	}

	name = eval_arg (&args, env);

	if (vt_string != name.type) {
		fprintf(stderr, "Error: initDatabase => expecting name:String => %s\n", strtype(name.type));
		return ERROR_script_internal;
	}

	size = conv2Int(eval_arg (&args, env)).nval;
	onDisk = conv2Bool(eval_arg (&args, env));

	v = createDocStore(name, catalog, size, onDisk.boolean);
	v.aux = hndl_database;

	replaceSlotValue(slot, &v);
	return OK;
}

//  createIndex(docStore, keys, idxname, type, size, onDisk, unique, partial) 

Status jsdb_createIndex(uint32_t args, environment_t *env) {
	value_t v, name, onDisk, docStore, unique, partial, keys, type;
	uint64_t size;
	Status s;

	if (debug) fprintf(stderr, "funcall : CreateIndex\n");

	docStore = eval_arg (&args, env);

	if (vt_handle != docStore.type || hndl_docStore != docStore.aux) {
		fprintf(stderr, "Error: createIndex => expecting Handle:docStore => %s\n", strtype(docStore.type));
		return ERROR_script_internal;
	}

	keys = eval_arg (&args, env);

	if (vt_object != keys.type) {
		fprintf(stderr, "Error: createIndex => expecting Object:keys => %s\n", strtype(keys.type));
		return ERROR_script_internal;
	}

	name = eval_arg (&args, env);

	if (vt_string != name.type) {
		fprintf(stderr, "Error: createIndex => expecting Name:string => %s\n", strtype(name.type));
		return ERROR_script_internal;
	}

	type = eval_arg (&args, env);

	if (vt_string != type.type) {
		fprintf(stderr, "Error: createIndex => expecting Type:string => %s\n", strtype(type.type));
		return ERROR_script_internal;
	}

	size = conv2Int(eval_arg (&args, env)).nval;

	onDisk = conv2Bool(eval_arg (&args, env));

	unique = conv2Bool(eval_arg (&args, env));

	partial = eval_arg (&args, env);

	if (!strcasecmp(type.str, "btree", type.aux))
		return createBtreeIndex(docStore.h, keys, name, size, onDisk.boolean, unique.boolean, partial, getSet(docStore.h));

	if (!strcasecmp(type.str, "art", type.aux))
		return createArtIndex(docStore.h, keys, name, size, onDisk.boolean, unique.boolean, partial, getSet(docStore.h));

	fprintf(stderr, "Error: createIndex => invalid type: => %.*s\n", type.aux, type.str);
	return ERROR_script_internal;
}

//	jdsd_drop(Handle)

Status jsdb_drop(uint32_t args, environment_t *env) {
	value_t v;
	Status s;

	if (debug) fprintf(stderr, "funcall : drop\n");

	v = eval_arg (&args, env);

	if (vt_handle != v.type) {
		fprintf(stderr, "Error: drop => expecting Handle => %s\n", strtype(v.type));
		return ERROR_script_internal;
	}

	if (hndl_artIndex != v.aux || hndl_btreeIndex != v.aux || hndl_database != v.aux || hndl_docStore != v.aux) {
		fprintf(stderr, "Error: drop => unsupported handle type\n", strtype(v.type));
		return ERROR_script_internal;
	}

	((DbMap *)v.h)->arena->drop = 1;
	return OK;
}

Status jsdb_createCursor(uint32_t args, environment_t *env) {
	value_t v, *slot, direction, result, index;
	Status s;

	if (debug) fprintf(stderr, "funcall : MakeCursor\n");

	index = eval_arg (&args, env);

	if (vt_handle != index.type) {
		fprintf(stderr, "Error: createCursor => expecting index:Handle => %s\n", strtype(index.type));
		return ERROR_script_internal;
	}

	v = eval_arg (&args, env);
	slot = v.ref;

	if (vt_ref != v.type) {
		fprintf(stderr, "Error: createCursor => expecting cursor:Symbol => %s\n", strtype(v.type));
		return ERROR_script_internal;
	}

	direction = conv2Bool(eval_arg (&args, env));

	switch (index.aux) {
	case hndl_btreeIndex:
		result = btreeCursor(index.h, direction.boolean);
		break;
		
	case hndl_artIndex:
		result = artCursor(index.h, direction.boolean);
		break;
	default:
		fprintf(stderr, "Error: createCursor => expecting index:Handle => %s\n", strtype(index.type));
		return ERROR_script_internal;
	}

	replaceSlotValue(slot, &result);
	return OK;
}

Status jsdb_seekKey(uint32_t args, environment_t *env) {
	value_t v, key, cursor, val;
	Status s;

	if (debug) fprintf(stderr, "funcall : SeekKey\n");

	cursor = eval_arg (&args, env);

	if (vt_handle != cursor.type ) {
		fprintf(stderr, "Error: seekKey => expecting cursor:Handle => %s\n", strtype(cursor.type));
		return ERROR_script_internal;
	}

	key = eval_arg (&args, env);

	if (vt_string != key.type) {
		fprintf(stderr, "Error: seekKey => expecting key:String => %s\n", strtype(key.type));
		return ERROR_script_internal;
	}

	v = eval_arg (&args, env);

	if (vt_ref != v.type) {
		fprintf(stderr, "Error: seekKey => expecting found:bool => %s\n", strtype(v.type));
		return ERROR_script_internal;
	}

	val.bits = vt_bool;

	switch (cursor.aux) {
	case hndl_artCursor:
		val.boolean = artSeekKey(cursor.h, key.str, key.aux);
	case hndl_btreeCursor:
		val.boolean = btreeSeekKey(cursor.h, key.str, key.aux);
	default:
		fprintf(stderr, "Error: seekKey => expecting cursor:Handle => %s\n", strtype(cursor.type));
		return ERROR_script_internal;
	}

	replaceSlotValue(v.ref, &val);
	return OK;
}

Status jsdb_nextKey(uint32_t args, environment_t *env) {
	value_t v, cursor;
	Status s;

	if (debug) fprintf(stderr, "funcall : NextKey\n");

	cursor = eval_arg (&args, env);

	if (vt_handle != cursor.type) {
		fprintf(stderr, "Error: nextKey => expecting cursor:Handle => %s\n", strtype(cursor.type));
		return ERROR_script_internal;
	}

	switch (cursor.aux) {
	case hndl_artCursor:
		s = artNextKey(cursor.h);
		break;
	case hndl_btreeCursor:
		s = btreeNextKey(cursor.h);
		break;
	}

	if (OK!=s)
		fprintf(stderr, "Error: nextKey => %s\n", strstatus(s));

	return OK;
}

Status jsdb_prevKey(uint32_t args, environment_t *env) {
	value_t v, cursor;
	Status s;

	if (debug) fprintf(stderr, "funcall : PrevKey\n");

	cursor = eval_arg (&args, env);

	if (vt_handle != cursor.type) {
		fprintf(stderr, "Error: prevKey => expecting cursor:Handle => %s\n", strtype(cursor.type));
		return ERROR_script_internal;
	}

	switch (cursor.aux) {
	case hndl_artCursor:
		s = artPrevKey(cursor.h);
		break;
	case hndl_btreeCursor:
		s = btreePrevKey(cursor.h);
		break;
	}


	if (OK!=s)
		fprintf(stderr, "Error: prevKey => %s\n", strstatus(s));
	return OK;
}

Status jsdb_getKey(uint32_t args, environment_t *env) {
	value_t v, *slot, cursor;
	Status s;

	if (debug) fprintf(stderr, "funcall : GetKey\n");

	cursor = eval_arg (&args, env);

	if (vt_handle != cursor.type) {
		fprintf(stderr, "Error: getKey => expecting cursor:Handle => %s\n", strtype(cursor.type));
		return ERROR_script_internal;
	}

	v = eval_arg (&args, env);
	slot = v.ref;

	if (vt_ref != v.type) {
		fprintf(stderr, "Error: getKey => expecting key:Symbol => %s\n", strtype(v.type));
		return ERROR_script_internal;
	}

	switch (cursor.aux) {
	case hndl_artCursor:
		v = artCursorKey(cursor.h);
		break;
	case hndl_btreeCursor:
		v = btreeCursorKey(cursor.h);
		break;
	default:
		fprintf(stderr, "Error: getKey => expecting cursor:Handle => %s\n", strtype(cursor.type));
		return ERROR_script_internal;
	}

	replaceSlotValue(slot, &v);
	return OK;
}

//	createDocStore(handle, database, name, size, onDisk, created)

Status jsdb_createDocStore(uint32_t args, environment_t *env) {
	value_t v, name, *slot, onDisk, database, created, docStore;
	uint64_t size;
	Status s;

	if (debug) fprintf(stderr, "funcall : CreateDocStore\n");

	v = eval_arg (&args, env);
	slot = v.ref;

	if (vt_ref != v.type) {
		fprintf(stderr, "Error: createDocStore => expecting handle:Symbol => %s\n", strtype(v.type));
		return ERROR_script_internal;
	}

	database = eval_arg (&args, env);

	if (vt_handle != database.type || hndl_database != database.aux) {
		fprintf(stderr, "Error: createDocStore => expecting Database:handle => %s\n", strtype(database.type));
		return ERROR_script_internal;
	}

	name = eval_arg (&args, env);

	if (vt_string != name.type) {
		fprintf(stderr, "Error: createDocStore => expecting Name => %s\n", strtype(name.type));
		return ERROR_script_internal;
	}

	size = conv2Int(eval_arg (&args, env)).nval;
	onDisk = conv2Bool(eval_arg (&args, env));

	docStore = createDocStore(name, database.h, size, onDisk.boolean);
	replaceSlotValue(slot, &docStore);

	v = eval_arg (&args, env);
	slot = v.ref;

	if (vt_ref != v.type) {
		fprintf(stderr, "Error: createDocStore => expecting created:Symbol => %s\n", strtype(v.type));
		return ERROR_script_internal;
	}

	v.bits = vt_bool;
	v.boolean = ((DbMap *)v.h)->created;
	replaceSlotValue(slot, &v);

	return OK;
}

Status jsdb_findDoc(uint32_t args, environment_t *env) {
	value_t v, *slot, docStore;
	DocId docId;
	Status s;

	if (debug) fprintf(stderr, "funcall : FindDoc\n");

	docStore = eval_arg (&args, env);

	if (vt_handle != docStore.type) {
		fprintf(stderr, "Error: findDoc => expecting store:Handle => %s\n", strtype(docStore.type));
		return ERROR_script_internal;
	}

	v = eval_arg (&args, env);
	docId.bits = v.docId.bits;

	if (vt_docId != v.type) {
		fprintf(stderr, "Error: findDoc => expecting id:docid => %s\n", strtype(v.type));
		return ERROR_script_internal;
	}

	v = eval_arg (&args, env);
	slot = v.ref;

	if (vt_ref != v.type) {
		fprintf(stderr, "Error: findDoc => expecting document:Symbol => %s\n", strtype(v.type));
		return ERROR_script_internal;
	}

	v.bits = vt_document;
	v.document = findDoc(docStore.h, docId);

	replaceSlotValue(slot, &v);
	return OK;
}

Status jsdb_deleteDoc(uint32_t args, environment_t *env) {
	value_t v, docStore;
	Status s;

	if (debug) fprintf(stderr, "funcall : DeleteDoc\n");

	docStore = eval_arg (&args, env);

	if (vt_handle != docStore.type) {
		fprintf(stderr, "Error: deleteDoc => expecting Handle => %s\n", strtype(docStore.type));
		return ERROR_script_internal;
	}

	v = eval_arg (&args, env);

	if (vt_docId != v.type) {
		fprintf(stderr, "Error: deleteDoc => expecting Number => %s\n", strtype(v.type));
		return ERROR_script_internal;
	}

	s = deleteDoc(docStore.h, v.docId);

	if (OK!=s)
		fprintf(stderr, "Error: deleteDoc => %s\n", strstatus(s));
	return OK;
}

Status jsdb_createIterator(uint32_t args, environment_t *env) {
	value_t v, *slot, docStore, iter;
	Status s;

	if (debug) fprintf(stderr, "funcall : CreateIterator\n");

	docStore = eval_arg (&args, env);

	if (vt_handle != docStore.type) {
		fprintf(stderr, "Error: createIterator => expecting store:Handle => %s\n", strtype(docStore.type));
		return ERROR_script_internal;
	}

	v = eval_arg (&args, env);
	slot = v.ref;

	if (vt_ref != v.type) {
		fprintf(stderr, "Error: createIterator => expecting iter:Symbol => %s\n", strtype(v.type));
		return ERROR_script_internal;
	}

	v = createIterator(docStore.h, true);

	replaceSlotValue(slot, &v);
	return OK;
}

Status jsdb_seekDoc(uint32_t args, environment_t *env) {
	value_t v, *slot, iter, val;

	if (debug) fprintf(stderr, "funcall : SeekDoc\n");

	iter = eval_arg (&args, env);

	if (vt_handle != iter.type || hndl_iterator != iter.aux) {
		fprintf(stderr, "Error: seekDoc => expecting iter:Handle => %s\n", strtype(iter.type));
		return ERROR_script_internal;
	}

	v = eval_arg (&args, env);
	slot = v.ref;

	if (vt_ref != v.type) {
		fprintf(stderr, "Error: seekDoc => expecting Document:Symbol => %s\n", strtype(v.type));
		return ERROR_script_internal;
	}

	v = eval_arg (&args, env);

	if (vt_docId != v.type) {
		fprintf(stderr, "Error: seekDoc => expecting DocId:Symbol => %s\n", strtype(v.type));
		return ERROR_script_internal;
	}

	val.document = iteratorSeek(iter.h, v.docId);

	if (!val.document)
		val.bits = vt_uninitialized;

	replaceSlotValue(slot, &val);
	return OK;
}

// nextDoc(iterator, &docId, &document)

Status jsdb_nextDoc(uint32_t args, environment_t *env) {
	value_t v, *slot, val, iter;
	DocId docId;

	if (debug) fprintf(stderr, "funcall : NextDoc\n");

	iter = eval_arg (&args, env);

	if (vt_handle != iter.type) {
		fprintf(stderr, "Error: nextDoc => expecting iter:Handle => %s\n", strtype(iter.type));
		return ERROR_script_internal;
	}

	v = eval_arg (&args, env);
	slot =  v.ref;

	if (vt_ref != v.type) {
		fprintf(stderr, "Error: nextDoc => expecting docid:Symbol => %s\n", strtype(v.type));
		return ERROR_script_internal;
	}

	val.bits = vt_document;
	val.document = iteratorNext(iter.h, &docId);

	v.bits = vt_docId;
	v.docId.bits = docId.bits;
	replaceSlotValue(slot, &v);

	v = eval_arg (&args, env);

	if (vt_ref != v.type) {
		fprintf(stderr, "Error: nextDoc => expecting document:Symbol => %s\n", strtype(v.type));
		return ERROR_script_internal;
	}

	if (!val.document)
		val.bits = vt_uninitialized;

	replaceSlotValue(v.ref, &val);
	return OK;
}

// prevDoc(iterator, &docId, &document)

Status jsdb_prevDoc(uint32_t args, environment_t *env) {
	value_t v, *slot, val, iter;
	DocId docId;
	Status s;

	if (debug) fprintf(stderr, "funcall : PrevDoc\n");

	iter = eval_arg (&args, env);

	if (vt_handle != iter.type || hndl_iterator != iter.aux) {
		fprintf(stderr, "Error: prevDoc => expecting iter:Handle => %s\n", strtype(iter.type));
		return ERROR_script_internal;
	}

	v = eval_arg (&args, env);
	slot =  v.ref;

	if (vt_ref != v.type) {
		fprintf(stderr, "Error: prevDoc => expecting DocId:Symbol => %s\n", strtype(v.type));
		return ERROR_script_internal;
	}

	val.bits = vt_document;
	val.document = iteratorPrev(iter.h, &docId);

	v.bits = vt_docId;
	v.docId.bits = docId.bits;
	replaceSlotValue(slot, &v);

	v = eval_arg (&args, env);

	if (vt_ref != v.type) {
		fprintf(stderr, "Error: prevDoc => expecting Document:Symbol => %s\n", strtype(v.type));
		return ERROR_script_internal;
	}

	if (!val.document)
		val.bits = vt_uninitialized;

	replaceSlotValue(v.ref, &val);
	return OK;
}

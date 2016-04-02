#include "jsdb.h"
#include "jsdb_db.h"

static bool debug = false;

//	jsdb_initDatabase(handle, name, size, onDisk)

value_t jsdb_initDatabase(uint32_t args, environment_t *env) {
	value_t v, name, onDisk, *slot;
	uint64_t size;
	value_t s;

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : InitDatabase\n");

	v = eval_arg (&args, env);
	slot = v.ref;

	if (vt_ref != v.type) {
		fprintf(stderr, "Error: initDatabase => expecting Handle:Ref => %s\n", strtype(v.type));
		return s.status = ERROR_script_internal, s;
	}

	name = eval_arg (&args, env);

	if (vt_string != name.type) {
		fprintf(stderr, "Error: initDatabase => expecting name:String => %s\n", strtype(name.type));
		return s.status = ERROR_script_internal, s;
	}

	v = eval_arg (&args, env);
	size = conv2Int(v).nval;
	abandonValue(v);

	v = eval_arg (&args, env);
	onDisk = conv2Bool(v);
	abandonValue(v);

	v = createDocStore(name, catalog, size, onDisk.boolean);
	v.aval->array[0].aux = hndl_database;

	replaceSlotValue(slot, v.aval->array[0]);
	abandonValue(v);
	return s.status = OK, s;
}

//  createIndex(docStore, handle, keys, idxname, type, size, onDisk, unique, partial) 

value_t jsdb_createIndex(uint32_t args, environment_t *env) {
	value_t v, name, onDisk, docStore, unique, partial, keys, type, sparse;
	value_t *slot;
	uint64_t size;
	value_t s;

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : CreateIndex\n");

	docStore = eval_arg (&args, env);

	if (vt_array != docStore.type || hndl_docStore != docStore.aval->array[0].aux) {
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
	size = conv2Int(v).nval;
	abandonValue(v);

	v = eval_arg (&args, env);
	onDisk = conv2Bool(v);
	abandonValue(v);

	v = eval_arg (&args, env);
	unique = conv2Bool(v);
	abandonValue(v);

	v = eval_arg (&args, env);
	sparse = conv2Bool(v);
	abandonValue(v);

	partial = eval_arg (&args, env);

	v = createIndex(docStore.aval->array[0].hndl, type, keys, name, size, onDisk.boolean, unique.boolean, sparse.boolean, partial, getSet(docStore.aval->array[0].hndl));

	vec_push(docStore.aval->array, v);
	incrRefCnt(v);

	abandonValue(type);
	abandonValue(keys);
	abandonValue(name);
	abandonValue(partial);
	return s.status = OK, s;
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

	if (hndl_artIndex != v.aux || hndl_btreeIndex != v.aux || hndl_database != v.aux || hndl_docStore != v.aux) {
		fprintf(stderr, "Error: drop => unsupported handle type\n", strtype(v.type));
		return s.status = ERROR_script_internal, s;
	}

	((DbMap *)v.hndl)->arena->drop = 1;
	return s.status = OK, s;
}

//  createCursor(index, handle, direction)

value_t jsdb_createCursor(uint32_t args, environment_t *env) {
	value_t v, *slot, direction, result, index;
	value_t s;

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : MakeCursor\n");

	index = eval_arg (&args, env);

	if (vt_handle != index.type) {
		fprintf(stderr, "Error: createCursor => expecting index:Handle => %s\n", strtype(index.type));
		return s.status = ERROR_script_internal, s;
	}

	v = eval_arg (&args, env);
	slot = v.ref;

	if (vt_ref != v.type) {
		fprintf(stderr, "Error: createCursor => expecting cursor:Symbol => %s\n", strtype(v.type));
		return s.status = ERROR_script_internal, s;
	}

	v = eval_arg (&args, env);
	direction = conv2Bool(v);
	abandonValue(v);

	switch (index.aux) {
	case hndl_btreeIndex:
		result = btreeCursor(index.hndl, direction.boolean);
		break;
		
	case hndl_artIndex:
		result = artCursor(index.hndl, direction.boolean);
		break;
	default:
		fprintf(stderr, "Error: createCursor => expecting index:Handle => %s\n", strtype(index.type));
		return s.status = ERROR_script_internal, s;
	}

	replaceSlotValue(slot, result);
	return s.status = OK, s;
}

value_t jsdb_seekKey(uint32_t args, environment_t *env) {
	value_t v, key, cursor, val;
	value_t s;

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : SeekKey\n");

	cursor = eval_arg (&args, env);

	if (vt_handle != cursor.type ) {
		fprintf(stderr, "Error: seekKey => expecting cursor:Handle => %s\n", strtype(cursor.type));
		return s.status = ERROR_script_internal, s;
	}

	key = eval_arg (&args, env);

	if (vt_string != key.type) {
		fprintf(stderr, "Error: seekKey => expecting key:String => %s\n", strtype(key.type));
		return s.status = ERROR_script_internal, s;
	}

	v = eval_arg (&args, env);

	if (vt_ref != v.type) {
		fprintf(stderr, "Error: seekKey => expecting found:bool => %s\n", strtype(v.type));
		return s.status = ERROR_script_internal, s;
	}

	val.bits = vt_bool;

	switch (cursor.aux) {
	case hndl_artCursor:
		val.boolean = artSeekKey(cursor.hndl, key.str, key.aux);
	case hndl_btreeCursor:
		val.boolean = btreeSeekKey(cursor.hndl, key.str, key.aux);
	default:
		fprintf(stderr, "Error: seekKey => expecting cursor:Handle => %s\n", strtype(cursor.type));
		return s.status = ERROR_script_internal, s;
	}

	replaceSlotValue(v.ref, val);
	return s.status = OK, s;
}

// nextKey(cursor, docId)

value_t jsdb_nextKey(uint32_t args, environment_t *env) {
	value_t v, cursor, *slot;
	DocId docId;
	value_t s;

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : NextKey\n");

	cursor = eval_arg (&args, env);

	if (vt_handle != cursor.type) {
		fprintf(stderr, "Error: nextKey => expecting cursor:Handle => %s\n", strtype(cursor.type));
		return s.status = ERROR_script_internal, s;
	}

	v = eval_arg (&args, env);
	slot = v.ref;

	if (vt_ref != v.type) {
		fprintf(stderr, "Error: nextKey => expecting docId:Symbol => %s\n", strtype(v.type));
		return s.status = ERROR_script_internal, s;
	}

	switch (cursor.aux) {
	case hndl_artCursor:
		if ((artNextKey(cursor.hndl)))
			docId.bits = artDocId(cursor.hndl);
		else
			return s.status = ERROR_endoffile, s;
		break;
	case hndl_btreeCursor:
		if ((btreeNextKey(cursor.hndl)))
			docId.bits = btreeDocId(cursor.hndl);
		else
			return s.status = ERROR_endoffile, s;
		break;
	}

	v.bits = vt_docId;
	v.docId = docId;
	replaceSlotValue(slot, v);
	return s.status = OK, s;
}

// prevKey(cursor, docId)

value_t jsdb_prevKey(uint32_t args, environment_t *env) {
	value_t v, cursor, *slot;
	DocId docId;
	value_t s;

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : PrevKey\n");

	cursor = eval_arg (&args, env);

	if (vt_handle != cursor.type) {
		fprintf(stderr, "Error: prevKey => expecting cursor:Handle => %s\n", strtype(cursor.type));
		return s.status = ERROR_script_internal, s;
	}

	v = eval_arg (&args, env);
	slot = v.ref;

	if (vt_ref != v.type) {
		fprintf(stderr, "Error: prevKey => expecting docId:Symbol => %s\n", strtype(v.type));
		return s.status = ERROR_script_internal, s;
	}

	switch (cursor.aux) {
	case hndl_artCursor:
		if (artPrevKey(cursor.hndl))
			docId.bits = artDocId(cursor.hndl);
		else
			return s.status = ERROR_endoffile, s;
		break;
	case hndl_btreeCursor:
		if (btreePrevKey(cursor.hndl))
			docId.bits = artDocId(cursor.hndl);
		else
			return s.status = ERROR_endoffile, s;
		break;
	}

	v.bits = vt_docId;
	v.docId = docId;
	replaceSlotValue(slot, v);
	return s.status = OK, s;
}

value_t jsdb_getKey(uint32_t args, environment_t *env) {
	value_t v, *slot, cursor;
	value_t s;

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : GetKey\n");

	cursor = eval_arg (&args, env);

	if (vt_handle != cursor.type) {
		fprintf(stderr, "Error: getKey => expecting cursor:Handle => %s\n", strtype(cursor.type));
		return s.status = ERROR_script_internal, s;
	}

	v = eval_arg (&args, env);
	slot = v.ref;

	if (vt_ref != v.type) {
		fprintf(stderr, "Error: getKey => expecting key:Symbol => %s\n", strtype(v.type));
		return s.status = ERROR_script_internal, s;
	}

	switch (cursor.aux) {
	case hndl_artCursor:
		v = artCursorKey(cursor.hndl);
		break;
	case hndl_btreeCursor:
		v = btreeCursorKey(cursor.hndl);
		break;
	default:
		fprintf(stderr, "Error: getKey => expecting cursor:Handle => %s\n", strtype(cursor.type));
		return s.status = ERROR_script_internal, s;
	}

	replaceSlotValue(slot, v);
	return s.status = OK, s;
}

//	createDocStore(handle, database, name, size, onDisk, created)

value_t jsdb_createDocStore(uint32_t args, environment_t *env) {
	value_t v, name, *slot, onDisk, database, created, docStore;
	uint64_t size;
	value_t s;

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : CreateDocStore\n");

	v = eval_arg (&args, env);
	slot = v.ref;

	if (vt_ref != v.type) {
		fprintf(stderr, "Error: createDocStore => expecting handle:Symbol => %s\n", strtype(v.type));
		return s.status = ERROR_script_internal, s;
	}

	database = eval_arg (&args, env);

	if (vt_handle != database.type || hndl_database != database.aux) {
		fprintf(stderr, "Error: createDocStore => expecting Database:handle => %s\n", strtype(database.type));
		return s.status = ERROR_script_internal, s;
	}

	name = eval_arg (&args, env);

	if (vt_string != name.type) {
		fprintf(stderr, "Error: createDocStore => expecting Name => %s\n", strtype(name.type));
		return s.status = ERROR_script_internal, s;
	}

	v = eval_arg(&args, env);
	size = conv2Int(v).nval;
	abandonValue(v);

	v = eval_arg(&args, env);
	onDisk = conv2Bool(v);
	abandonValue(v);

	docStore = createDocStore(name, database.hndl, size, onDisk.boolean);
	replaceSlotValue(slot, docStore);

	v = eval_arg (&args, env);
	slot = v.ref;

	if (vt_ref != v.type) {
		fprintf(stderr, "Error: createDocStore => expecting created:Symbol => %s\n", strtype(v.type));
		return s.status = ERROR_script_internal, s;
	}

	v.bits = vt_bool;
	v.boolean = ((DbMap *)docStore.aval->array[0].hndl)->created;
	replaceSlotValue(slot, v);

	abandonValue(name);
	return s.status = OK, s;
}

//  findDoc (docStore, docId, document)

value_t jsdb_findDoc(uint32_t args, environment_t *env) {
	value_t v, *slot, docStore;
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

	v = eval_arg (&args, env);
	slot = v.ref;

	if (vt_ref != v.type) {
		fprintf(stderr, "Error: findDoc => expecting document:Symbol => %s\n", strtype(v.type));
		return s.status = ERROR_script_internal, s;
	}

	v.bits = vt_document;
	v.document = findDoc(docStore.hndl, docId);

	replaceSlotValue(slot, v);
	return s.status = OK, s;
}

value_t jsdb_deleteDoc(uint32_t args, environment_t *env) {
	value_t v, docStore, s;
	uint32_t set;
	Status stat;

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : DeleteDoc\n");

	docStore = eval_arg (&args, env);

	if (vt_handle != docStore.type) {
		fprintf(stderr, "Error: deleteDoc => expecting Handle => %s\n", strtype(docStore.type));
		return s.status = ERROR_script_internal, s;
	}

	set = getSet(docStore.aval->array[0].hndl);
	v = eval_arg (&args, env);

	if (vt_docId != v.type) {
		fprintf(stderr, "Error: deleteDoc => expecting Number => %s\n", strtype(v.type));
		return s.status = ERROR_script_internal, s;
	}

	s.status = deleteDoc(docStore.aval, v.docId, set);
	return s;
}

value_t jsdb_createIterator(uint32_t args, environment_t *env) {
	value_t v, *slot, docStore, iter;
	value_t s;

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : CreateIterator\n");

	docStore = eval_arg (&args, env);

	if (vt_handle != docStore.type) {
		fprintf(stderr, "Error: createIterator => expecting store:Handle => %s\n", strtype(docStore.type));
		return s.status = ERROR_script_internal, s;
	}

	v = eval_arg (&args, env);
	slot = v.ref;

	if (vt_ref != v.type) {
		fprintf(stderr, "Error: createIterator => expecting iter:Symbol => %s\n", strtype(v.type));
		return s.status = ERROR_script_internal, s;
	}

	v = createIterator(docStore.hndl, true);

	replaceSlotValue(slot, v);
	return s.status = OK, s;
}

value_t jsdb_seekDoc(uint32_t args, environment_t *env) {
	value_t v, *slot, iter, val, s;

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : SeekDoc\n");

	iter = eval_arg (&args, env);

	if (vt_handle != iter.type || hndl_iterator != iter.aux) {
		fprintf(stderr, "Error: seekDoc => expecting iter:Handle => %s\n", strtype(iter.type));
		return s.status = ERROR_script_internal, s;
	}

	v = eval_arg (&args, env);
	slot = v.ref;

	if (vt_ref != v.type) {
		fprintf(stderr, "Error: seekDoc => expecting Document:Symbol => %s\n", strtype(v.type));
		return s.status = ERROR_script_internal, s;
	}

	v = eval_arg (&args, env);

	if (vt_docId != v.type) {
		fprintf(stderr, "Error: seekDoc => expecting DocId:Symbol => %s\n", strtype(v.type));
		return s.status = ERROR_script_internal, s;
	}

	val.document = iteratorSeek(iter.hndl, v.docId);

	if (!val.document)
		val.bits = vt_null;

	replaceSlotValue(slot, val);
	return s.status = OK, s;
}

// nextDoc(iterator, &docId, &document)

value_t jsdb_nextDoc(uint32_t args, environment_t *env) {
	value_t v, *slot, val, iter, s;
	DocId docId;

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : NextDoc\n");

	iter = eval_arg (&args, env);

	if (vt_handle != iter.type) {
		fprintf(stderr, "Error: nextDoc => expecting iter:Handle => %s\n", strtype(iter.type));
		return s.status = ERROR_script_internal, s;
	}

	v = eval_arg (&args, env);
	slot =  v.ref;

	if (vt_ref != v.type) {
		fprintf(stderr, "Error: nextDoc => expecting docid:Symbol => %s\n", strtype(v.type));
		return s.status = ERROR_script_internal, s;
	}

	val.bits = vt_document;
	val.document = iteratorNext(iter.hndl, &docId);

	v.bits = vt_docId;
	v.docId.bits = docId.bits;
	replaceSlotValue(slot, v);

	v = eval_arg (&args, env);

	if (vt_ref != v.type) {
		fprintf(stderr, "Error: nextDoc => expecting document:Symbol => %s\n", strtype(v.type));
		return s.status = ERROR_script_internal, s;
	}

	if (!val.document)
		val.bits = vt_null;

	replaceSlotValue(v.ref, val);
	return s.status = OK, s;
}

// prevDoc(iterator, &docId, &document)

value_t jsdb_prevDoc(uint32_t args, environment_t *env) {
	value_t v, *slot, val, iter;
	DocId docId;
	value_t s;

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : PrevDoc\n");

	iter = eval_arg (&args, env);

	if (vt_handle != iter.type || hndl_iterator != iter.aux) {
		fprintf(stderr, "Error: prevDoc => expecting iter:Handle => %s\n", strtype(iter.type));
		return s.status = ERROR_script_internal, s;
	}

	v = eval_arg (&args, env);
	slot =  v.ref;

	if (vt_ref != v.type) {
		fprintf(stderr, "Error: prevDoc => expecting DocId:Symbol => %s\n", strtype(v.type));
		return s.status = ERROR_script_internal, s;
	}

	val.bits = vt_document;
	val.document = iteratorPrev(iter.hndl, &docId);

	v.bits = vt_docId;
	v.docId.bits = docId.bits;
	replaceSlotValue(slot, v);

	v = eval_arg (&args, env);

	if (vt_ref != v.type) {
		fprintf(stderr, "Error: prevDoc => expecting Document:Symbol => %s\n", strtype(v.type));
		return s.status = ERROR_script_internal, s;
	}

	if (!val.document)
		val.bits = vt_null;

	replaceSlotValue(v.ref, val);
	return s.status = OK, s;
}

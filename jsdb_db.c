#include "jsdb.h"
#include "jsdb_db.h"

static bool debug = false;
extern value_t makeCursor( DbMap *index, bool dir, value_t start, value_t limits);

//  createIndex(docStore, keys, idxname, type, size, onDisk, unique, partial) 

value_t jsdb_createIndex(uint32_t args, environment_t *env) {
	value_t v, name, docStore, unique, partial, keys, type, sparse;
	uint64_t size;
	value_t s;

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : CreateIndex\n");

	docStore = eval_arg (&args, env);

	if (vt_object != docStore.type || hndl_docStore != docStore.oval->pairs[0].value.aux) {
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

	v = createIndex(docStore.oval->pairs[0].value.hndl, type, keys, name, size, unique.boolean, sparse.boolean, partial, getSet(docStore.oval->pairs[0].value.hndl));

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

	if (hndl_artIndex != v.aux || hndl_btreeIndex != v.aux || hndl_docStore != v.aux) {
		fprintf(stderr, "Error: drop => unsupported handle type\n", strtype(v.type));
		return s.status = ERROR_script_internal, s;
	}

	((DbMap *)v.hndl)->arena->drop = 1;
	return s.status = OK, s;
}

//  createCursor(index, dir, start, limit)

value_t jsdb_createCursor(uint32_t args, environment_t *env) {
	value_t v, start, result, index, dir, limits;
	value_t s;

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : createCursor\n");

	index = eval_arg (&args, env);

	if (vt_handle != index.type) {
		fprintf(stderr, "Error: createCursor => expecting index:handle => %s\n", strtype(index.type));
		return s.status = ERROR_script_internal, s;
	}

	dir = eval_arg (&args, env);

	if (vt_bool != dir.type) {
		fprintf(stderr, "Error: createCursor => expecting direction:bool => %s\n", strtype(dir.type));
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

	return makeCursor(index.hndl, dir.boolean, start, limits);
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
	case hndl_artCursor:
		if ((artNextKey(cursor.hndl)))
			docId.bits = artDocId(cursor.hndl);
		else
			docId.bits = 0;
		break;
	case hndl_btreeCursor:
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
	case hndl_artCursor:
		if (artPrevKey(cursor.hndl))
			docId.bits = artDocId(cursor.hndl);
		else
			docId.bits = 0;
		break;
	case hndl_btreeCursor:
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

	return v;
}

//	createDocStore(name, size, onDisk, created)

value_t jsdb_createDocStore(uint32_t args, environment_t *env) {
	value_t v, name, slot, onDisk, created, docStore;
	uint64_t size;
	value_t s;

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : CreateDocStore\n");

	name = eval_arg (&args, env);

	if (vt_string != name.type) {
		fprintf(stderr, "Error: createDocStore => expecting Name => %s\n", strtype(name.type));
		return s.status = ERROR_script_internal, s;
	}

	v = eval_arg(&args, env);
	size = conv2Int(v, true).nval;

	v = eval_arg(&args, env);
	onDisk = conv2Bool(v, true);

	docStore = createDocStore(name, size, onDisk.boolean);

	slot = eval_arg (&args, env);

	if (vt_lval != slot.type) {
		fprintf(stderr, "Error: createDocStore => expecting created:Symbol => %s\n", strtype(slot.type));
		return s.status = ERROR_script_internal, s;
	}

	v.bits = vt_bool;
	v.boolean = ((DbMap *)docStore.oval->pairs[0].value.hndl)->created;
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

//	jsdb_deleteDoc(docStore, docId)

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

	set = getSet(docStore.aval->values[0].hndl);
	v = eval_arg (&args, env);

	if (vt_docId != v.type) {
		fprintf(stderr, "Error: deleteDoc => expecting Number => %s\n", strtype(v.type));
		return s.status = ERROR_script_internal, s;
	}

	s.status = deleteDoc(docStore.oval, v.docId, set);
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

	if (vt_handle != iter.type || hndl_iterator != iter.aux) {
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

	if (vt_handle != iter.type || hndl_iterator != iter.aux) {
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

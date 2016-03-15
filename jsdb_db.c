#include "jsdb.h"
#include "jsdb_db.h"
#include "jsdb_arena.h"
#include "jsdb_docs.h"
#include "jsdb_btree.h"
#include "jsdb_art.h"

static bool debug = false;

Status jsdb_initEngine(uint32_t args, environment_t *env) {
	value_t v, name, inMem, *slot;
	uint64_t size;
    Status s;

    if (debug) fprintf(stderr, "funcall : InitEngine\n");

    v = eval_arg (&args, env);
	slot = v.ref;

    if (vt_ref != v.type) {
        fprintf(stderr, "Error: initEngine => expecting Handle:Ref => %s\n", strtype(v.type));
        return ERROR_script_internal;
    }

    name = eval_arg (&args, env);

    if (vt_string != name.type) {
        fprintf(stderr, "Error: initEngine => expecting name:String => %s\n", strtype(name.type));
        return ERROR_script_internal;
    }

    v = eval_arg (&args, env);
	size = v.nval;

    if (vt_int != v.type) {
        fprintf(stderr, "Error: initEngine => expecting size:int => %s\n", strtype(v.type));
        return ERROR_script_internal;
    }

    inMem = eval_arg (&args, env);

    if (vt_bool != inMem.type) {
        fprintf(stderr, "Error: initEngine => expecting inMem:Bool => %s\n", strtype(inMem.type));
        return ERROR_script_internal;
    }

	v.bits = vt_handle;
	v.aux = hndl_engine;
    v.h = openMap(name.str, name.aux, sizeof(DocStore), size, inMem.boolean);

    if (!v.h)
        fprintf(stderr,"Error: initEngine: %.*s\n", name.aux, name.str);

    replaceSlotValue(slot, &v);
    return OK;
}

Status jsdb_createArtIndex(uint32_t args, environment_t *env) {
    value_t v, name, *slot, inMem;
	uint64_t size;
    Status s;

    if (debug) fprintf(stderr, "funcall : CreateIndex\n");

    v = eval_arg (&args, env);
    slot = v.ref;

    if (vt_ref != v.type) {
        fprintf(stderr, "Error: createArtIndex => expecting Handle:Symbol => %s\n", strtype(v.type));
        return ERROR_script_internal;
    }

    name = eval_arg (&args, env);

    if (vt_string != v.type) {
        fprintf(stderr, "Error: createArtIndex => expecting Name:string => %s\n", strtype(v.type));
        return ERROR_script_internal;
    }

    v = eval_arg (&args, env);
	size = v.nval;

    if (vt_int != v.type) {
        fprintf(stderr, "Error: initEngine => expecting size:int => %s\n", strtype(v.type));
        return ERROR_script_internal;
    }

    inMem = eval_arg (&args, env);

    if (vt_bool != inMem.type) {
        fprintf(stderr, "Error: createArtIndex => expecting inMem:bool => %s\n", strtype(v.type));
        return ERROR_script_internal;
    }

	v.bits = vt_handle;
    v.h = openMap(name.str, name.aux, sizeof(ArtIndex), size, inMem.boolean);

    replaceSlotValue(slot, &v);
    return OK;
}

Status jsdb_drop(uint32_t args, environment_t *env) {
	DbMap *map;
    value_t v;
    Status s;

    if (debug) fprintf(stderr, "funcall : drop\n");

    v = eval_arg (&args, env);
	map = v.h;

    if (vt_handle != v.type) {
        fprintf(stderr, "Error: drop => expecting index:Handle => %s\n", strtype(v.type));
        return ERROR_script_internal;
    }

    if (hndl_artIndex != v.aux || hndl_btreeIndex != v.aux || hndl_engine != v.aux || hndl_docStore != v.aux) {
        fprintf(stderr, "Error: drop => wrong handle type\n", strtype(v.type));
        return ERROR_script_internal;
    }

	map->arena->drop = 1;
    return OK;
}

Status jsdb_createCursor(uint32_t args, environment_t *env) {
    value_t v, *slot, direction, result;
    DbMap *index;
    Status s;

    if (debug) fprintf(stderr, "funcall : MakeCursor\n");

    v = eval_arg (&args, env);
    index = v.h;

    if (vt_handle != v.type) {
        fprintf(stderr, "Error: createCursor => expecting index:Handle => %s\n", strtype(v.type));
        return ERROR_script_internal;
    }

    v = eval_arg (&args, env);
    slot = v.ref;

    if (vt_ref != v.type) {
        fprintf(stderr, "Error: createCursor => expecting cursor:Symbol => %s\n", strtype(v.type));
        return ERROR_script_internal;
    }

    direction = eval_arg (&args, env);

    if (vt_bool != direction.type) {
        fprintf(stderr, "Error: createCursor => expecting direction:bool => %s\n", strtype(v.type));
        return ERROR_script_internal;
    }

	switch (v.aux) {
	case hndl_btreeIndex:
		result = btreeCursor(index, direction.boolean);
		break;
		
	case hndl_artIndex:
		result = artCursor(index, direction.boolean);
		break;
	default:
        fprintf(stderr, "Error: createCursor => expecting index:Handle => %s\n", strtype(v.type));
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

Status jsdb_createDocStore(uint32_t args, environment_t *env) {
    value_t v, name, *slot, inMem;
    uint64_t size;
    DbMap *store;
    Status s;

    if (debug) fprintf(stderr, "funcall : CreateDocStore\n");

    v = eval_arg (&args, env);
	slot = v.ref;

    if (vt_ref != v.type) {
        fprintf(stderr, "Error: createDocStore => expecting handle:Symbol => %s\n", strtype(v.type));
        return ERROR_script_internal;
    }

    name = eval_arg (&args, env);

    if (vt_string != name.type) {
        fprintf(stderr, "Error: createDocStore => expecting Name => %s\n", strtype(name.type));
        return ERROR_script_internal;
    }

    v = eval_arg (&args, env);
    size = v.nval;

    if (vt_int != v.type) {
        fprintf(stderr, "Error: createDocStore => expecting size:number => %s\n", strtype(v.type));
        return ERROR_script_internal;
    }

    inMem = eval_arg (&args, env);

    if (vt_bool != inMem.type) {
        fprintf(stderr, "Error: createDocStore => expecting inMem:bool => %s\n", strtype(inMem.type));
        return ERROR_script_internal;
    }

	v.bits = vt_handle;
    v.refcount = true;
	v.aux = hndl_docStore;
    v.h = openMap(name.str, name.aux, sizeof(DocStore), size, inMem.boolean);

    if (!v.h) {
        fprintf(stderr, "Error: createDocStore\n");
        return ERROR_script_internal;
    }

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
    docId = v.docId;

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

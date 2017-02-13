#include "js.h"
#include "js_props.h"
#include "js_string.h"

#include "js_db.h"
#include "js_dbindex.h"

#ifdef __linux__
#define offsetof(type,member) __builtin_offsetof(type,member)
#endif

extern void cloneObject(value_t *obj); 

//	return base value for a document version
//	or a cloned copy

value_t convDocument(value_t val, bool lVal) {
	document_t *document = val.addr;

	if (lVal)
		if (!document->update->type) {
			*document->update = *document->ver->rec;
			cloneObject(document->update);
		}

	if (document->update->type)
		return *document->update;

	return *document->ver->rec;
}

//	document store Insert method
//	return docId, or array of docId

value_t fcnStoreInsert(value_t *args, value_t *thisVal) {
	object_t *oval = js_addr(*thisVal);
	Handle *docHndl, **idxHndls;
	value_t s, resp, keys;
	DocArena *docArena;
	DbHandle *hndl;
	ObjId txnId;
	ObjId docId;

	s.bits = vt_status;
	txnId.bits = 0;

	hndl = (DbHandle *)oval->base->hndl;

	if (!(docHndl = bindHandle(hndl)))
		return s.status = DB_ERROR_handleclosed, s;

	idxHndls = bindDocIndexes(docHndl);
	docArena = docarena(docHndl->map);

	// multiple document/value case

	if (args[0].type == vt_array) {
	  resp = newArray(array_value);
	  array_t *respval = resp.addr;
	  array_t *aval = js_addr(args[0]);
	  value_t *values = args[0].marshaled ? aval->valueArray : aval->valuePtr;
	  uint32_t cnt = args[0].marshaled ? aval->cnt : vec_cnt(aval->valuePtr);

	  for (int idx = 0; idx < cnt; idx++) {
		value_t v;

		docId.bits = allocObjId(docHndl->map, listFree(docHndl,0), listWait(docHndl,0), docArena->storeId);
		keys = installKeys (values[idx], idxHndls, docId, NULL);
		v.docBits = insertDoc(idxHndls, values[idx], keys, 0, NULL, docId, txnId);
		v.bits = vt_docId;
		vec_push(respval->valuePtr, v);
		abandonValue(keys);
	  }
	} else {
      docId.bits = allocObjId(docHndl->map, listFree(docHndl,0), listWait(docHndl,0), docArena->storeId);
	  keys = installKeys (args[0], idxHndls, docId, NULL);
	  resp.bits = vt_docId;
	  resp.docBits = insertDoc(idxHndls, args[0], keys, 0, NULL, docId, txnId);
	  abandonValue(keys);
	}

	for (int idx = 0; idx < vec_cnt(idxHndls); idx++)
		releaseHandle(idxHndls[idx], hndl);

	vec_free(idxHndls);
	return resp;
}

//	return the latest version of a document from a docStore by docId

value_t fcnStoreFetch(value_t *args, value_t *thisVal) {
	object_t *oval = js_addr(*thisVal);
	document_t *document;
	DbHandle *hndl;
	value_t v, s;
	ObjId docId;
	Doc *doc;

	hndl = (DbHandle *)oval->base->hndl;
	s.bits = vt_status;

	if (args->type != vt_docId) {
		fprintf(stderr, "Error: fetch => expecting docId => %s\n", strtype(args->type));
		return s.status = ERROR_script_internal, s;
	}

	docId.bits = args->docBits;

	if ((s.status = fetchDoc(hndl, &doc, docId)))
		return s;

	//	return highest version doc value

	v.bits = vt_document;
	v.addr = js_alloc(sizeof(document_t),true);
	v.refcount = true;

	document = v.addr;
	*document->hndl = hndl->hndlBits;
	document->ver = (Ver *)((uint8_t *)doc + doc->lastVer);
	return v;
}

//	convert DocId to string

value_t fcnDocIdToString(value_t *args, value_t *thisVal) {
	char buff[64];
	ObjId docId;
	int len;

	docId.bits = thisVal->docBits;

#ifndef _WIN32
	len = snprintf(buff, sizeof(buff), "%X:%X", docId.seg, docId.index);
#else
	len = _snprintf_s(buff, sizeof(buff), _TRUNCATE, "%X:%X", docId.seg, docId.index);
#endif
	return newString(buff, len);
}

//	display a document

value_t fcnDocToString(value_t *args, value_t *thisVal) {
	document_t *document = thisVal->addr;

	if (document->update->type)
		return conv2Str(*document->update, true, false);

	return conv2Str(*document->ver->rec, true, false);
}

//	return base value for a document version (usually a vt_document object)

value_t fcnDocValueOf(value_t *args, value_t *thisVal) {
	document_t *document = thisVal->addr;

	if (document->update->type)
		return *document->update;

	return *document->ver->rec;
}

//	return size of a document version

value_t fcnDocSize(value_t *args, value_t *thisVal) {
	document_t *document = thisVal->addr;
	value_t v;

	v.bits = vt_int;
	v.nval = document->ver->verSize;
	return v;
}

//	update a document in a docStore

value_t fcnDocUpdate(value_t *args, value_t *thisVal) {
	document_t *document = thisVal->addr;
	Handle *docHndl, **idxHndls;
	value_t resp, txn, s, keys;
	DbHandle *hndl;
	ObjId txnId;
	Ver *ver;

	ver = document->ver;
	Doc *doc = (Doc *)((uint8_t)ver - ver->offset - sizeof(Doc));

	s.bits = vt_status;
	resp.bits = vt_undef;
	txnId.bits = 0;

	//	any document updates done?

	if (!document->update->type)
		return resp;

	hndl = (DbHandle *)document->hndl;

	if (!(docHndl = bindHandle(hndl)))
		return s.status = DB_ERROR_handleclosed, s;

	idxHndls = bindDocIndexes(docHndl);

	keys = installKeys (*document->update, idxHndls, doc->docId, ver);

	if (vec_cnt(args)) {
	  if (args->type == vt_txn)
		txnId.bits = args->txnBits;
	}

	resp.bits = vt_docId;
	resp.docBits = updateDoc(idxHndls, document, keys, txnId);

	for (int idx = 0; idx < vec_cnt(idxHndls); idx++)
		releaseHandle(idxHndls[idx], hndl);

	vec_free(idxHndls);
	abandonValue(keys);
	return resp;
}

//	return the docId of a version

value_t propDocDocId(value_t val, bool lval) {
	document_t *document = val.addr;
	Ver *ver = document->ver;
	Doc *doc = (Doc *)((uint8_t)ver - ver->offset - sizeof(Doc));
	value_t v;

	v.bits = vt_docId;
	v.docBits = doc->docId.bits;
	return v;
}

PropFcn builtinDocIdFcns[] = {
	{ fcnDocIdToString, "toString" },
	{ NULL, NULL}
};

PropVal builtinDocIdProp[] = {
	{ NULL, NULL}
};

PropFcn builtinDocFcns[] = {
	{ fcnDocToString, "toString" },
	{ fcnDocValueOf, "valueOf" },
	{ fcnDocUpdate, "update" },
	{ fcnDocSize, "size" },
	{ NULL, NULL}
};

PropVal builtinDocProp[] = {
	{ propDocDocId, "docId" },
	{ NULL, NULL}
};

PropFcn builtinStoreFcns[] = {
	{ fcnStoreInsert, "insert" },
	{ fcnStoreFetch, "fetch" },
	{ NULL, NULL}
};

PropVal builtinStoreProp[] = {
	{ NULL, NULL}
};


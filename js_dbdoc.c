#include "js.h"
#include "js_props.h"
#include "js_string.h"

#include "js_db.h"
#include "js_dbindex.h"

#ifdef __linux__
#define offsetof(type,member) __builtin_offsetof(type,member)
#endif

extern void cloneObject(value_t *obj); 

//	delete a document reference

void deleteDocument(value_t val) {
	document_t *document = val.addr;

	releaseHandle(document->docHndl, NULL);
	deleteValue(*document->update);
	js_free(val.raw);
}

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
	value_t s, resp;
	DbHandle *hndl;
	ObjId txnId;
	ObjId docId;

	s.bits = vt_status;
	txnId.bits = 0;

	hndl = (DbHandle *)oval->base->hndl;

	if (!(docHndl = bindHandle(hndl)))
		return s.status = DB_ERROR_handleclosed, s;

	idxHndls = bindDocIndexes(docHndl);

	// multiple document/value case

	if (args[0].type == vt_array) {
	  resp = newArray(array_value);
	  array_t *respval = resp.addr;
	  array_t *aval = js_addr(args[0]);
	  value_t *values = args[0].marshaled ? aval->valueArray : aval->valuePtr;
	  uint32_t cnt = args[0].marshaled ? aval->cnt : vec_cnt(aval->valuePtr);

	  for (int idx = 0; idx < cnt; idx++) {
		value_t v;

		docId.bits = allocObjId(docHndl->map, listFree(docHndl,0), listWait(docHndl, 0));
		docId.xtra = docHndl->map->arenaDef->storeId;

		v.docBits = insertDoc(idxHndls, values[idx], 0, docId, txnId, NULL);
		v.bits = vt_docId;
		vec_push(respval->valuePtr, v);
	  }
	} else {
      docId.bits = allocObjId(docHndl->map, listFree(docHndl,0), listWait(docHndl, 0));
	  docId.xtra = docHndl->map->arenaDef->storeId;

	  resp.bits = vt_docId;
	  resp.docBits = insertDoc(idxHndls, args[0], 0, docId, txnId, NULL);
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
	Handle *docHndl;
	DbHandle *hndl;
	DbAddr *slot;
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
	hndl = (DbHandle *)oval->base->hndl;

	if (!(docHndl = bindHandle(hndl)))
		return s.status = DB_ERROR_handleclosed, s;

    slot = fetchIdSlot(docHndl->map, docId);
    doc = getObj(docHndl->map, *slot);

	//	return highest version doc value

	v.bits = vt_document;
	v.addr = js_alloc(sizeof(document_t),true);
	v.refcount = true;

	//  leave docHndl bound until the document_t is deleted

	document = v.addr;
	document->docHndl = docHndl;
	document->ver = findDocVer(docHndl->map, doc, NULL);

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
	value_t resp;
	ObjId txnId;

	resp.bits = vt_undef;
	txnId.bits = 0;

	//	any document updates done?

	if (!document->update->type)
		return resp;

	docHndl = document->docHndl;
	idxHndls = bindDocIndexes(docHndl);

	if (vec_cnt(args)) {
	  if (args->type == vt_txn)
		txnId.bits = args->txnBits;
	}

	resp.bits = vt_docId;
	resp.docBits = updateDoc(idxHndls, document, txnId);

	for (int idx = 1; idx < vec_cnt(idxHndls); idx++)
		releaseHandle(idxHndls[idx], NULL);

	vec_free(idxHndls);
	return resp;
}

//	return the docId of a version

value_t propDocDocId(value_t val, bool lval) {
	document_t *document = val.addr;
	Ver *ver = document->ver;
	Doc *doc = (Doc *)((uint8_t *)ver - ver->offset);
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

#include "js.h"
#include "js_props.h"
#include "js_string.h"

#include "js_db.h"
#include "js_dbindex.h"

#ifdef __linux__
#define offsetof(type,member) __builtin_offsetof(type,member)
#endif

extern CcMethod *cc;

//	make a new document reference

value_t makeDocument(Ver *ver, DbHandle hndl[1]) {
	Doc *doc = (Doc *)((uint8_t *)ver - ver->offset);
	document_t *document;
	value_t val;

	val.bits = vt_document;

	val.addr = js_alloc(sizeof(document_t),true);
	val.refcount = true;

	document = val.addr;
	document->ver = ver;
	*document->value = *ver->rec;
	document->addrBits = doc->ourAddr.bits;
	document->hndl->hndlBits = hndl->hndlBits;

	atomicAdd32(doc->refCnt, 1);
	return val;
}

void moveDocument(Ver *ver, document_t *document) {
	Doc *oldDoc = (Doc *)((uint8_t *)document->ver - document->ver->offset);
	Doc *newDoc = (Doc *)((uint8_t *)ver - ver->offset);

	if (oldDoc != newDoc) {
		atomicAdd32(oldDoc->refCnt, -1);
		atomicAdd32(newDoc->refCnt, 1);
	}

	deleteValue(*document->value);

	document->ver = ver;
	*document->value = *ver->rec;
	document->addrBits = newDoc->ourAddr.bits;
}
	
//	delete a document reference

void deleteDocument(value_t val) {
	document_t *document = val.addr;
//	Doc *doc = (Doc *)((uint8_t *)document->ver - document->ver->offset);

//	if (!atomicAdd32(doc->refCnt, -1))
//		if (doc->op & TYPE_mask == Delete)

	deleteValue(*document->value);
	js_free(val.raw);
}

//	return base value for a document version
//	or a cloned copy

value_t convDocument(value_t val, bool lVal) {
	document_t *document = val.addr;

	if (lVal) {
	  if (document->value->marshaled)
		cloneValue(document->value, false);
	}

	return *document->value;
}

//	document store Insert method
//	return docId, or array of docId

value_t fcnStoreInsert(value_t *args, value_t *thisVal, environment_t *env) {
	Handle *docHndl, **idxHndls;
	value_t resp, s;
	DbHandle *hndl;
	ObjId txnId;

	txnId.bits = *env->txnBits;
	s.bits = vt_status;

	hndl = (DbHandle *)baseObject(*thisVal)->hndl;

	if (!(docHndl = bindHandle(hndl)))
		return s.status = DB_ERROR_handleclosed, s;

	idxHndls = bindDocIndexes(docHndl);

	// multiple document/value case

	if (args[0].type == vt_array) {
	  dbarray_t *dbaval = js_addr(args[0]);
	  value_t *values = args[0].marshaled ? dbaval->valueArray : args[0].aval->valuePtr;
	  uint32_t cnt = args[0].marshaled ? dbaval->cnt : vec_cnt(values);
	  resp = newArray(array_value, cnt);
	  array_t *respval = resp.addr;

	  for (int idx = 0; idx < cnt; idx++) {
		Ver *ver = insertDoc(idxHndls, values[idx], 0, 0, txnId, NULL);

		if (jsError(ver))
			return s.status = (Status)ver, s;

		respval->valuePtr[idx] = makeDocument(ver, hndl);
	  }
	} else {
	  Ver *ver = insertDoc(idxHndls, args[0], NULL, 0, txnId, NULL);

	  if (jsError(ver))
		return s.status = (Status)ver, s;

	  resp = makeDocument(ver, hndl);
	}

	for (int idx = 0; idx < vec_cnt(idxHndls); idx++)
		releaseHandle(idxHndls[idx], hndl);

	vec_free(idxHndls);
	return resp;
}

//	convert DocId to string

value_t fcnDocIdToString(value_t *args, value_t *thisVal, environment_t *env) {
	char buff[64];
	ObjId docId;
	int len;

	docId.bits = thisVal->idBits;

#ifndef _WIN32
	len = snprintf(buff, sizeof(buff), "%X:%X", docId.seg, docId.idx);
#else
	len = _snprintf_s(buff, sizeof(buff), _TRUNCATE, "%X:%X", docId.seg, docId.idx);
#endif
	return newString(buff, len);
}

//	display a document

value_t fcnDocToString(value_t *args, value_t *thisVal, environment_t *env) {
	document_t *document = thisVal->addr;
	return conv2Str(*document->value, true, false);
}

//	return base value for a document version (usually a vt_document object)

value_t fcnDocValueOf(value_t *args, value_t *thisVal, environment_t *env) {
	document_t *document = thisVal->addr;
	return *document->value;
}

//	return size of a document version

value_t fcnDocSize(value_t *args, value_t *thisVal, environment_t *env) {
	document_t *document = thisVal->addr;
	value_t v;

	v.bits = vt_int;
	v.nval = document->ver->verSize;
	return v;
}

//	update a document

value_t fcnDocUpdate(value_t *args, value_t *thisVal, environment_t *env) {
	document_t *document = thisVal->addr;
	Handle **idxHndls, *docHndl;
	ObjId txnId;
	Ver *ver;
	value_t s;

	txnId.bits = *env->txnBits;

	if (!(docHndl = bindHandle(document->hndl)))
		return s.status = DB_ERROR_handleclosed, s;

	idxHndls = bindDocIndexes(docHndl);

	ver = updateDoc(idxHndls, document, txnId);
	s.bits = vt_status;

	if (jsError(ver))
		return s.status = (Status)ver, s;

	for (int idx = 0; idx < vec_cnt(idxHndls); idx++)
		releaseHandle(idxHndls[idx], NULL);

	vec_free(idxHndls);

	moveDocument(ver, document);
	return *thisVal;
}

//	return the docId of a version

value_t propDocDocId(value_t val, bool lval) {
	document_t *document = val.addr;
	Ver *ver = document->ver;
	Doc *doc = (Doc *)((uint8_t *)ver - ver->offset);
	value_t v;

	v.bits = vt_docId;
	v.idBits = doc->docId.bits;
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
	{ NULL, NULL}
};

PropVal builtinStoreProp[] = {
	{ NULL, NULL}
};


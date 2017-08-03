#include "js.h"
#include "js_props.h"
#include "js_string.h"

#include "js_db.h"
#include "js_dbindex.h"
#include <stddef.h>

extern CcMethod *cc;

void *js_dbaddr(value_t val, document_t *doc) {
	document_t *document;

	if ((document = val.addr))
		return document->base + val.offset;
	else if (doc)
		return doc->base + val.offset;

	fprintf(stderr, "Not document item: %s\n", strtype(val.type));
	exit(1);
}

value_t makeDocument(Ver *ver, DbHandle hndl[1]) {
	Doc *doc = (Doc *)((uint8_t *)ver - ver->offset);
	document_t *document;
	value_t val;

	val.bits = vt_document;

	val.addr = js_alloc(sizeof(document_t),true);
	val.refcount = true;

	document = val.addr;
	document->ver = ver;
	document->value = *ver->rec;

	if (ver->rec->marshaled)
		document->value.document = document;

	document->base = (uint8_t *)doc;
	document->hndl->hndlBits = hndl->hndlBits;

	atomicAdd32(doc->refCnt, 1);
	return val;
}

//	delete a document reference

void deleteDocument(value_t val) {
	document_t *document = val.document;
//	Doc *doc = (Doc *)((uint8_t *)document->ver - document->ver->offset);

//	if (!atomicAdd32(doc->refCnt, -1))
//		if (doc->op & TYPE_mask == Delete)

	deleteValue(document->value);
	js_free(val.raw);
}

//  convert document object to modifiable object

value_t convDocObject(value_t obj) {
	document_t *document = obj.document;

	if (obj.type == vt_document) {
	  if (document->value.marshaled)
		document->value = cloneValue(document->value);

	  obj = document->value;
	} else {
	  if (obj.marshaled)
		obj = cloneValue(obj);
	}

	incrRefCnt(obj);
	return obj;
}

// retrieve document object

value_t getDocObject(value_t doc) {
	document_t *document = doc.addr;
	return document->value;
}

//	clone marshaled array

value_t cloneArray(value_t obj, value_t doc) {
	dbarray_t *dbaval = (dbarray_t *)(doc.document->base + obj.offset);
	uint32_t cnt = dbaval->cnt, idx;
	value_t val = newArray(array_value, cnt + cnt / 4);

	for (idx = 0; idx < cnt; idx++) {
	  value_t element = dbaval->valueArray[idx];

	  if (element.marshaled) {
		element.addr = doc.document;
		incrRefCnt(doc);
	  }

	  val.aval->valuePtr[idx] = element;
	}

	return val;
}

value_t cloneObject(value_t obj, value_t doc) {
	dbobject_t *dboval = (dbobject_t *)(doc.document->base + obj.offset);
	value_t val = newObject(vt_object);
	pair_t *pairs = dboval->pairs;
	uint32_t cnt = dboval->cnt;
	uint32_t cap, hashEnt;
	value_t left, right;
	uint32_t h, idx;
	void *hashTbl;

	pair_t *newPair = newVector(cnt + cnt / 4, sizeof(pair_t), true);
	val.oval->pairsPtr = newPair;

	cap = vec_max(newPair);
	hashTbl = newPair + cap;
	hashEnt = hashBytes(cap);

	for (idx = 0; idx < cnt; idx++) {
	  left = pairs[idx].name;

	  if (left.marshaled) {
		left.addr = doc.document;
		incrRefCnt(doc);
	  }

	  h = -lookupValue(val, left, 0, false);
	  right = pairs[idx].value;

	  if (right.marshaled) {
		right.addr = doc.document;
		incrRefCnt(doc);
	  }

	  replaceSlot (&newPair[idx].name, left);
	  replaceSlot (&newPair[idx].value, right);
	  hashStore(hashTbl, hashEnt, h, idx + 1);
	}

	vec_size(newPair) = cnt;
	return val;
}
//	document store Insert method
//	return docId, or array of docId

value_t fcnStoreInsert(value_t *args, value_t thisVal, environment_t *env) {
	Handle *docHndl, **idxHndls;
	value_t resp, s;
	DbHandle *hndl;
	uint32_t idx;
	ObjId txnId;

	txnId.bits = *env->txnBits;
	s.bits = vt_status;
	s.status = 0;

	hndl = (DbHandle *)baseObject(thisVal)->hndl;

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

	  for (idx = 0; idx < cnt; idx++) {
		Ver *ver = insertDoc(idxHndls, values[idx], 0, 0, txnId, NULL, env->timestamp, NULL);

		if (jsError(ver)) {
			s.status = (Status)ver;
			break;
		}

		respval->valuePtr[idx] = makeDocument(ver, hndl);
	  }
	} else {
	  Ver *ver = insertDoc(idxHndls, args[0], NULL, 0, txnId, NULL, env->timestamp, NULL);

	  if (jsError(ver))
		s.status = (Status)ver;
	  else
	  	resp = makeDocument(ver, hndl);
	}

	for (idx = 0; idx < vec_cnt(idxHndls); idx++)
		releaseHandle(idxHndls[idx], hndl);

	vec_free(idxHndls);

	if (!s.status)
		return resp;

	deleteValue(resp);
	return s;
}

//	convert DocId to string

value_t fcnDocIdToString(value_t *args, value_t thisVal, environment_t *env) {
	char buff[64];
	ObjId docId;
	int len;

	docId.bits = thisVal.idBits;

#ifndef _WIN32
	len = snprintf(buff, sizeof(buff), "%X:%X", docId.seg, docId.idx);
#else
	len = _snprintf_s(buff, sizeof(buff), _TRUNCATE, "%X:%X", docId.seg, docId.idx);
#endif
	return newString(buff, len);
}

//	display a document

value_t fcnDocToString(value_t *args, value_t thisVal, environment_t *env) {
	document_t *document = thisVal.addr;
	return conv2Str(document->value, true, false);
}

//	return base value for a document version (usually a vt_document object)

value_t fcnDocValueOf(value_t *args, value_t thisVal, environment_t *env) {
	document_t *document = thisVal.addr;
	return document->value;
}

//	return size of a document version

value_t fcnDocSize(value_t *args, value_t thisVal, environment_t *env) {
	document_t *document = thisVal.addr;
	value_t v;

	v.bits = vt_int;
	v.nval = document->ver->verSize;
	return v;
}

//	update a document

value_t fcnDocUpdate(value_t *args, value_t thisVal, environment_t *env) {
	document_t *document = thisVal.addr;
	Handle **idxHndls, *docHndl;
	uint32_t idx;
	ObjId txnId;
	value_t s;
	Ver *ver;

	s.bits = vt_status;
	s.status = 0;

	txnId.bits = *env->txnBits;
	docHndl = bindHandle(document->hndl);

	idxHndls = bindDocIndexes(docHndl);

	ver = updateDoc(idxHndls, document, txnId, env->timestamp);

	//	update original document to new version

	if (jsError(ver))
	  s.status = (Status)ver;
	else
	  s = makeDocument(ver, document->hndl);

	for (idx = 0; idx < vec_cnt(idxHndls); idx++)
		releaseHandle(idxHndls[idx], NULL);

	vec_free(idxHndls);
	return s;
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


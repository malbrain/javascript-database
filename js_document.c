#include "js.h"
#include "js_props.h"
#include "js_string.h"

#include "database/db_api.h"
#include "database/db_map.h"
#include "database/db_object.h"
#include "database/db_handle.h"
#include "database/db_arena.h"

void marshalDoc(value_t document, uint8_t *doc, uint32_t offset, DbAddr addr, uint32_t docSize);

//	insert a document, or array of documents into a docStore

value_t fcnStoreInsert(value_t *args, value_t *thisVal) {
	object_t *oval = js_addr(*thisVal);
	Handle *docStore;
	value_t v, s;
	ObjId txnId;
	Doc *doc;
	int size;

	s.bits = vt_status;
	txnId.bits = 0;

	if (!(docStore = bindHandle((DbHandle *)oval->base->handle)))
		return s.status = DB_ERROR_handleclosed, s;

	// multiple document/value case

	if (args[0].type == vt_array) {
	  value_t resp = newArray(array_value);
	  array_t *respval = resp.addr;
	  array_t *aval = js_addr(args[0]);
	  value_t *values = args[0].marshaled ? aval->valueArray : aval->valuePtr;
	  uint32_t cnt = args[0].marshaled ? aval->cnt : vec_cnt(aval->valuePtr);

	  for (int idx = 0; idx < cnt; idx++) {
		size = calcSize(values[idx]);

		if ((s.status = allocDoc(docStore, &doc, 0)))
			return s;

		marshalDoc(values[idx], (uint8_t*)doc, sizeof(Doc), doc->addr, size);
		  
		if ((s.status = installDoc(docStore, doc, txnId)))
			return s;

		v.bits = vt_docId;
		v.docBits = doc->ver->docId.bits;
		vec_push(respval->valuePtr, v);
	  }

	  releaseHandle(docStore);
	  return resp;
	}

	// single document/value case

	if (!(docStore = bindHandle((DbHandle *)oval->base->handle)))
		return s.status = DB_ERROR_handleclosed, s;

	size = calcSize(args[0]);

	if ((s.status = allocDoc(docStore, &doc, size)))
		return s;

	marshalDoc(args[0], (uint8_t*)doc, sizeof(Doc), doc->addr, size);

	if ((s.status = installDoc(docStore, doc, true)))
		return s;

	releaseHandle(docStore);

	v.bits = vt_docId;
	v.docBits = doc->ver->docId.bits;
	return v;
}

//	return the latest version of a document from a docStore by docId

value_t fcnStoreFetch(value_t *args, value_t *thisVal) {
	object_t *oval = js_addr(*thisVal);
	document_t *document;
	DbHandle *hndl;
	value_t v, s;
	ObjId docId;
	Doc *doc;

	hndl = (DbHandle *)oval->base->handle;
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
	*document->handle = hndl->hndlBits;
	*document->addr = doc->addr.bits;
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
	len = snprintf(buff, sizeof(buff), "%X":%X, docId.seg, docId.index);
#else
	len = _snprintf_s(buff, sizeof(buff), _TRUNCATE, "%X:%X", docId.seg, docId.index);
#endif
	return newString(buff, len);
}

//	return base value for a document version (usually a vt_document object)

value_t fcnDocValueOf(value_t *args, value_t *thisVal) {
	document_t *document = thisVal->addr;
	value_t *slot = (value_t *)(document->ver + 1);
	return *slot;
}

//	return size of a document version

value_t fcnDocSize(value_t *args, value_t *thisVal) {
	document_t *document = thisVal->addr;
	value_t v;

	v.bits = vt_int;
	v.nval = document->ver->size;
	return v;
}

value_t propDocValueOf(value_t *args, value_t *thisVal) {
	document_t *document = thisVal->addr;
	value_t *slot = (value_t *)(document->ver + 1);
	return *slot;
}

PropFcn builtinDocIdFcns[] = {
	{ fcnDocIdToString, "toString" },
	{ NULL, NULL}
};

PropVal builtinDocIdProp[] = {
	{ NULL, NULL}
};

PropFcn builtinDocFcns[] = {
	{ fcnDocValueOf, "valueOf" },
	{ fcnDocSize, "size" },
	{ NULL, NULL}
};

PropVal builtinDocProp[] = {
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


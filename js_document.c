#include "js.h"
#include "js_props.h"
#include "js_string.h"
#include "js_dbindex.h"

DbAddr *buildKeys(Handle *docHndl, Handle *idxHndl, value_t document, ObjId docId, Ver *prevVer);
void marshalDoc(value_t document, uint8_t *doc, uint32_t offset, dbaddr_t addr, uint32_t docSize, value_t *val);
uint32_t calcSize (value_t doc);

//	return base value for a document version

value_t convDocument(value_t val) {
	document_t *document = val.addr;
	JsVersion *version = (JsVersion *)((Ver *)document->ver + 1);
	return *version->rec;
}

//	bind index DbHandles for document insert batch
//	returns a vector of index handles

Handle **bindDocIndexes(Handle *docHndl) {
	DocStore *docStore = (DocStore *)(docHndl + 1);
	Handle **idxHndls = NULL, *idxHndl;
	DbHandle dbHndl[1];
	SkipNode *skipNode;
	SkipEntry *entry;
	DbAddr *next;

	readLock (docStore->indexes->lock);
	next = docStore->indexes->head;

	while (next->addr) {
      skipNode = getObj(docHndl->map, *next);

	  for (int idx = 0; idx < next->nslot; idx++) {
      	entry = skipNode->array + idx;
		dbHndl->hndlBits = *entry->val;

		if ((idxHndl = bindHandle(dbHndl)))
		  vec_push(idxHndls, idxHndl);
      }

      next = skipNode->next;
	}

	readUnlock (docStore->indexes->lock);
	return idxHndls;
}

//	insert a document, or array of documents into a docStore

uint64_t insertDoc(Handle *docHndl, value_t document, Handle **idxHndls) {
	uint32_t keySize, docSize = calcSize(document);
	DocStore *docStore = (DocStore *)(docHndl + 1);
	DocArena *docArena = docarena(docHndl->map);
	value_t keys = newObject(vt_object);
	IndexKeyValue *keyValue;
	DbAddr addr, *slot;
	JsVersion *version;
	dbaddr_t dbAddr;
	ObjId docId;
	Doc *doc;

	// allocate the docId for the new document

    docId.bits = allocObjId(docHndl->map, listFree(docHndl,0), listWait(docHndl,0), docArena->storeId);

	// build object of index keys for this document

	for (int idx = 0; idx < vec_cnt(idxHndls); idx++) {
		DbAddr *list = buildKeys(docHndl, idxHndls[idx], document, docId, NULL);

		for (int i = 0; i < vec_cnt(list); i++) {
			IndexKeyValue *keyValue = getObj(docHndl->map, list[idx]);
			value_t name, *slot;

			name.bits = vt_string;
			name.offset = offsetof(IndexKeyValue, baseLen);
			name.addr = keyValue;
			name.marshaled = 1;

			slot = lookup(keys.addr, name, false, hashStr(keyValue->keyBytes, *keyValue->baseLen));
			atomicAdd64(keyValue->refCnt, 1);
			slot->bits = vt_key;
			slot->keyBits = list[idx].bits;
		}

		vec_free(list);
	}

	keySize = calcSize(keys);

    if ((addr.bits = dbAllocDocStore(docHndl, keySize + docSize + sizeof(Doc) + sizeof(JsVersion), false)))
        doc = getObj(docHndl->map, addr);
    else
        return 0;

	version = (JsVersion *)(doc + 1);

    memset (doc, 0, sizeof(Doc));
    doc->lastVer = sizeof(Doc) - sizeof(Ver);
    doc->addr.bits = addr.bits;
    doc->verCnt = 1;

    doc->ver->size = docSize + sizeof(JsVersion);
    doc->ver->offset = sizeof(Doc) - sizeof(Ver);
    doc->ver->docId.bits = docId.bits;
    doc->ver->version = 1;
 
	dbAddr.bits = addr.addr;
	dbAddr.storeId = docArena->storeId;

	marshalDoc(document, (uint8_t*)(doc + 1), sizeof(JsVersion), dbAddr, docSize, version->rec);
	marshalDoc(keys, (uint8_t*)(doc + 1), docSize + sizeof(JsVersion), dbAddr, keySize, version->keys);

	//	install the document
	//	and return docId

	slot = fetchIdSlot(docHndl->map, docId);
	slot->bits = addr.bits;
	return docId.bits;
}

//	document store Insert method

value_t fcnStoreInsert(value_t *args, value_t *thisVal) {
	object_t *oval = js_addr(*thisVal);
	Handle *docHndl, **idxHndls;
	value_t s, resp;
	ObjId txnId;
	DbAddr addr;
	Doc *doc;
	int size;

	s.bits = vt_status;
	txnId.bits = 0;

	if (!(docHndl = bindHandle((DbHandle *)oval->base->handle)))
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

		v.docBits = insertDoc(docHndl, values[idx], idxHndls);
		v.bits = vt_docId;
		vec_push(respval->valuePtr, v);
	  }
	} else {
	  resp.bits = vt_docId;
	  resp.docBits = insertDoc(docHndl, args[0], idxHndls);
	}

	releaseHandle(docHndl);

	for (int idx = 0; idx < vec_cnt(idxHndls); idx++)
		releaseHandle(idxHndls[idx]);

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
	len = snprintf(buff, sizeof(buff), "%X:%X", docId.seg, docId.index);
#else
	len = _snprintf_s(buff, sizeof(buff), _TRUNCATE, "%X:%X", docId.seg, docId.index);
#endif
	return newString(buff, len);
}

//	display a document

value_t fcnDocToString(value_t *args, value_t *thisVal) {
	document_t *document = thisVal->addr;
	JsVersion *version = (JsVersion *)((Ver *)document->ver + 1);

	return conv2Str(*version->rec, true, false);
}

//	return base value for a document version (usually a vt_document object)

value_t fcnDocValueOf(value_t *args, value_t *thisVal) {
	document_t *document = thisVal->addr;
	JsVersion *version = (JsVersion *)((Ver *)document->ver + 1);
	return *version->rec;
}

//	return size of a document version

value_t fcnDocSize(value_t *args, value_t *thisVal) {
	document_t *document = thisVal->addr;
	Ver *ver = (Ver *)document->ver;
	value_t v;

	v.bits = vt_int;
	v.nval = ver->size;
	return v;
}

//	return the docId of a version

value_t propDocDocId(value_t val, bool lval) {
	document_t *document = val.addr;
	Ver *ver = (Ver *)document->ver;
	value_t v;

	v.bits = vt_docId;
	v.docBits = ver->docId.bits;
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


#include "js.h"
#include "js_props.h"
#include "js_string.h"
#include "js_dbindex.h"

#ifdef __linux__
#define offsetof(type,member) __builtin_offsetof(type,member)
#endif

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

uint64_t insertDoc(Handle *docHndl, value_t val, Handle **idxHndls, ObjId txnId) {
	uint32_t keySize, docSize = calcSize(val, true);
	DocArena *docArena = docarena(docHndl->map);
	value_t keys = newObject(vt_object);
	object_t *oval = js_addr(val);
	DbAddr addr, *slot;
	JsVersion *version;
	dbaddr_t dbAddr;
	ObjId docId;
	Ver *ver;
	Doc *doc;

	// allocate the docId for the new document

    docId.bits = allocObjId(docHndl->map, listFree(docHndl,0), listWait(docHndl,0), docArena->storeId);

	// build object of index keys for this document

	if (val.type == vt_object)
	  for (int idx = 0; idx < vec_cnt(idxHndls); idx++) {
		DbAddr *list = buildKeys(docHndl, idxHndls[idx], oval, docId, NULL);

		for (int i = 0; i < vec_cnt(list); i++) {
			IndexKeyValue *keyValue = getObj(docHndl->map, list[i]);
			value_t name, *key;

			name.bits = vt_string;
			name.offset = offsetof(IndexKeyValue, keyLen);
			name.arenaAddr.storeId = docArena->storeId;
			name.arenaAddr.addr = list[i].addr;
			name.marshaled = 1;

			key = lookup(keys.addr, name, true, hashStr(keyValue->keyBytes, *keyValue->keyLen));
			atomicAdd64(keyValue->refCnt, 1);
			key->bits = vt_key;
			key->keyBits = list[i].bits;
		}

		vec_free(list);
	}

	keySize = calcSize(keys, true);

    if ((addr.bits = dbAllocDocStore(docHndl, keySize + docSize + sizeof(Doc) + sizeof(Ver) + sizeof(JsVersion), false)))
        doc = getObj(docHndl->map, addr);
    else
        return 0;

	ver = (Ver *)(doc + 1);
	version = (JsVersion *)(ver + 1);

    memset (doc, 0, sizeof(Doc) + sizeof(Ver));
    doc->lastVer = sizeof(Doc);
    doc->addr.bits = addr.bits;
    doc->verCnt = 1;

    ver->size = docSize + sizeof(JsVersion);
    ver->offset = sizeof(Doc);
    ver->docId.bits = docId.bits;
	ver->txnId.bits = txnId.bits;
    ver->version = 1;
 
	dbAddr.addr = addr.addr;
	dbAddr.storeId = docArena->storeId;

	marshalDoc(val, (uint8_t*)doc, sizeof(Doc) + sizeof(Ver) + sizeof(JsVersion), dbAddr, docSize, version->rec, true);
	marshalDoc(keys, (uint8_t*)doc, sizeof(Doc) + sizeof(Ver) + sizeof(JsVersion) + docSize, dbAddr, keySize, version->keys, true);

	//	install the document
	//	and return docId

	slot = fetchIdSlot(docHndl->map, docId);
	slot->bits = addr.bits;
	return docId.bits;
}

uint64_t updateDoc(Handle *docHndl, document_t *document, Handle **idxHndls, ObjId txnId) {
	object_t *oval = js_addr(*document->update);
	DocArena *docArena = docarena(docHndl->map);
	value_t keys = newObject(vt_object);
	uint32_t verSize, totSize;
	DbAddr addr, *slot;
	JsVersion *version;
	dbaddr_t dbAddr;
	ObjId docId;
	value_t v;
	Doc *doc;

    doc = (Doc *)((uint8_t)document->ver - document->ver->offset - sizeof(Doc));

	if (document->update->type == vt_object)
	  for (int idx = 0; idx < vec_cnt(idxHndls); idx++) {
		DbAddr *list = buildKeys(docHndl, idxHndls[idx], oval, docId, NULL);

		for (int i = 0; i < vec_cnt(list); i++) {
			IndexKeyValue *keyValue = getObj(docHndl->map, list[i]);
			value_t name, *key;

			name.bits = vt_string;
			name.offset = offsetof(IndexKeyValue, keyLen);
			name.arenaAddr.storeId = docArena->storeId;
			name.arenaAddr.addr = list[i].addr;
			name.marshaled = 1;

			key = lookup(keys.addr, name, true, hashStr(keyValue->keyBytes, *keyValue->keyLen));
			atomicAdd64(keyValue->refCnt, 1);
			key->bits = vt_key;
			key->keyBits = list[i].bits;
		}

		vec_free(list);
	}

	verSize = calcSize(*document->update, false);
	totSize = calcSize(keys, true) + verSize + sizeof(Ver) + sizeof(JsVersion);

	if (db_rawSize(doc->addr.bits) < totSize) {
	}
}

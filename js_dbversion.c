#include "js.h"
#include "js_db.h"
#include "js_dbindex.h"

#ifdef __linux__
#define offsetof(type,member) __builtin_offsetof(type,member)
#endif

//	insert a document into a docStore

uint64_t insertDoc(Handle **idxHndls, value_t val, value_t keys, uint64_t prevAddr, Ver *prevVer, ObjId docId, ObjId txnId) {
	uint32_t keySize, docSize = calcSize(val, true);
	DocArena *docArena = docarena(idxHndls[0]->map);
	object_t *oval = js_addr(val);
	DbAddr addr, *slot;
	dbaddr_t dbAddr;
	Ver *ver;
	Doc *doc;

	// build object of index keys for this document

	if (val.type == vt_object)
	  for (int idx = 1; idx < vec_cnt(idxHndls); idx++) {
		DbAddr *list = buildKeys(idxHndls[0], idxHndls[idx], oval, docId, NULL);

		for (int i = 0; i < vec_cnt(list); i++) {
			IndexKeyValue *keyValue = getObj(idxHndls[0]->map, list[i]);
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

    if ((addr.bits = dbAllocDocStore(idxHndls[0], keySize + docSize + sizeof(Doc) + sizeof(Ver), false)))
        doc = getObj(idxHndls[0]->map, addr);
    else
        return 0;

	ver = (Ver *)(doc + 1);

    memset (doc, 0, sizeof(Doc) + sizeof(Ver));
    doc->prevAddr.bits = prevAddr;
    doc->docAddr.bits = addr.bits;
	doc->docId.bits = docId.bits;
    doc->lastVer = sizeof(Doc);

    ver->version = prevVer ? prevVer->version + 1 : 1;
	ver->txnId.bits = txnId.bits;
    ver->offset = sizeof(Doc);
    ver->verSize = docSize;

	dbAddr.addr = addr.addr;
	dbAddr.storeId = docArena->storeId;

	marshalDoc(val, (uint8_t*)doc, sizeof(Doc) + sizeof(Ver), dbAddr, docSize, ver->rec, true);
	marshalDoc(keys, (uint8_t*)doc, sizeof(Doc) + sizeof(Ver) + docSize, dbAddr, keySize, ver->keys, true);

	//	install the document
	//	and return docId

	slot = fetchIdSlot(idxHndls[0]->map, docId);
	slot->bits = addr.bits;
	return docId.bits;
}

uint64_t updateDoc(Handle **idxHndls, document_t *document, value_t keys, ObjId txnId) {
	uint32_t verSize, totSize, keySize;
	Ver *prevVer, *newVer;
	DocArena *docArena;
	DbAddr addr, *slot;
	dbaddr_t dbAddr;
	Doc *doc;

	docArena = docarena(idxHndls[0]->map);

    doc = (Doc *)((uint8_t)document->ver - document->ver->offset - sizeof(Doc));
	prevVer = document->ver;

	keySize = calcSize(keys, true);
	verSize = calcSize(*document->update, false);
	totSize = keySize + verSize + sizeof(Ver);

	//	start over if not enough room for the version in the set

	if (db_rawSize(doc->docAddr.bits) < totSize)
		return insertDoc(idxHndls, *document->update, keys, doc->docAddr.bits, prevVer, doc->docId, txnId);

	verSize = calcSize(*document->update, true);
	totSize = keySize + verSize + sizeof(Ver);

	newVer = (Ver *)((uint8_t *)doc + doc->lastVer);
    newVer->version = prevVer ? prevVer->version + 1 : 1;
	newVer->txnId.bits = txnId.bits;
    newVer->offset = doc->lastVer;
    newVer->verSize = totSize;

	//	install the update

    doc->lastVer = doc->lastVer + totSize;
	return doc->docId.bits;
}

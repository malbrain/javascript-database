#include "js.h"
#include "js_db.h"
#include "js_dbindex.h"

#ifdef __linux__
#define offsetof(type,member) __builtin_offsetof(type,member)
#endif

//	insert a document into a docStore

uint64_t insertDoc(Handle **idxHndls, value_t val, value_t keys, uint64_t prevAddr, Ver *prevVer, ObjId docId, ObjId txnId) {
	uint32_t keySize, docSize = calcSize(val, true), rawSize;
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

	rawSize = db_rawSize(addr.bits);

    memset (doc, 0, sizeof(Doc));
    doc->lastVer = rawSize - sizeof(Ver) - sizeof(struct VerEnd) - docSize - keySize;
    doc->prevAddr.bits = prevAddr;
    doc->docAddr.bits = addr.bits;
	doc->docId.bits = docId.bits;

	//	fill-in end of area

	ver = (Ver *)((uint8_t *)doc + rawSize - sizeof(struct VerEnd));
    ver->version = prevVer ? prevVer->version : 0;
    ver->offset = rawSize - sizeof(struct VerEnd);
    ver->verSize = 0;

	ver = (Ver *)((uint8_t *)doc + doc->lastVer);
    ver->verSize = sizeof(Ver) + keySize + docSize;
    ver->version = prevVer ? prevVer->version + 1 : 1;
    ver->offset = doc->lastVer;

	if (txnId.bits)
		ver->txnId.bits = txnId.bits;
	else
		ver->timestamp = allocateTimestamp(idxHndls[0]->map->db, en_writer);

	dbAddr.addr = addr.addr;
	dbAddr.storeId = docArena->storeId;

	marshalDoc(val, (uint8_t*)doc, doc->lastVer + sizeof(Ver), dbAddr, docSize, ver->rec, true);
	marshalDoc(keys, (uint8_t*)doc, doc->lastVer + sizeof(Ver) + docSize, dbAddr, keySize, ver->keys, true);

	//	install the document
	//	and return docId

	//  TODO: resolve timestamps re: new reader > ourTs

	slot = fetchIdSlot(idxHndls[0]->map, docId);
	slot->bits = addr.bits;
	return docId.bits;
}

uint64_t updateDoc(Handle **idxHndls, document_t *document, value_t keys, ObjId txnId) {
	uint32_t docSize, totSize, keySize, offset;
	Ver *prevVer, *newVer;
	DocArena *docArena;
	DbAddr addr, *slot;
	dbaddr_t dbAddr;
	Doc *doc;

	docArena = docarena(idxHndls[0]->map);

    doc = (Doc *)((uint8_t *)document->ver - document->ver->offset);
	prevVer = document->ver;

	keySize = calcSize(keys, true);
	docSize = calcSize(*document->update, false);
	totSize = keySize + docSize + sizeof(Ver);

	//	start over if not enough room for the version in the set

	if (totSize > doc->lastVer - sizeof(Doc))
		return insertDoc(idxHndls, *document->update, keys, doc->docAddr.bits, prevVer, doc->docId, txnId);

	docSize = calcSize(*document->update, true);
	totSize = keySize + docSize + sizeof(Ver);
	offset = doc->lastVer - totSize;

	newVer = (Ver *)((uint8_t *)doc + offset);
    newVer->version = prevVer ? prevVer->version + 1 : 1;
	newVer->txnId.bits = txnId.bits;
    newVer->verSize = totSize;
    newVer->offset = offset;

	//	install the update

    doc->lastVer = offset;
	return doc->docId.bits;
}

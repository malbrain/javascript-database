#include "js.h"
#include "js_db.h"
#include "js_dbindex.h"

#ifdef __linux__
#define offsetof(type,member) __builtin_offsetof(type,member)
#endif

//	insert a document into a docStore

uint64_t insertDoc(Handle **idxHndls, value_t val, uint64_t prevAddr, ObjId docId, ObjId txnId, Ver *prevVer) {
	uint32_t docSize = calcSize(val, true), rawSize;
	DbAddr addr, *slot, keys[1];
	Ver *ver;
	Doc *doc;

	// build object of index keys for this document

	keys->bits = 0;

	if (val.type == vt_object)
	  for (int idx = 1; idx < vec_cnt(idxHndls); idx++)
		buildKeys(idxHndls[0], idxHndls[idx], val, keys, docId, prevVer, vec_cnt(idxHndls));

    if ((addr.bits = dbAllocDocStore(idxHndls[0], docSize + sizeof(Doc) + sizeof(Ver), false)))
        doc = getObj(idxHndls[0]->map, addr);
    else
        return 0;

	rawSize = db_rawSize(addr.bits);

    memset (doc, 0, sizeof(Doc));
    doc->lastVer = rawSize - sizeof(Ver) - offsetof(Ver, txnId) - docSize;
	doc->pending = prevAddr ? TxnUpdate : TxnInsert;
    doc->verNo = prevVer ? prevVer->verNo + 1 : 1;
    doc->prevAddr.bits = prevAddr;
    doc->docAddr.bits = addr.bits;
	doc->docId.bits = docId.bits;

	//	fill-in stopper for version array

	ver = (Ver *)((uint8_t *)doc + rawSize - offsetof(Ver, txnId));
    ver->offset = rawSize - offsetof(Ver, txnId);
    ver->verSize = 0;

	//	fill-in new version

	ver = (Ver *)((uint8_t *)doc + doc->lastVer);
    ver->verSize = sizeof(Ver) + docSize;
	ver->keys->bits = keys->bits;
    ver->offset = doc->lastVer;
    ver->verNo = doc->verNo;

	if (txnId.bits)
		ver->txnId.bits = txnId.bits;
	else
		ver->timestamp = allocateTimestamp(idxHndls[0]->map->db, en_writer);

	addr.xtra = idxHndls[0]->map->arenaDef->storeId;

	marshalDoc(val, (uint8_t*)doc, doc->lastVer + sizeof(Ver), addr, docSize, ver->rec, true);

	//	add updated document to current txn

	if (txnId.bits)
		addDocToTxn(idxHndls[0]->map->db, fetchIdSlot(idxHndls[0]->map->db, txnId), docId);

	//	install the document
	//	and return docId

	slot = fetchIdSlot(idxHndls[0]->map, docId);
	slot->bits = addr.bits;
	return docId.bits;
}

uint64_t updateDoc(Handle **idxHndls, document_t *document, ObjId txnId) {
	uint32_t docSize, totSize, offset;
	Ver *prevVer, *newVer;
	Doc *newDoc, *oldDoc;
	DbAddr *docSlot;
	DbAddr keys[1];
	DbAddr addr;

	keys->bits = 0;

    newDoc = (Doc *)((uint8_t *)document->ver - document->ver->offset);
	prevVer = document->ver;

	docSlot = fetchIdSlot(idxHndls[0]->map, newDoc->docId);
	lockLatch(docSlot->latch);

	oldDoc = getObj(idxHndls[0]->map, *docSlot);

	if (oldDoc->pending || oldDoc->verNo != prevVer->verNo) {
		unlockLatch(docSlot->latch);
		return 0;
	}

	docSize = calcSize(*document->update, false);
	totSize = docSize + sizeof(Ver);

	//	start over if not enough room for the version in the set

	if (totSize + sizeof(Doc) > newDoc->lastVer)
		return insertDoc(idxHndls, *document->update, newDoc->docAddr.bits, newDoc->docId, txnId, prevVer);

	if (document->update->type == vt_object)
	  for (int idx = 1; idx < vec_cnt(idxHndls); idx++)
		buildKeys(idxHndls[0], idxHndls[idx], *document->update, keys, newDoc->docId, prevVer, vec_cnt(idxHndls));

	offset = newDoc->lastVer - totSize;

	newVer = (Ver *)((uint8_t *)newDoc + offset);
	newVer->txnId.bits = txnId.bits;
	newVer->keys->bits = keys->bits;
    newVer->verSize = totSize;
    newVer->offset = offset;

	//	build and install the update

	addr.addr = newDoc->docAddr.addr;
	addr.xtra = idxHndls[0]->map->arenaDef->storeId;

	marshalDoc(*document->update, (uint8_t*)newDoc, offset + sizeof(Ver), addr, docSize, newVer->rec, false);

	if (txnId.bits)
		addDocToTxn(idxHndls[0]->map->db, fetchIdSlot(idxHndls[0]->map->db, txnId), oldDoc->docId);

	//  install new version
	//	and unlock docId slot

	newDoc->pending = TxnUpdate;
    newDoc->lastVer = offset;

	unlockLatch(docSlot->latch);
	return newDoc->docId.bits;
}

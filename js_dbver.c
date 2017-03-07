#include "js.h"
#include "js_db.h"
#include "js_dbindex.h"

#ifdef __linux__
#define offsetof(type,member) __builtin_offsetof(type,member)
#endif

extern CcMethod *cc;

//	insert an uncommitted document or an updated version into a docStore
//	if update, call with docId slot locked.
//	returns pointer to the new document

void *insertDoc(Handle **idxHndls, value_t val, uint64_t prevAddr, uint64_t docBits, ObjId txnId, Ver *prevVer) {
	uint32_t docSize = calcSize(val, true), rawSize;
	Handle *docHndl = idxHndls[0];
	DbAddr addr, *slot, keys[1];
	JsStatus stat;
	ObjId docId;
	Ver *ver;
	Doc *doc;

	//	assign a new docId slot if inserting

	if (!(docId.bits = docBits)) {
		docId.bits = allocObjId(docHndl->map, listFree(docHndl,0), listWait(docHndl, 0));
		docId.xtra = docHndl->map->arenaDef->storeId;
	}

	// build object of index keys for this document

	keys->bits = 0;

	if (val.type == vt_object)
	  for (int idx = 1; idx < vec_cnt(idxHndls); idx++)
		buildKeys(idxHndls, idx, val, keys, docId, prevVer, vec_cnt(idxHndls));

	//	allocate space in docStore for the version

    if ((addr.bits = allocDocStore(docHndl, docSize + sizeof(Doc) + sizeof(Ver) + offsetof(Ver, rec), false)))
		rawSize = db_rawSize(addr.bits);
    else
        return (JsStatus)ERROR_outofmemory;

	//  discard type and replace with storeId

	addr.xtra = docHndl->map->arenaDef->storeId;

	//	set up the document header

    doc = getObj(docHndl->map, addr);
    memset (doc, 0, sizeof(Doc));

    doc->lastVer = rawSize - sizeof(Ver) - offsetof(Ver, rec) - docSize;
	assert(doc->lastVer >= sizeof(Doc));

    doc->prevAddr.bits = prevAddr;
	doc->docId.bits = docId.bits;
	doc->txnId.bits = txnId.bits;

	if (txnId.bits)
		doc->pending = prevAddr ? TxnUpdate : TxnInsert;

	//	fill-in stopper (verSize == 0) at end of version array

	ver = (Ver *)((uint8_t *)doc + rawSize - offsetof(Ver, rec));
    ver->offset = rawSize - offsetof(Ver, rec);
    ver->verSize = 0;

	//	fill-in new version

	ver = (Ver *)((uint8_t *)doc + doc->lastVer);
    memset (ver, 0, sizeof(Ver));

    ver->verNo = prevVer ? prevVer->verNo : 1;

	if (prevVer && prevVer->commitTs)
		ver->verNo++;

    ver->verSize = sizeof(Ver) + docSize;
	ver->keys->bits = keys->bits;
    ver->offset = doc->lastVer;

	marshalDoc(val, (uint8_t*)doc, doc->lastVer + sizeof(Ver), addr, docSize, ver->rec, true);

	//  install the document version keys

	if ((stat = installKeys(idxHndls, ver)))
		return stat;

	//	add updated document to current txn
	//	inserts w/o txn get current ts

	if (txnId.bits)
		addDocWrToTxn(txnId, docId);
	else
		ver->commitTs = getTimestamp(true);

	//	install the document
	//	and return pointer to docId slot

	slot = fetchIdSlot(docHndl->map, docId);
	slot->bits = addr.bits;
	return doc;
}

//	update document
//	return error, or pointer to the document

void *updateDoc(Handle **idxHndls, document_t *document, ObjId txnId) {
	uint32_t docSize, totSize, offset;
	Handle *docHndl = idxHndls[0];
	Ver *prevVer = document->ver;
	Ver *newVer, *curVer;
	Doc *newDoc, *curDoc;
	DbAddr *docSlot;
	DbAddr keys[1];
	JsStatus stat;
	DbAddr addr;

	keys->bits = 0;

	//	newDoc is the document used to create the new update version

    newDoc = (Doc *)((uint8_t *)document->ver - document->ver->offset);

	//	grab and latch the current document in the docId slot

	docSlot = fetchIdSlot(docHndl->map, newDoc->docId);
	lockLatch(docSlot->latch);

	//	grab the current document at the docId slot

	curDoc = getObj(docHndl->map, *docSlot);
	curVer = (Ver *)((uint8_t *)curDoc + curDoc->lastVer);
	assert (newDoc->docId.bits == curDoc->docId.bits);

	//  is there a txn pending on this document?
	//	if its ours, rollback our previous version

	if (curDoc->pending) {
	  if (curDoc->txnId.bits != txnId.bits) {
		unlockLatch(docSlot->latch);
		return (JsStatus)ERROR_write_conflict;
	  }
	}

	docSize = calcSize(*document->update, false);
	totSize = docSize + sizeof(Ver);

	//	start over in a new version set
	//	if not enough room

	if (totSize + sizeof(Doc) > curDoc->lastVer) {
	  stat = insertDoc(idxHndls, *document->update, docSlot->bits, curDoc->docId.bits, txnId, prevVer);
	  unlockLatch(docSlot->latch);
	  return stat;
	}

	if (document->update->type == vt_object)
	  for (int idx = 1; idx < vec_cnt(idxHndls); idx++)
		buildKeys(idxHndls, idx, *document->update, keys, curDoc->docId, prevVer, vec_cnt(idxHndls));

	offset = curDoc->lastVer - totSize;

	newVer = (Ver *)((uint8_t *)curDoc + offset);
    memset (newVer, 0, sizeof(Ver));

	newVer->keys->bits = keys->bits;
    newVer->verSize = totSize;
    newVer->offset = offset;

	//	build and install the update

	addr.addr = docSlot->addr;
	addr.xtra = docHndl->map->arenaDef->storeId;

	marshalDoc(*document->update, (uint8_t*)curDoc, offset + sizeof(Ver), addr, docSize, newVer->rec, false);

	//  install the version keys

	if ((stat = installKeys(idxHndls, newVer)))
		return stat;

	if ((newDoc->txnId.bits = txnId.bits)) {
		newDoc->pending = TxnUpdate;
		addDocWrToTxn(txnId, curDoc->docId);
	} else
		newVer->commitTs = getTimestamp(true);

	//  install new version
	//	and unlock docId slot

    curDoc->lastVer = offset;

	unlockLatch(docSlot->latch);
	return curDoc;
}

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

JsStatus insertDoc(Handle **idxHndls, value_t val, DbAddr *docSlot, uint64_t docBits, ObjId txnId, Ver *prevVer) {
	uint32_t docSize = calcSize(val, true), rawSize;
	Handle *docHndl = idxHndls[0];
	DbAddr addr, docAddr, keys[1];
	JsStatus stat;
	ObjId docId;
	Ver *ver;
	Doc *doc;

	//	assign a new docId slot if inserting

	if (!(docId.bits = docBits)) {
		docId.bits = allocObjId(docHndl->map, listFree(docHndl,0), listWait(docHndl, 0));
		docId.xtra = docHndl->map->arenaDef->storeId;
		docSlot = fetchIdSlot(docHndl->map, docId);
	}

	// build object of index keys for this document

	keys->bits = 0;

	if (val.type == vt_object)
	  for (int idx = 1; idx < vec_cnt(idxHndls); idx++)
		buildKeys(idxHndls, idx, val, keys, docId, prevVer, vec_cnt(idxHndls));

	if (prevVer)
		rawSize = db_rawSize(docSlot->bits);
	else
		rawSize = docSize + sizeof(Doc) + sizeof(Ver) + offsetof(Ver, rec);

	if (rawSize < 12 * 1024 * 1024)
		rawSize += rawSize / 2;

	//	allocate space in docStore for the version

    if ((addr.bits = allocDocStore(docHndl, rawSize, false)))
		rawSize = db_rawSize(addr.bits);
    else
        return (JsStatus)ERROR_outofmemory;

	//  discard type and replace with storeId

	docAddr.bits = addr.bits;
	docAddr.xtra = docHndl->map->arenaDef->storeId;

	//	set up the document header

    doc = getObj(docHndl->map, addr);
    memset (doc, 0, sizeof(Doc));

    doc->lastVer = rawSize - sizeof(Ver) - offsetof(Ver, rec) - docSize;
	assert(doc->lastVer >= sizeof(Doc));

    doc->prevAddr.bits = docSlot->bits;
	doc->ourAddr.bits = addr.bits;
	doc->docId.bits = docId.bits;
	doc->txnId.bits = txnId.bits;

	if (txnId.bits)
		doc->op = prevVer ? Update : Insert;

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

	marshalDoc(val, (uint8_t*)doc, doc->lastVer + sizeof(Ver), docAddr, docSize, ver->rec, true);

	//  install the document version keys

	if ((stat = installKeys(idxHndls, ver)))
		return stat;

	//	add updated document to current txn
	//	inserts w/o txn get current ts

	if (txnId.bits)
		addDocWrToTxn(txnId, docId);
	else
		ver->commitTs = getSnapshotTimestamp(NULL, true);

	//	install the document
	//	and return new version

	docSlot->bits = addr.bits;
	return ver;
}

//	update document
//	return error or version

JsStatus updateDoc(Handle **idxHndls, document_t *document, ObjId txnId) {
	uint32_t docSize, totSize, offset;
	Ver *newVer, *curVer, *prevVer;
	Handle *docHndl = idxHndls[0];
	Doc *prevDoc, *curDoc;
	DbAddr *docSlot;
	DbAddr keys[1];
	JsStatus stat;
	DbAddr addr;

	keys->bits = 0;

	//	prevDoc is the document that was used to create the new version

    prevDoc = (Doc *)((uint8_t *)document->ver - document->ver->offset);
	prevVer = document->ver;

	//	grab and latch the current document in the docId slot

	docSlot = fetchIdSlot(docHndl->map, prevDoc->docId);
	lockLatch(docSlot->latch);

	//	grab the current document at the docId slot
	//	curDoc is the existing current document

	curDoc = getObj(docHndl->map, *docSlot);
	curVer = (Ver *)((uint8_t *)curDoc + curDoc->lastVer);

	//  is there a txn pending on this document?
	//	if its not ours, issue write_conflict error

	if (curDoc->op) {
	  if (curDoc->txnId.bits != txnId.bits) {
		unlockLatch(docSlot->latch);
		return (JsStatus)ERROR_write_conflict;
	  }
	} else if (prevVer != curVer) {
		unlockLatch(docSlot->latch);
		return (JsStatus)ERROR_write_conflict;
	}

	docSize = calcSize(*document->value, false);
	totSize = docSize + sizeof(Ver);

	//	start over in a new version set
	//	if not enough room

	if (totSize + sizeof(Doc) > curDoc->lastVer) {
	  stat = insertDoc(idxHndls, *document->value, docSlot, curDoc->docId.bits, txnId, prevVer);
	  unlockLatch(docSlot->latch);
	  return stat;
	}

	if (document->value->type == vt_object)
	  for (int idx = 1; idx < vec_cnt(idxHndls); idx++)
		buildKeys(idxHndls, idx, *document->value, keys, curDoc->docId, prevVer, vec_cnt(idxHndls));

	offset = curDoc->lastVer - totSize;

	newVer = (Ver *)((uint8_t *)curDoc + offset);
    memset (newVer, 0, sizeof(Ver));

	newVer->keys->bits = keys->bits;
    newVer->verSize = totSize;
    newVer->offset = offset;

	//	build and install the update

	addr.addr = docSlot->addr;
	addr.xtra = docHndl->map->arenaDef->storeId;

	marshalDoc(*document->value, (uint8_t*)curDoc, offset + sizeof(Ver), addr, docSize, newVer->rec, false);

	//  install the version keys

	if ((stat = installKeys(idxHndls, newVer)))
		return stat;

	if ((curDoc->txnId.bits = txnId.bits)) {
		curDoc->op = Update;
		addDocWrToTxn(txnId, curDoc->docId);
	} else
		newVer->commitTs = getSnapshotTimestamp(NULL, true);

	//  install new version
	//	and unlock docId slot

    curDoc->lastVer = offset;

	unlockLatch(docSlot->latch);
	return newVer;
}

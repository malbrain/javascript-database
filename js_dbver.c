#include "js.h"
#include "js_db.h"
#include "js_dbindex.h"

#include <stddef.h>

extern CcMethod *cc;

//	insert an uncommitted document or an updated version into a docStore
//	if update, call with docId slot locked.
//	returns pointer to the new document

JsStatus insertDoc(Handle **idxHndls, value_t val, DbAddr *docSlot, uint64_t docBits, ObjId txnId, Ver *prevVer, Timestamp *tsGen, uint8_t *src) {
	uint32_t verSize, docSize, rawSize;
	Handle *docHndl = idxHndls[0];
	DbAddr docAddr, keys[1];
	JsStatus stat;
	ObjId docId;
	Ver *ver;
	Doc *doc;

	docSize = calcSize(val, true, src);
	//	assign a new docId slot if inserting

	if (!(docId.bits = docBits)) {
		docId.bits = allocObjId(docHndl->map, listFree(docHndl,0), listWait(docHndl, 0));
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

    if ((docAddr.bits = allocDocStore(docHndl, rawSize, false)))
		rawSize = db_rawSize(docAddr.bits);
    else
        return (JsStatus)ERROR_outofmemory;

	//	set up the document header

    doc = getObj(docHndl->map, docAddr);
    memset (doc, 0, sizeof(Doc));

    doc->prevAddr.bits = docSlot->bits;
	doc->ourAddr.bits = docAddr.bits;
	doc->docId.bits = docId.bits;
	doc->txnId.bits = txnId.bits;

	if (txnId.bits)
		doc->op = prevVer ? Update : Insert;

	//	fill-in stopper (verSize == 0) at end of version array

	verSize = sizeof(Ver) - offsetof(Ver,rec);
	verSize += 15;
	verSize &= -16;

	ver = (Ver *)((uint8_t *)doc + rawSize - verSize);
    ver->offset = rawSize - verSize;
    ver->verSize = 0;

    doc->lastVer = ver->offset;

	verSize = sizeof(Ver) + docSize;
	verSize += 15;
	verSize &= -16;

    doc->lastVer -= verSize;
	assert(doc->lastVer >= sizeof(Doc));

	//	fill-in new version

	ver = (Ver *)((uint8_t *)doc + doc->lastVer);
    memset (ver, 0, sizeof(Ver));

    ver->verNo = prevVer ? prevVer->verNo : 1;

	if (prevVer && prevVer->commit)
		ver->verNo++;

	ver->keys->bits = keys->bits;
    ver->offset = doc->lastVer;
    ver->verSize = verSize;

	marshalDoc(val, (uint8_t*)doc, doc->lastVer + sizeof(Ver), verSize - sizeof(Ver), ver->rec, true, src);

	//  install the document version keys

	if ((stat = installKeys(idxHndls, ver)))
		return stat;

	//	add updated document to current txn
	//	inserts w/o txn get current ts

	if (txnId.bits)
		addDocWrToTxn(txnId, docId, ver, NULL, docHndl->hndlId.bits);
	else
		newTs (ver->commit, tsGen, false);

	//	install the document
	//	and return new version

	docSlot->bits = docAddr.bits;
	return ver;
}

//	update document
//	return error or version

JsStatus updateDoc(Handle **idxHndls, document_t *document, ObjId txnId, Timestamp *tsGen) {
	uint32_t docSize, offset, verSize;
	Ver *newVer, *curVer, *prevVer;
	Handle *docHndl = idxHndls[0];
	bool fullClone = false;
	Doc *prevDoc, *curDoc;
	DbAddr *docSlot;
	DbAddr keys[1];
	JsStatus stat;

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

	fullClone = prevDoc != curDoc;
	docSize = calcSize(document->value, fullClone, document->base);
	verSize = docSize + sizeof(Ver);

	verSize += 15;
	verSize &= -16;

	//	start over in a new version set
	//	if not enough room

	if (verSize + sizeof(Doc) > curDoc->lastVer) {
	  stat = insertDoc(idxHndls, document->value, docSlot, curDoc->docId.bits, txnId, prevVer, tsGen, document->base);
	  unlockLatch(docSlot->latch);
	  return stat;
	}

	if (document->value.type == vt_object)
	  for (int idx = 1; idx < vec_cnt(idxHndls); idx++)
		buildKeys(idxHndls, idx, document->value, keys, curDoc->docId, prevVer, vec_cnt(idxHndls));

	offset = curDoc->lastVer - verSize;

	newVer = (Ver *)((uint8_t *)curDoc + offset);
    memset (newVer, 0, sizeof(Ver));

    newVer->verNo = prevVer ? prevVer->verNo : 1;

	if (prevVer && prevVer->commit)
		newVer->verNo++;

	newVer->keys->bits = keys->bits;
    newVer->verSize = verSize;
    newVer->offset = offset;

	marshalDoc(document->value, (uint8_t*)curDoc, offset + sizeof(Ver), verSize - sizeof(Ver), newVer->rec, fullClone, document->base);

	//  install the version keys

	if ((stat = installKeys(idxHndls, newVer)))
		return stat;

	if ((curDoc->txnId.bits = txnId.bits)) {
		curDoc->op = Update;
		addDocWrToTxn(txnId, curDoc->docId, newVer, prevVer, docHndl->hndlId.bits);
	} else
		newTs (newVer->commit, tsGen, false);

	//  install new version
	//	and unlock docId slot

    curDoc->lastVer = offset;

	unlockLatch(docSlot->latch);
	return newVer;
}

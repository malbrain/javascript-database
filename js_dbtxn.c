#include "js.h"
#include "js_db.h"
#include "js_dbindex.h"
#include "database/db_frame.h"

extern Handle **arenaHandles;
extern DbMap memMap[1];
CcMethod *cc;

//  Txn arena free txn frames

DbAddr txnFree[1], txnWait[1];
DbArena txnArena[1];
DbMap txnMap[1];
bool txnInit;

//	initialize transaction database

//  the current system timestamp is
//	the highest committed transaction
//  timestamp, and is always odd.

//  uncommitted transactions start out
//  with the system timestamp + 1
//  which is incremented at commit.

void initTxn(void) {
ArenaDef arenaDef[1];

	txnMap->arena = txnArena;
	txnMap->arena->nxtTs = 1;
	txnMap->db = txnMap;

#ifdef _WIN32
	txnMap->hndl = INVALID_HANDLE_VALUE;
#else
	txnMap->hndl = -1;
#endif

	//	set up memory arena and handle addr ObjId

	memset (arenaDef, 0, sizeof(arenaDef));
	arenaDef->objSize = sizeof(Txn);

	initArena(txnMap, arenaDef, "_txn", 4, NULL);
	txnInit = true;
}

//	fetch and lock txn

Txn *fetchTxn(ObjId txnId) {
	Txn *txn = fetchIdSlot(txnMap, txnId);

	lockLatch((volatile char *)txn->state);
	return txn;
}

//	add read docId and rts to txn read-set

JsStatus addDocRdToTxn(ObjId txnId, ObjId docId, uint64_t readerTs, uint64_t verNo) {
Txn *txn = fetchIdSlot(txnMap, txnId);
JsStatus stat = (JsStatus)OK;
uint64_t values[3];

	values[0] = docId.bits;
	values[1] = readerTs;
	values[2] = verNo;

	lockLatch((volatile char *)txn->state);

	if ((*txn->state & TYPE_BITS) == TxnGrow)
		addValuesToFrame (txnMap, txn->rdrFrame, NULL, values, 3);
	else
		stat = (JsStatus)ERROR_txn_being_committed;

	unlockLatch((volatile char *)txn->state);
	return (JsStatus)stat;
}

//	add new version to txn write-set

JsStatus addDocWrToTxn(ObjId txnId, ObjId docId) {
Txn *txn = fetchIdSlot(txnMap, txnId);
JsStatus stat = (JsStatus)OK;
uint64_t timestamp;

	lockLatch((volatile char *)txn->state);
	txn->wrtCount++;

	if ((*txn->state & TYPE_BITS) == TxnGrow)
		stat = addValuesToFrame (txnMap, txn->docFrame, NULL, &docId.bits, 1) ? (JsStatus)OK : (JsStatus)ERROR_outofmemory;
	else
		stat = (JsStatus)ERROR_txn_being_committed;

	unlockLatch((volatile char *)txn->state);
	return (JsStatus)stat;
}

//  find appropriate document version per reader timestamp

JsStatus findDocVer(DbMap *map, Doc *doc, JsMvcc *jsMvcc) {
uint32_t offset, size;
JsStatus stat;
ObjId txnId;
Ver *ver;

	offset = doc->lastVer;

	//  is there a pending update for the document
	//	made by our transaction?

	if ((txnId.bits = doc->txnId.bits)) {
	  ver = (Ver *)((uint8_t *)doc + offset);

	  if (jsMvcc->txnId.bits == txnId.bits)
		return ver;

	  // otherwise find a previously committed version

	  offset += ver->verSize;
	}

	//	examine previously committed document versions

	do {
	  ver = (Ver *)((uint8_t *)doc + offset);

	  //  continue to next version chain on stopper version

	  if (!(size = ver->verSize)) {
		if (doc->prevAddr.bits) {
		  doc = getObj(map, doc->prevAddr);
		  offset = doc->lastVer;
		  continue;
		} else
		  return (JsStatus)ERROR_no_visible_version;
	  }

	  if (cc->isolation == Serializable)
		break;

	  if (ver->commitTs < jsMvcc->ts)
		break;

	} while ((offset += size));

	//	add this document to the txn read-set

	if (cc->isolation == Serializable)
	  if ((stat = addDocRdToTxn(txnId, doc->docId, ver->readerTs, ver->verNo)))
		return stat;

	return ver;
}

// 	begin a new Txn

JsStatus beginTxn(Params *params, uint64_t *txnBits) {
ObjId txnId;
Txn *txn;

	if (!txnInit)
		initTxn();

	txnId.bits = allocObjId(txnMap, txnFree, txnWait);
	txn = fetchIdSlot(txnMap, txnId);
	memset (txn, 0, sizeof(Txn));

	//  add 1 to highest committed timestamp
	//	and make preliminary commitment ts

	if (cc->isolation != Serializable)
		txn->timestamp = txnMap->arena->nxtTs + 1;

	txn->nextTxn = *txnBits;
	*txnBits = txnId.bits;
	return (JsStatus)OK;
}

int64_t getTimestamp(bool commit) {
	if (!txnInit)
		initTxn();

	if (commit)
		return atomicAdd64(&txnMap->arena->nxtTs, 2);

	return txnMap->arena->nxtTs + 1;
}

DbStatus rollbackTxn(uint64_t *txnBits) {
	return DB_OK;
}

Ver *firstCommittedVersion(Doc *doc, ObjId docId) {
uint32_t offset = doc->lastVer;
uint32_t size;
Ver *ver;

	ver = (Ver *)((uint8_t *)doc + offset);

	if (!(size = ver->verSize)) {
	  if (doc->prevAddr.bits)
	  	doc = getObj(arenaHandles[docId.xtra]->map, doc->prevAddr);
	  else
		return 0;

	  offset = doc->lastVer;
	}

	return (Ver *)((uint8_t *)doc + offset + ver->verSize);
}

uint64_t tictocValidate(Txn *txn) {
DbAddr *next, *slot, addr;
uint64_t commitTs = 0;
uint64_t rds, verNo;
DbAddr wrtSet[1];
int frameSet;
ObjId docId;
Doc *doc;
Ver *ver;

  wrtSet->bits = 0;
  iniMmbr (memMap, wrtSet, txn->wrtCount);

  // step one: make a WrtSet deduplication mmbr hash
  // table and compute commit timestamp from WrtSet

  next = txn->docFrame;

  while (next->addr) {
	Frame *frame = getObj(txnMap, *next);

	for (int idx = 0; idx < next->nslot; idx++) {
	  docId.bits = frame->slots[idx];

	  slot = fetchIdSlot(arenaHandles[docId.xtra]->map, docId);
	  *setMmbr(memMap, wrtSet, docId.bits, true) = docId.bits;

	  doc = getObj(arenaHandles[docId.xtra]->map, *slot);

	  if ((ver = firstCommittedVersion(doc, docId)))
	    if (ver->readerTs < commitTs)
		  continue;
		else
		  commitTs = ver->readerTs + 1;
	}
	
	next = &frame->next;
  }

  // step two: update the commit timestamp from RdSet

  next = txn->rdrFrame;
  frameSet = 0;

  while (next->addr) {
	Frame *frame = getObj(txnMap, *next);

	for (int idx = 0; idx < next->nslot; idx++) {
	  switch (frameSet++ % 3) {
	  case 0:
		docId.bits = frame->slots[idx];
		continue;

	  case 1:
		rds = frame->slots[idx];
		continue;

	  case 2:
		verNo = frame->slots[idx];
		break;
	  }

	  // was the read doc also updated?

	  if (*setMmbr(memMap, wrtSet, docId.bits, false))
		continue;

	  slot = fetchIdSlot(arenaHandles[docId.xtra]->map, docId);
	  doc = getObj(arenaHandles[docId.xtra]->map, *slot);
	  ver = firstCommittedVersion(doc, docId);

	  if (ver->commitTs > commitTs)
		commitTs = ver->commitTs;
	}

	next = &frame->next;
  }

  // step three: advance the readerTs of the read-set to commitTs
  // and return the rdr Frames

  frameSet = 0;

  while ((addr.bits = txn->rdrFrame->bits)) {
	Frame *frame = getObj(txnMap, addr);

	for (int idx = 0; idx < addr.nslot; idx++) {
	  switch (frameSet++ % 3) {
	  case 0:
		docId.bits = frame->slots[idx];
		continue;

	  case 1:
		rds = frame->slots[idx];
		continue;

	  case 2:
		verNo = frame->slots[idx];
		break;
	  }

	  if (rds >= commitTs)
		continue;

	  slot = fetchIdSlot(arenaHandles[docId.xtra]->map, docId);
	  doc = getObj(arenaHandles[docId.xtra]->map, *slot);
	  ver = firstCommittedVersion(doc, docId);

	  // is our commit impossible?

	  if (ver->readerTs <= commitTs)
		return 0;

	  // install our commitTs as the most recent readerTs
	  // if no other transaction committed a new version of the
	  // document after we read it

	  if (ver->verNo == verNo)
		while ((rds = ver->readerTs) < commitTs)
		  compareAndSwap(&ver->readerTs, rds, commitTs);
	}

	txn->rdrFrame->bits = frame->next.bits;
	returnFreeFrame(txnMap, addr);
  }

  //  step four: commit the inserts/updates
  //	and remove their txnId and return
  //    txn doc Frames.

  while ((addr.bits = txn->docFrame->bits)) {
	Frame *frame = getObj(txnMap, addr);

	for (int idx = 0; idx < addr.nslot; idx++) {
	  docId.bits = frame->slots[idx];

	  slot = fetchIdSlot(arenaHandles[docId.xtra]->map, docId);
	  doc = getObj(arenaHandles[docId.xtra]->map, *slot);

	  if ((ver = firstCommittedVersion(doc, docId))) {
		ver->commitTs = commitTs;
		ver->readerTs = commitTs;
		doc->txnId.bits = 0;
	  }
	}
	
	txn->docFrame->bits = frame->next.bits;
	returnFreeFrame(txnMap, addr);
  }

  return commitTs;
}

JsStatus commitTxn(uint64_t *txnBits) {
ObjId docId, txnId;
Ver *ver, *prevVer;
DbAddr addr, *slot;
uint32_t offset;
uint64_t ts;
DbMap *map;
Doc *doc;
Txn *txn;

	txnId.bits = *txnBits;
	txn = fetchIdSlot(txnMap, txnId);

	lockLatch((volatile char *)txn->state);

	if (*txn->state == TxnGrow)
		*txn->state = TxnShrink;
	else {
		unlockLatch((volatile char *)txn->state);
		return (JsStatus)ERROR_txn_being_committed;
	}

	//	commit the transaction

	if (cc->isolation == Serializable)
		txn->timestamp = tictocValidate(txn);
	else {
		atomicOr64(&txn->timestamp, 1);

	    //  advance master commit timestamp to our timestamp

	   while ((ts = txnMap->arena->nxtTs) < txn->timestamp)
	    compareAndSwap(&txnMap->arena->nxtTs, ts, txn->timestamp);
	}

	while ((addr.bits = txn->docFrame->bits)) {
	  Frame *frame = getObj(txnMap, addr);

	  for (int idx = 0; idx < addr.nslot; idx++) {
		docId.bits = frame->slots[idx];
		map = arenaHandles[docId.xtra]->map;
		slot = fetchIdSlot(map, docId);

		lockLatch(slot->latch);

		doc = getObj(map, *slot);
		ver = (Ver *)((uint8_t *)doc + doc->lastVer);

		// find previous version

		offset = doc->lastVer + ver->verSize;
		prevVer = (Ver *)((uint8_t *)doc + offset);

		//	at end, find in next doc block

		if (!prevVer->verSize) {
		  if (doc->prevAddr.bits) {
		  	Doc *prevDoc = getObj(map, doc->prevAddr);
			prevVer = (Ver *)((uint8_t *)prevDoc + doc->lastVer);
		  } else
			prevVer = NULL;
		}

		//  set the commit timestamp first,
		//  then remove the TxnId

		switch (doc->pending) {
		  case TxnInsert:
		  case TxnUpdate:
			ver->commitTs = txn->timestamp;
			doc->pending = TxnFinished;
			doc->txnId.bits = 0;
			break;

		  default:
			break;
		}

		//	TODO: add previous doc versions to wait queue

		unlockLatch(slot->latch);
	  }

	  //  return processed docFrame,
	  //	advance to next frame

	  txn->docFrame->bits = frame->next.bits;
	  returnFreeFrame(txnMap, addr);
	}

	//	TODO: recycle the txnId and remove the ssiPush frames

	//	remove nested txn

	*txnBits = txn->nextTxn;
	return (JsStatus)OK;
}

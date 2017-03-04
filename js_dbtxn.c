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

//	add read docId to txn read-set

DbStatus addDocRdToTxn(ObjId txnId, ObjId docId) {
Txn *txn = fetchIdSlot(txnMap, txnId);
DbStatus stat = DB_OK;

	lockLatch((volatile char *)txn->state);

	if ((*txn->state & TYPE_BITS) == TxnGrow)
		addSlotToFrame (txnMap, txn->rdrFrame, NULL, docId.bits);
	else
		stat = ERROR_txn_being_committed;

	unlockLatch((volatile char *)txn->state);
	return stat;
}

//	add new version to txn write-set

DbStatus addDocWrToTxn(ObjId txnId, ObjId docId) {
Txn *txn = fetchIdSlot(txnMap, txnId);
DbStatus stat = DB_OK;
uint64_t timestamp;

	lockLatch((volatile char *)txn->state);
	txn->wrtCount++;

	if ((*txn->state & TYPE_BITS) == TxnGrow)
	  if (cc->isolation == Serializable)
		stat = addSlotToFrame (txnMap, txn->docFrame, NULL, docId.bits) ? DB_OK : DB_ERROR_outofmemory;
	  else
		stat = addItemToSkiplist(txnMap, txn->docFrame, docId.bits, 0);
	else
		stat = ERROR_txn_being_committed;

	unlockLatch((volatile char *)txn->state);
	return stat;
}

//  find appropriate document version per reader timestamp
//	call w/docId slot locked

JsStatus findDocVer(DbMap *map, Doc *doc, JsMvcc *jsMvcc) {
uint32_t offset, size;
Txn *docTxn, *rdrTxn;
ObjId txnId;
Ver *ver;

	offset = doc->lastVer;

	//  is there a pending update for the document?

	if (doc->txnId.bits)
	    if (jsMvcc->txnId.bits == doc->txnId.bits)
		  return (Ver *)((uint8_t *)doc + offset);

	//	examine document versions

	do {
	  ver = (Ver *)((uint8_t *)doc + offset);

	  //  continue to next chain on stopper version

	  if (!(size = ver->verSize)) {
		if (doc->prevAddr.bits) {
		  doc = getObj(map, doc->prevAddr);
		  offset = doc->lastVer;
		  continue;
		} else
		  return (JsStatus)ERROR_no_suitable_version;
	  }

	  // was version committed before our txn began?
	  // versions added outside a txn have a commitTs

	  if (ver->commitTs) {
	    if (cc->isolation == Serializable)
		  break;

		if (ver->commitTs < jsMvcc->ts)
		  break;
		else
		  continue;
	  }

	  //  commit sets the ver commit timestamp first,
	  //  then removes the doc TxnId bits.
	  //  retry while both zero

	  if (!(txnId.bits = doc->txnId.bits)) {
		yield();
		size = 0;
		continue;
	  }

	  // skip uncommitted version

	  if (cc->isolation == Serializable)
		continue;

	  // good if committed before our TS

	  docTxn = fetchTxn(txnId);

	  if (cc->isolation == Serializable) {
		addSlotToFrame (txnMap, docTxn->rdrFrame, NULL, doc->docId.bits);
		unlockLatch((volatile char *)docTxn->state);
		break;
	  }

	  if (isCommitted(docTxn->timestamp))
		if (docTxn->timestamp < jsMvcc->ts) {
		  unlockLatch((volatile char *)docTxn->state);
		  break;
		}

	  unlockLatch((volatile char *)docTxn->state);

	} while ((offset += size));

	//	add this document to the txn read-set

	return ver;
}

// 	begin a new Txn

DbStatus beginTxn(Params *params, uint64_t *txnBits) {
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
	return DB_OK;
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

Ver *firstCommitted(Doc *doc, ObjId docId) {
uint32_t offset = doc->lastVer;
uint32_t size;
Ver *ver;

	//  continue to next chain on stopper version

  do {
	ver = (Ver *)((uint8_t *)doc + offset);

	if (!(size = ver->verSize)) {
		if (doc->prevAddr.bits) {
	  	  doc = getObj(arenaHandles[docId.xtra]->map, doc->prevAddr);
		  offset = doc->lastVer;
		  continue;
		} else
		  return NULL;
	}

  } while ((offset += size));

  return ver;
}

uint64_t tictocValidate(Txn *txn) {
DbAddr *next = txn->docFrame, *slot;
uint64_t commitTs = 0;
DbAddr wrtSet[1];
ObjId docId;
Doc *doc;
Ver *ver;

  wrtSet->bits = 0;
  iniMmbr (memMap, wrtSet, txn->wrtCount);

  // step one: lock updated/inserted documents in sorted order
  //   make a deduplication mmbr array

  while (next->addr) {
	SkipNode *skipNode = getObj(txnMap, *next);

	for (int idx = 0; idx < next->nslot; idx++) {
	  SkipEntry *entry = skipNode->array + idx;
	  docId.bits = *entry->key;

	  slot = fetchIdSlot(arenaHandles[docId.xtra]->map, docId);
	  *setMmbr(memMap, wrtSet, docId.bits, true) = docId.bits;
	  lockLatch(slot->latch);
	}
	
	next = skipNode->next;
  }

  // step two: compute the commit timestamp
  // part one: write-set

  next = txn->docFrame;

  while (next->addr) {
	SkipNode *skipNode = getObj(txnMap, *next);

	for (int idx = 0; idx < next->nslot; idx++) {
	  SkipEntry *entry = skipNode->array + idx;
	  docId.bits = *entry->key;

	  slot = fetchIdSlot(arenaHandles[docId.xtra]->map, docId);
	  doc = getObj(arenaHandles[docId.xtra]->map, *slot);
	  ver = firstCommitted(doc, docId);

	  if (ver->readerTs < commitTs)
		continue;

	  commitTs = ver->readerTs + 1;
	}

	next = skipNode->next;
  }

  // step two: compute the commit timestamp
  // part two: read-set

  next = txn->rdrFrame;

  while (next->addr) {
	SkipNode *skipNode = getObj(txnMap, *next);

	for (int idx = 0; idx < next->nslot; idx++) {
	  SkipEntry *entry = skipNode->array + idx;

	  if (*setMmbr(memMap, wrtSet, *entry->key, false))
		continue;

	  docId.bits = *entry->key;

	  slot = fetchIdSlot(arenaHandles[docId.xtra]->map, docId);
	  doc = getObj(arenaHandles[docId.xtra]->map, *slot);
	  ver = firstCommitted(doc, docId);

	  if (ver->commitTs > commitTs)
	  	commitTs = ver->commitTs;
	}

	next = skipNode->next;
  }

  // step three: advance the readerTs of the read-set

  next = txn->rdrFrame;

  while (next->addr) {
	SkipNode *skipNode = getObj(txnMap, *next);

	for (int idx = 0; idx < next->nslot; idx++) {
	  SkipEntry *entry = skipNode->array + idx;
	  uint64_t currStamp;

	  docId.bits = *entry->key;

	  slot = fetchIdSlot(arenaHandles[docId.xtra]->map, docId);
	  doc = getObj(arenaHandles[docId.xtra]->map, *slot);
	  ver = firstCommitted(doc, docId);

	  // is our commit impossible?

	  if (ver->readerTs <= commitTs)
		return 0;

	  // did another transaction commit a new version
	  // after we read this document?

	  
	  while ((currStamp = ver->readerTs) < commitTs)
		compareAndSwap(&ver->readerTs, currStamp, commitTs);

	  if (ver->commitTs > commitTs)
	  	commitTs = ver->commitTs;
	}

	next = skipNode->next;
  }

  //  step four: commit the insert/updates

  return commitTs;
}

DbStatus commitTxn(uint64_t *txnBits) {
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
		return ERROR_txn_being_committed;
	}

	//	commit the transaction

	if (cc->isolation == Serializable)
		txn->timestamp = tictocValidate(txn);
	else
		atomicOr64(&txn->timestamp, 1);

	//	advance master commit timestamp to our timestamp

	if (cc->isolation != Serializable)
	 while ((ts = txnMap->arena->nxtTs) < txn->timestamp)
	  compareAndSwap(&txnMap->arena->nxtTs, ts, txn->timestamp);

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
	return DB_OK;
}

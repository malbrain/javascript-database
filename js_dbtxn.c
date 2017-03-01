#include "js.h"
#include "js_db.h"
#include "js_dbindex.h"
#include "database/db_frame.h"

extern Handle **arenaHandles;

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

Txn *fetchTxn(ObjId txnId) {
	return fetchIdSlot(txnMap, txnId);
}

DbStatus addDocToTxn(ObjId txnId, ObjId docId) {
Txn *txn = fetchIdSlot(txnMap, txnId);
DbStatus stat = DB_OK;

	lockLatch((volatile char *)txn->state);

	if ((*txn->state & TYPE_BITS) == TxnGrow)
		addSlotToFrame (txnMap, txn->frame, NULL, docId.bits);
	else
		stat = ERROR_txn_nolonger_growing;

	unlockLatch((volatile char *)txn->state);
	return stat;
}

//  find appropriate document version per txn beginning timestamp
//	call w/docId slot locked

Ver *findDocVer(DbMap *map, Doc *doc, JsMvcc *jsMvcc) {
uint32_t offset, size;
uint64_t rdTs;
Txn *docTxn;
ObjId txnId;
Ver *ver;

	offset = doc->lastVer;

	// same TXN made version?

	if ((txnId.bits = doc->txnId.bits))
	 if (jsMvcc->txnId.bits)
	  if (txnId.bits == jsMvcc->txnId.bits)
	  	return (Ver *)((uint8_t *)doc + offset);

	//  is the read operation legal?
	//  a read operation cannot be newer
	//	than the uncommitted update ts

	if (doc->writeTs)
	  if (doc->writeTs < jsMvcc->ts)
		return TXN_ERR_rw_conflict;
		
	do {
	  ver = (Ver *)((uint8_t *)doc + offset);

	  //  continue chain on stopper version

	  if (!(size = ver->verSize)) {
		if (doc->prevAddr.bits) {
		  doc = getObj(map, doc->prevAddr);
		  offset = doc->lastVer;
		  continue;
		} else
		  return NULL;
	  }

	  // was version committed before our txn began?
	  // versions added outside a txn have a commitTs

	  if (ver->commitTs) {
		if (ver->commitTs < jsMvcc->ts)
		  break;
		else
		  continue;
	  }

	  //  commit sets the ver commit timestamp first,
	  //  then removes the doc TxnId bits.
	  //  retry while both zero

	  if ((txnId.bits = doc->txnId.bits))
	  	docTxn = fetchTxn(txnId);
	  else {
		yield();
		size = 0;
		continue;
	  }

	  // good if committed before our TS

	  if (isCommitted(docTxn->timestamp))
		if (docTxn->timestamp < jsMvcc->ts)
		  break;

	} while ((offset += size));

	//  keep the maximum reader timestamp for the read version
	//	while no newer committed versions exists

	while ((rdTs = ver->maxRdTs) < jsMvcc->ts && !ver->newerVer)
	  compareAndSwap(&ver->maxRdTs, rdTs, jsMvcc->ts);

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
		return ERROR_txn_nolonger_growing;
	}

	//	commit the transaction

	atomicOr64(&txn->timestamp, 1);

	//	advance master commit timestamp to our timestamp

	while ((ts = txnMap->arena->nxtTs) < txn->timestamp)
	  compareAndSwap(&txnMap->arena->nxtTs, ts, txn->timestamp);

	while ((addr.bits = txn->frame->bits)) {
	  Frame *frame = getObj(txnMap, addr);

	  for (int idx = 0; idx < addr.nslot; idx++) {
		docId.bits = frame->slots[idx];
		map = arenaHandles[docId.xtra]->map;
		slot = fetchIdSlot(arenaHandles[docId.xtra]->map, docId);

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

		if (prevVer)
		  prevVer->newerVer = true;

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

	  //  return processed frame,
	  //	advance to next frame

	  txn->frame->bits = frame->next.bits;
	  returnFreeFrame(txnMap, addr);
	}

	//	remove nested txn

	*txnBits = txn->nextTxn;
	return DB_OK;
}

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

void initTxn(void) {
ArenaDef arenaDef[1];

	txnMap->arena = txnArena;
	txnMap->db = txnMap;

#ifdef _WIN32
	txnMap->hndl = INVALID_HANDLE_VALUE;
#else
	txnMap->hndl = -1;
#endif

	//	set up memory arena and handle addr ObjId

	memset (arenaDef, 0, sizeof(arenaDef));
	arenaDef->objSize = sizeof(Txn);

	initArena(txnMap, arenaDef, "txn", 3, NULL);
	txnInit = true;
}

Txn *fetchTxn(ObjId txnId) {
	return fetchIdSlot(txnMap, txnId);
}

DbStatus addDocToTxn(ObjId txnId, ObjId docId) {
Txn *txn = fetchIdSlot(txnMap, txnId);
DbStatus stat = DB_OK;

	lockLatch((volatile char *)txn->state);

	if (*txn->state & TYPE_BITS == TxnGrow)
		addSlotToFrame (txnMap, txn->frame, NULL, docId.bits);
	else
		stat = ERROR_txn_nolonger_growing;

	unlockLatch((volatile char *)txn->state);
	return stat;
}

//  find appropriate document version per txn beginning timestamp

Ver *findDocVer(DbMap *map, Doc *doc, JsMvcc *jsMvcc) {
uint64_t timestamp = UINT64_MAX, txnTs;
Txn *txn = NULL;

  if (jsMvcc) {
   if (jsMvcc->txnId.bits) {
	txn = fetchIdSlot(txnMap, jsMvcc->txnId);
	timestamp = txn->timestamp;
   } else
	timestamp = jsMvcc->ts;
  }

  if (doc->pending == TxnNone)
	return (Ver *)((uint8_t *)doc + doc->lastVer);

  //  examine versions for visibility

  do {
	uint32_t offset = doc->lastVer;

	while (true) {
	  Ver *ver = (Ver *)((uint8_t *)doc + offset);

	  //  stopper version?

	  if (!ver->verSize)
		break;

	  // same TXN?

	  if (jsMvcc)
	   if (jsMvcc->txnId.bits && ver->txnId.bits == jsMvcc->txnId.bits)
		return ver;

	  // is version committed before our txn began?

	  if (ver->timestamp) {
		if (ver->timestamp < timestamp)
		  return ver;

		offset += ver->verSize;
		continue;
	  }

	  // check txn timestamp

	  if (ver->txnId.bits) {
		Txn *verTxn = fetchIdSlot(txnMap, ver->txnId);

		if (isCommitted(verTxn->timestamp))
		  if (verTxn->timestamp < txn->timestamp)
			return ver;

		//	advance txn timestamp to reader's ts

		while (isReader((txnTs = txn->timestamp)) && txnTs < verTxn->timestamp)
		  compareAndSwap(&txn->timestamp, txnTs, verTxn->timestamp);

		//  in an extremely narrow window,
		//  the txn might have committed

		if (isCommitted(verTxn->timestamp))
		  if (verTxn->timestamp < txn->timestamp)
			return ver;

		offset += ver->verSize;
	  }
	}
  } while ((doc = doc->prevAddr.bits ? getObj(map, doc->prevAddr) : NULL));

  return NULL;
}

// 	allocate a new Txn in read state

ObjId beginTxn(Params *params) {
ObjId txnId;
Txn *txn;

	if (!txnInit)
		initTxn();

	txnId.bits = allocObjId(txnMap, txnFree, txnWait);
	txn = fetchIdSlot(txnMap, txnId);
	memset (txn, 0, sizeof(Txn));

	txn->timestamp = allocateTimestamp(en_reader);
	return txnId;
}

DbStatus rollbackTxn(DbHandle hndl[1], ObjId txnId) {
	return DB_OK;
}

DbStatus commitTxn(ObjId txnId) {
DbAddr addr;
ObjId docId;
uint64_t ts;
Doc *doc;
Txn *txn;
Ver *ver;

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

	//	advance master timestamp to our timestamp

	while ((ts = txnMap->arena->nxtTs) < txn->timestamp)
	  compareAndSwap(&txnMap->arena->nxtTs, ts, txn->timestamp);

	while ((addr.bits = txn->frame->bits)) {
	  Frame *frame = getObj(txnMap, addr);

	  for (int idx = 0; idx < addr.nslot; idx++) {
		docId.bits = frame->slots[idx];
		doc = fetchIdSlot(arenaHandles[docId.xtra]->map, docId);
		ver = (Ver *)((uint8_t *)doc + doc->lastVer);

		//  set the commit timestamp first,
		//  then remove the TxnId

		switch (doc->pending) {
		  case TxnInsert:
		  case TxnUpdate:
			ver->timestamp = txn->timestamp;
			ver->txnId.bits = 0;

			doc->pending = TxnFinished;
			break;
		  default:
			break;
		}

		//	TODO: add previous doc versions to wait queue

		//  return processed frame,
		//	advance to next frame

		txn->frame->bits = frame->next.bits;
		returnFreeFrame(txnMap, addr);
	  }
	}

	return DB_OK;
}

//	allocate a new timestamp

uint64_t allocateTimestamp(ReaderWriterEnum e) {
uint64_t ts;

	ts = txnMap->arena->nxtTs;

	switch (e) {
	case en_reader:
		while (!isReader(ts))
			ts = atomicAdd64(&txnMap->arena->nxtTs, 1);
		break;
	case en_writer:
		while (!isWriter(ts))
			ts = atomicAdd64(&txnMap->arena->nxtTs, 1);
		break;

	default: break;
	}

	return ts;
}


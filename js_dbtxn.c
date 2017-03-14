#include "js.h"
#include "js_props.h"
#include "js_string.h"

#include "js_db.h"
#include "js_dbindex.h"
#include "database/db_frame.h"

extern Handle **arenaHandles;
extern DbMap memMap[1];
extern DbMap *hndlMap;
CcMethod *cc;

//  Txn arena free txn frames

char txnInit[1];
DbMap *txnMap;

//	Txn structure

typedef union {
  struct {
	DbAddr txnFree[1];		// frames of available txnId
	DbAddr txnWait[1];		// frames of waiting txnId
  };
  char filler[256];
} Transactions;

Transactions *transactions;

//	initialize transaction database

//  the timestamps are issued w/even
//	values to readers, and odd values
//	to committers.

#define isReader(ts) (~(ts) & 1)
#define isCommit(ts) ((ts) & 1)

void initTxn(void) {
ArenaDef arenaDef[1];

	lockLatch(txnInit);

	if (*txnInit & TYPE_BITS) {
		unlockLatch(txnInit);
		return;
	}

	// configure transaction table

	memset(arenaDef, 0, sizeof(arenaDef));
	arenaDef->params[OnDisk].boolVal = hndlMap->arenaDef->params[OnDisk].boolVal;
	arenaDef->objSize = sizeof(Txn);
	arenaDef->arenaType = Hndl_txns;

	txnMap = openMap(NULL, "Txns", 4, arenaDef, NULL);
	txnMap->db = txnMap;

	transactions = (Transactions *)(txnMap->arena + 1);

	*txnMap->arena->type = Hndl_txns;
	*txnInit = Hndl_txns;
}

//	fetch and lock txn

Txn *fetchTxn(ObjId txnId) {
	Txn *txn = fetchIdSlot(txnMap, txnId);

	lockLatch(txn->state);
	return txn;
}

//	add read docId and rts to txn read-set

JsStatus addDocRdToTxn(ObjId txnId, ObjId docId, Ver *ver) {
JsStatus stat = (JsStatus)OK;
Txn *txn = fetchTxn(txnId);
uint64_t values[3];

	values[0] = docId.bits;
	values[1] = ver->readerTs;
	values[2] = ver->commitTs;

	if ((*txn->state & TYPE_BITS) == TxnGrow)
		addValuesToFrame (txnMap, txn->rdrFrame, NULL, values, 3);
	else
		stat = (JsStatus)ERROR_txn_being_committed;

	unlockLatch(txn->state);
	return (JsStatus)stat;
}

//	add new version to txn write-set

JsStatus addDocWrToTxn(ObjId txnId, ObjId docId) {
JsStatus stat = (JsStatus)OK;
Txn *txn = fetchTxn(txnId);

	txn->wrtCount++;

	if ((*txn->state & TYPE_BITS) == TxnGrow)
		stat = addValuesToFrame (txnMap, txn->docFrame, NULL, &docId.bits, 1) ? (JsStatus)OK : (JsStatus)ERROR_outofmemory;
	else
		stat = (JsStatus)ERROR_txn_being_committed;

	unlockLatch(txn->state);
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

	  if (jsMvcc->isolation == Serializable)
		break;

	  if (ver->commitTs < jsMvcc->ts)
		break;

	} while ((offset += size));

	//	add this document to the txn read-set

	if (jsMvcc->txnId.bits)
	  if ((stat = addDocRdToTxn(jsMvcc->txnId, doc->docId, ver)))
		return stat;

	return ver;
}

// 	begin a new Txn

uint64_t beginTxn(Params *params, uint64_t *txnBits) {
ObjId txnId;
Txn *txn;

	if (!*txnInit)
		initTxn();

	txnId.bits = allocObjId(txnMap, transactions->txnFree, transactions->txnWait);
	txn = fetchIdSlot(txnMap, txnId);
	memset (txn, 0, sizeof(Txn));

	if (params[Concurrency].intVal)
		txn->isolation = params[Concurrency].intVal;
	else if (cc->isolation)
		txn->isolation = cc->isolation;
	else
		txn->isolation = SnapShot;

	//  add 1 to highest committed timestamp
	//	and make preliminary commitment ts

	if (txn->isolation != Serializable)
	  while (!isReader(txn->timestamp = txnMap->arena->nxtTs))
		atomicAdd64(&txnMap->arena->nxtTs, 1LL);

	txn->nextTxn = *txnBits;

	*txn->state = TxnGrow;
	return *txnBits = txnId.bits;
}

uint64_t getSnapshotTimestamp(JsMvcc *jsMvcc, bool commit) {
	uint64_t ts = 0;

	if (!*txnInit)
	  initTxn();

	//	return the txn read timestamp

	if (jsMvcc && jsMvcc->txnId.bits) {
	  Txn *txn = fetchIdSlot(txnMap, jsMvcc->txnId);

	  if ((jsMvcc->isolation = txn->isolation) == SnapShot)
		return jsMvcc->ts = txn->timestamp;
	}

	if (cc->isolation != Serializable) {
	  if (commit) {
	    while (!isCommit(ts = txnMap->arena->nxtTs))
		  atomicAdd64(&txnMap->arena->nxtTs, 1LL);
	  } else {
	    while (!isReader(ts = txnMap->arena->nxtTs))
		  atomicAdd64(&txnMap->arena->nxtTs, 1LL);
	  }

	  if (jsMvcc)
	  	jsMvcc->ts = ts;
	}

	return ts;
}

JsStatus rollbackTxn(Params *params, uint64_t *txnBits) {
	return (JsStatus)OK;
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

//	verify and commit txn under
//	Serializable isolation

bool tictocCommit(Txn *txn) {
uint64_t commitTs = 0, *wrtMmbr;
DbAddr *next, *slot, addr;
bool result = true;
uint64_t wts, rds;
DbAddr wrtSet[1];
int frameSet;
ObjId docId;
Doc *doc;
Ver *ver;

  wrtSet->bits = 0;
  iniMmbr (memMap, wrtSet, txn->wrtCount);

  // step one: lock document and make a WrtSet deduplication
  // mmbr hash table and compute commit timestamp from WrtSet

  next = txn->docFrame;

  while (next->addr) {
	Frame *frame = getObj(txnMap, *next);

	for (int idx = 0; idx < next->nslot; idx++) {
	  docId.bits = frame->slots[idx];
	  slot = fetchIdSlot(arenaHandles[docId.xtra]->map, docId);

	  wrtMmbr = setMmbr(memMap, wrtSet, docId.bits, true);

	  //  if we have already started to commit doc
	  //	remove this subsequent entry

	  if (*wrtMmbr) {
		unlockLatch(slot->latch);
		frame->slots[idx] = 0;
		continue;
	  }

	  // add docId to wrtSet dedup hash table

	  *wrtMmbr = docId.bits;

	  // preliminary commitment

	  lockLatch(slot->latch);
	  doc = getObj(arenaHandles[docId.xtra]->map, *slot);
	  doc->op |= Committing;
	  unlockLatch(slot->latch);

	  //  compute commitTs

	  if ((ver = firstCommittedVersion(doc, docId))) {
	    if (ver->readerTs < commitTs)
		  continue;
		else
		  commitTs = ver->readerTs + 1;
	  }
	}
	
	next = &frame->next;
  }

  // step two: use RdSet to update the commit timestamp

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
		wts = frame->slots[idx];
		break;
	  }

	  // was the RdSet doc also in WrtSet (ie, updated)?

	  if (*setMmbr(memMap, wrtSet, docId.bits, false))
		continue;

	  if (wts > commitTs)
		commitTs = wts;
	}

	next = &frame->next;
  }

  // step three: update the readerTs of the read-set mmbrs to commitTs
  // and release the rdr Frames

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
		wts = frame->slots[idx];
		break;
	  }

	  if (rds >= commitTs)
		continue;

	  slot = fetchIdSlot(arenaHandles[docId.xtra]->map, docId);

	  // if we are also writing the read-set mmbr, skip the update

	  if (*setMmbr(memMap, wrtSet, docId.bits, false))
		continue;

	  // lock the document from being committed elsewhere

	  lockLatch(slot->latch);
	  doc = getObj(arenaHandles[docId.xtra]->map, *slot);

	  ver = firstCommittedVersion(doc, docId);

	  //  has another commit blocked us out from our commitTs?
	  //  or was the version already committed elsewhere?

	  if (ver->readerTs <= commitTs || ver->commitTs != wts || doc->op & Committing) {
		  unlockLatch(slot->latch);
		  return false;
	  }

	  // install our commitTs as the most recent readerTs

	  if (ver->readerTs < commitTs)
		ver->readerTs = commitTs;

	  unlockLatch(slot->latch);
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
	  if ((docId.bits = frame->slots[idx]))
	    slot = fetchIdSlot(arenaHandles[docId.xtra]->map, docId);
	  else
		continue;

	  doc = getObj(arenaHandles[docId.xtra]->map, *slot);

	  if ((ver = firstCommittedVersion(doc, docId))) {
		ver->readerTs = commitTs;
		ver->commitTs = commitTs;
		doc->txnId.bits = 0;
		doc->op = Done;
	  }

	  unlockLatch(slot->latch);
	 }
	
	txn->docFrame->bits = frame->next.bits;
	returnFreeFrame(txnMap, addr);
  }

  if (result)
	txn->timestamp = commitTs;

  return result;
}

//	commit txn under snapshot isolation
//	always succeeds

bool snapshotCommit(Txn *txn) {
Ver *ver, *prevVer;
DbAddr addr, *slot;
uint32_t offset;
ObjId docId;
DbMap *map;
Doc *doc;

	while ((addr.bits = txn->docFrame->bits)) {
	  Frame *frame = getObj(txnMap, addr);

	  for (int idx = 0; idx < addr.nslot; idx++) {
		docId.bits = frame->slots[idx];
		map = arenaHandles[docId.xtra]->map;
		slot = fetchIdSlot(map, docId);

		doc = getObj(map, *slot);
		ver = (Ver *)((uint8_t *)doc + doc->lastVer);

		// find previous version

		offset = doc->lastVer + ver->verSize;
		prevVer = (Ver *)((uint8_t *)doc + offset);

		//	at top end, find in prev doc block

		if (!prevVer->verSize) {
		  if (doc->prevAddr.bits) {
		  	Doc *prevDoc = getObj(map, doc->prevAddr);
			prevVer = (Ver *)((uint8_t *)prevDoc + doc->lastVer);
		  } else
			prevVer = NULL;
		}

		//  set the commit timestamp first,
		//  then remove the TxnId

		switch (doc->op) {
		  case Insert:
		  case Update:
			ver->commitTs = txn->timestamp;
			doc->txnId.bits = 0;
			doc->op = Done;
			break;

		  default:
			break;
		}

		//	TODO: add previous doc versions to wait queue
	  }

	  //  return processed docFrame,
	  //	advance to next frame

	  txn->docFrame->bits = frame->next.bits;
	  returnFreeFrame(txnMap, addr);
	}

	return true;
}

JsStatus commitTxn(Params *params, uint64_t *txnBits) {
ObjId txnId;
Txn *txn;

	txnId.bits = *txnBits;
	txn = fetchTxn(txnId);

	if ((*txn->state & TYPE_BITS) == TxnGrow)
		*txn->state = TxnShrink | MUTEX_BIT;
	else {
		unlockLatch(txn->state);
		return (JsStatus)ERROR_txn_being_committed;
	}

	//	commit the transaction

	switch (txn->isolation) {
	  case Serializable:
		if (tictocCommit(txn))
			break;

	  case SnapShot:
	    while (!isCommit(txn->timestamp = txnMap->arena->nxtTs))
		  atomicAdd64(&txnMap->arena->nxtTs, 1LL);

		snapshotCommit(txn);
		break;
	}

	//	TODO: recycle the txnId

	//	remove nested txn
	//	and unlock

	*txnBits = txn->nextTxn;
	*txn->state = Done;
	return (JsStatus)OK;
}

//	display a txn

value_t fcnTxnToString(value_t *args, value_t *thisVal, environment_t *env) {
	char buff[64];
	ObjId txnId;
	int len;

	txnId.bits = thisVal->idBits;

#ifndef _WIN32
	len = snprintf(buff, sizeof(buff), "%X:%X", txnId.seg, txnId.idx);
#else
	len = _snprintf_s(buff, sizeof(buff), _TRUNCATE, "%X:%X", txnId.seg, txnId.idx);
#endif
	return newString(buff, len);
}

value_t propTxnCount(value_t val, bool lVal) {
	value_t count;
	ObjId txnId;

	count.bits = vt_int;
	count.nval = 0;

	if (val.type == vt_txn)
	  if ((txnId.bits = val.idBits)) {
		Txn *txn = fetchIdSlot(txnMap, txnId);
		count.nval = txn->wrtCount;
	  }

	return count;
}

PropFcn builtinTxnFcns[] = {
	{ fcnTxnToString, "toString" },
	{ NULL, NULL}
};

PropVal builtinTxnProp[] = {
	{ propTxnCount, "count" },
	{ NULL, NULL}
};


#include "js.h"
#include "js_props.h"
#include "js_string.h"

#include "js_db.h"
#include "js_dbindex.h"
#include "database/db_frame.h"

extern DbMap memMap[1];
extern DbMap *hndlMap;

Catalog *catalog;
CcMethod *cc;

//  Txn arena free txn frames

void initTxn(void);
DbMap *txnMap;
char txnInit[1];

extern int clusterId;		//  used for db clusters

#ifdef __MACH__
#include <mach/clock.h>
#include <mach/mach.h>
#endif

#ifndef _WIN32
#include <time.h>
#include <sys/time.h>
#endif

//	GlobalTxn structure, per clusterId

typedef union {
  struct {
	DbAddr txnFree[1];				// frames of available txnId
	DbAddr txnWait[1];				// frames of waiting txnId
	Timestamp baseTs[1];			// master timestamp basis
  };
  char filler[256];
} GlobalTxn;

GlobalTxn *globalTxn;

//	install new timestamp if > or < existing value

void compareSwapTs(Timestamp *dest, Timestamp *src, int chk) {
Timestamp cmp[1];

  do {
	cmp->epoch = dest->epoch;
	cmp->lowBits = dest->lowBits;

	if (chk > 0 && compareTs(cmp, src) <= 0)
	  return;

	if (chk < 0 && compareTs(cmp, src) >= 0)
	  return;

#ifdef _WIN32
  } while (!_InterlockedCompareExchange128((uint64_t *)dest, src->epoch, src->lowBits, (uint64_t *)cmp) );
#else
  } while (!__atomic_compare_exchange((__int128 *)dest, (__int128 *)cmp, (__int128 *)src, false, __ATOMIC_SEQ_CST, __ATOMIC_ACQUIRE));
#endif
}

//	install a timestamp value

void installTs(Timestamp *dest, Timestamp *src) {
#ifdef _WIN32
Timestamp temp[1];

  do {
	temp->epoch = dest->epoch;
	temp->lowBits = dest->lowBits;
  } while (!_InterlockedCompareExchange128((uint64_t *)dest, src->epoch, src->lowBits, (uint64_t *)temp) );
#else
	__atomic_store((__int128 *)dest, (__int128 *)src, __ATOMIC_SEQ_CST);
#endif
}

//	create new timestamp generator

void *newTsGen (void) {
Timestamp *ts = js_alloc(sizeof(Timestamp), true);

	if (!*txnInit)
		initTxn();

	ts->processId = atomicAdd16(&globalTxn->baseTs->processId, 1);
	ts->clusterId = globalTxn->baseTs->clusterId;
	ts->epoch = globalTxn->baseTs->epoch;
	ts->sequence = 1;
	return ts;
}

//	generate new timestamp

//	based on globalTxn and
//	the previous timestamp

void newTs(Timestamp *ts, Timestamp *gen, bool reader) {
	if (gen->epoch != globalTxn->baseTs->epoch) {
		gen->epoch = globalTxn->baseTs->epoch;
		gen->lowBits = globalTxn->baseTs->lowBits;
		gen->sequence = 1;
	}

	ts->epoch = gen->epoch;

	if (reader)
		ts->lowBits = gen->lowBits + 1;
	else
		ts->lowBits = gen->lowBits += 2;

	return;
}

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
	arenaDef->baseSize = sizeof(GlobalTxn);
	arenaDef->arenaType = Hndl_txns;
	arenaDef->objSize = sizeof(Txn);

	txnMap = openMap(NULL, "Txns", 4, arenaDef, NULL);
	txnMap->db = txnMap;

	globalTxn = (GlobalTxn *)(txnMap->arena + 1);
	globalTxn->baseTs->clusterId = clusterId;
	globalTxn->baseTs->epoch = db_getEpoch();

	*txnMap->arena->type = Hndl_txns;
	*txnInit = Hndl_txns;
}

//	fetch and lock txn

Txn *fetchTxn(ObjId txnId) {
	Txn *txn = fetchIdSlot(txnMap, txnId);

	lockLatch(txn->state);
	return txn;
}

//	add docId and verNo to txn read-set
//	do not call for "read my writes"

JsStatus addDocRdToTxn(ObjId txnId, ObjId docId, Ver *ver, uint64_t hndlBits) {
JsStatus stat = (JsStatus)OK;
Txn *txn = fetchTxn(txnId);
uint64_t values[3];
Handle *docHndl;
DbAddr *slot;
int cnt = 0;
ObjId objId;

	if (txn->isolation != Serializable) {
		unlockLatch(txn->state);
		return stat;
	}

	objId.bits = hndlBits;
	objId.xtra = TxnHndl;

	slot = slotHandle(objId);
	docHndl = getObj(hndlMap, *slot);

	if (txn->hndlId->bits != docHndl->hndlId.bits) {
		txn->hndlId->bits = docHndl->hndlId.bits;
		atomicAdd32(docHndl->bindCnt, 1);
		values[cnt++] = objId.bits;
	}

	docId.xtra = TxnDoc;
	values[cnt++] = docId.bits;
	values[cnt++] = ver->verNo;

	if ((*txn->state & TYPE_BITS) != TxnGrow)
		stat = (JsStatus)ERROR_txn_being_committed;
	else if (ver->sstamp->epoch < ~0ULL)
		addValuesToFrame (txnMap, txn->rdrFrame, txn->rdrFirst, values, cnt);
	else
		compareSwapTs(txn->sstamp, ver->sstamp, -1);

	if (stat == (JsStatus)OK)
	  if (compareTs(txn->sstamp, txn->pstamp) <= 0)
		stat = (JsStatus)ERROR_txn_not_serializable;

	unlockLatch(txn->state);
	return stat;
}

//	add version creation to txn write-set
//  do not call for "update my writes"

JsStatus addDocWrToTxn(ObjId txnId, ObjId docId, Ver *ver, Ver *prevVer, uint64_t hndlBits) {
JsStatus stat = (JsStatus)OK;
Txn *txn = fetchTxn(txnId);
uint64_t values[2];
Handle *docHndl;
DbAddr *slot;
int cnt = 0;
ObjId objId;

	if (txn->isolation == NotSpecified) {
		unlockLatch(txn->state);
		return stat;
	}

	objId.bits = hndlBits;
	objId.xtra = TxnHndl;

	slot = slotHandle(objId);
	docHndl = getObj(hndlMap, *slot);

	if (txn->hndlId->bits != hndlBits) {
		txn->hndlId->bits = docHndl->hndlId.bits;
		atomicAdd32(docHndl->bindCnt, 1);
		values[cnt++] = objId.bits;
	}

	txn->wrtCount++;

	docId.xtra = TxnDoc;
	values[cnt++] = docId.bits;

	if (txn->isolation == Serializable) {
	  if (prevVer)
		compareSwapTs(txn->pstamp, prevVer->pstamp, -1);

	  if (compareTs(txn->sstamp, txn->pstamp) <= 0)
		stat = (JsStatus)ERROR_txn_not_serializable;
	}

	if ((*txn->state & TYPE_BITS) == TxnGrow)
		stat = addValuesToFrame (txnMap, txn->docFrame, txn->docFirst, values, cnt) ? (JsStatus)OK : (JsStatus)ERROR_outofmemory;
	else
		stat = (JsStatus)ERROR_txn_being_committed;

	unlockLatch(txn->state);
	return stat;
}

//	retrieve previous version

//	retrieve version by verNo

Ver *getVersion(DbMap *map, Doc *doc, uint64_t verNo) {
uint32_t offset, size;
Ver *ver;

	offset = doc->lastVer;

	//	enumerate previous document versions

	do {
	  ver = (Ver *)((uint8_t *)doc + offset);

	  //  continue to next version chain on stopper version

	  if (!(size = ver->verSize)) {
		if (doc->prevAddr.bits) {
		  doc = getObj(map, doc->prevAddr);
		  offset = doc->lastVer;
		  continue;
		} else
		  return NULL;
	  }

	  if (ver->verNo == verNo)
		break;

	} while ((offset += size));

	return ver;
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

	  if (compareTs(ver->commit, jsMvcc->reader) < 0)
		break;

	} while ((offset += size));

	//	add this document to the txn read-set

	if (jsMvcc->txnId.bits)
	  if ((stat = addDocRdToTxn(jsMvcc->txnId, doc->docId, ver, jsMvcc->hndl->hndlBits)))
		return stat;

	return ver;
}

// 	begin a new Txn

uint64_t beginTxn(Params *params, uint64_t *txnBits, Timestamp *tsGen) {
ObjId txnId;
Txn *txn;

	if (!*txnInit)
		initTxn();

	txnId.bits = allocObjId(txnMap, globalTxn->txnFree, globalTxn->txnWait);
	txn = fetchIdSlot(txnMap, txnId);
	memset (txn, 0, sizeof(Txn));

	if (params[Concurrency].intVal)
		txn->isolation = params[Concurrency].intVal;
	else if (cc->isolation)
		txn->isolation = cc->isolation;
	else
		txn->isolation = SnapShot;
	
	newTs(txn->reader, tsGen, true);
	txn->nextTxn = *txnBits;
	*txn->state = TxnGrow;

	txn->sstamp->epoch = ~0ULL;
	return *txnBits = txnId.bits;
}

JsStatus rollbackTxn(Params *params, uint64_t *txnBits) {
	return (JsStatus)OK;
}

Ver *firstCommittedVersion(DbMap *map, Doc *doc, ObjId docId) {
uint32_t offset = doc->lastVer;
uint32_t size;
Ver *ver;

	ver = (Ver *)((uint8_t *)doc + offset);

	if (!(size = ver->verSize)) {
	  if (doc->prevAddr.bits)
	  	doc = getObj(map, doc->prevAddr);
	  else
		return 0;

	  offset = doc->lastVer;
	}

	return (Ver *)((uint8_t *)doc + offset + ver->verSize);
}

//	verify and commit txn under
//	Serializable isolation

bool SSNCommit(Txn *txn) {
DbAddr *slot, next, addr;
Ver *ver, *prevVer;
bool result = true;
uint64_t *wrtMmbr;
DbAddr wrtSet[1];
Handle *docHndl;
uint32_t offset;
int frameSet;
ObjId docId;
ObjId objId;
int nSlot;
Doc *doc;
int idx;

  wrtSet->bits = 0;
  iniMmbr (memMap, wrtSet, txn->wrtCount);

  // make a WrtSet deduplication
  // mmbr hash table

  next.bits = txn->docFirst->bits;
  docHndl = NULL;
  frameSet = 0;

  //  PreCommit

  //  construct de-duplication hash table for wrtSet
  //	and finalize txn->pstamp

  while (next.addr) {
	Frame *frame = getObj(txnMap, next);

	// when we get to the last frame,
	//	pull the count from free head

	if (!frame->prev.bits)
		nSlot = txn->docFrame->nslot;
	else
		nSlot = FrameSlots;

	for (idx = 0; idx < nSlot; idx++) {
	  //  finalize TxnDoc

	  if (frameSet) {
		DbAddr *docSlot = fetchIdSlot(docHndl->map, docId);
		uint64_t verNo = frame->slots[idx];

		lockLatch(docSlot->latch);

		doc = getObj(docHndl->map, *docSlot);
        doc->op |= Committing;

		prevVer = getVersion(docHndl->map, doc, verNo - 1);

		//	keep larger pstamp

		compareSwapTs(txn->pstamp, prevVer->pstamp, -1);

		unlockLatch(docSlot->latch);
		frameSet = 0;
		continue;
	  }

	  objId.bits = frame->slots[idx];

	  switch (objId.xtra) {
		case TxnHndl:
		  if (docHndl)
			releaseHandle(docHndl, NULL);

	  	  slot = fetchIdSlot(hndlMap, objId);
		  docHndl = getObj(hndlMap, *slot);
		  continue;

		case TxnDoc:
		  docId.bits = objId.bits;
	  	  wrtMmbr = setMmbr(memMap, wrtSet, docId.bits, true);

		  // add docId to wrtSet dedup hash table
		  // this txn owns the document

		  *wrtMmbr = docId.bits;
		  frameSet = 1;
		  break;
	  }	
	}
	
	next.bits = frame->prev.bits;
  }

  if (docHndl)
	releaseHandle(docHndl, NULL);

  //  PreCommit

  // finalize txn->sstamp from the readSet

  next.bits = txn->rdrFirst->bits;
  docHndl = NULL;
  frameSet = 0;

  while ((addr.bits = next.bits)) {
	Frame *frame = getObj(txnMap, addr);

	// when we get to the last frame,
	//	pull the count from free head

	if (!frame->prev.bits)
		nSlot = txn->rdrFrame->nslot;
	else
		nSlot = FrameSlots;

	for (idx = 0; idx < nSlot; idx++) {
	  // finish TxnDoc steps

	  if (frameSet) {
		DbAddr *docSlot = fetchIdSlot(docHndl->map, docId);
		uint64_t verNo = frame->slots[idx];

		frameSet = 0;

		// if we also write this read-set mmbr, skip it

		if (*setMmbr(memMap, wrtSet, docId.bits, false))
		  continue;

		doc = getObj(docHndl->map, *docSlot);
		prevVer = getVersion(docHndl->map, doc, verNo);

		//	keep smaller sstamp

		compareSwapTs(txn->sstamp, prevVer->sstamp, 1);
		continue;
	  }

	  objId.bits = frame->slots[idx];

	  switch (objId.xtra) {
		case TxnHndl:
		  if (docHndl)
			releaseHandle(docHndl, NULL);
		  
	  	  slot = fetchIdSlot(hndlMap, objId);
		  docHndl = getObj(hndlMap, *slot);
		  continue;

		case TxnDoc:
		  docId.bits = objId.bits;
		  frameSet = 1;
		  continue;
	  }
	}

	next.bits = frame->prev.bits;
  }

  if (compareTs(txn->sstamp, txn->pstamp) <= 0)
	result = false;

  if (result)
	*txn->state = TxnCommit | MUTEX_BIT;
  else
	*txn->state = TxnRollback | MUTEX_BIT;

  if (docHndl)
	releaseHandle(docHndl, NULL);

  //  Post Commit

  //  process the reader pstamp from our commit time
  //	return reader set Frames.

  next.bits = txn->rdrFirst->bits;
  docHndl = NULL;
  frameSet = 0;

  while ((addr.bits = next.bits)) {
	Frame *frame = getObj(txnMap, addr);

	// when we get to the last frame,
	//	pull the count from free head

	if (!frame->prev.bits)
		nSlot = txn->rdrFrame->nslot;
	else
		nSlot = FrameSlots;

	for (idx = 0; idx < nSlot; idx++) {
	  // finish TxnDoc steps

	  if (frameSet) {
		DbAddr *docSlot = fetchIdSlot(docHndl->map, docId);
		uint64_t verNo = frame->slots[idx];

		frameSet = 0;

		// if we also write this read-set mmbr, skip it

		if (*setMmbr(memMap, wrtSet, docId.bits, false))
		  continue;

		doc = getObj(docHndl->map, *docSlot);
		ver = getVersion(docHndl->map, doc, verNo);

		//	keep larger ver pstamp

		compareSwapTs(ver->pstamp, txn->commit, -1);
		continue;
	  }

	  objId.bits = frame->slots[idx];

	  switch (objId.xtra) {
		case TxnKill:
		  continue;

		case TxnHndl:
		  if (docHndl)
			releaseHandle(docHndl, NULL);
		  
	  	  slot = fetchIdSlot(hndlMap, objId);
		  docHndl = getObj(hndlMap, *slot);
		  continue;

		case TxnDoc:
		  docId.bits = objId.bits;
		  frameSet = 0;
		  continue;
		 }
	  }

	next.bits = frame->prev.bits;
	returnFreeFrame(txnMap, addr);
  }

  if (docHndl)
	releaseHandle(docHndl, NULL);

  //  finally install wrt set versions

  next.bits = txn->docFirst->bits;
  docHndl = NULL;
  frameSet = 0;

  while ((addr.bits = next.bits)) {
	Frame *frame = getObj(txnMap, addr);

	// when we get to the last frame,
	//	pull the count from free head

	if (!frame->prev.bits)
		nSlot = txn->docFrame->nslot;
	else
		nSlot = FrameSlots;

	for (idx = 0; idx < nSlot; idx++) {
	  objId.bits = frame->slots[idx];

	  switch (objId.xtra) {
		case TxnKill:
		  continue;

		case TxnHndl:
		  if (docHndl)
			releaseHandle(docHndl, NULL);
		  
	  	  slot = fetchIdSlot(hndlMap, objId);
		  docHndl = getObj(hndlMap, *slot);
		  continue;

		case TxnDoc:
		  slot = fetchIdSlot(docHndl->map, objId);
		  lockLatch(slot->latch);
		  break;
	  }

	  // find previous version

	  doc = getObj(docHndl->map, *slot);
	  ver = (Ver *)((uint8_t *)doc + doc->lastVer);

	  offset = doc->lastVer + ver->verSize;
	  prevVer = (Ver *)((uint8_t *)doc + offset);

	  //	at top end, find in prev doc block

	  if (!prevVer->verSize) {
		if (doc->prevAddr.bits) {
		  Doc *prevDoc = getObj(docHndl->map, doc->prevAddr);
		  prevVer = (Ver *)((uint8_t *)prevDoc + prevDoc->lastVer);
		} else
		  prevVer = NULL;
	  }

	  if (prevVer)
		  installTs(prevVer->sstamp, txn->commit);

	  installTs(ver->commit, txn->commit);
	  installTs(ver->pstamp, txn->commit);
	  ver->sstamp->epoch = ~0ULL;

	  doc->txnId.bits = 0;
	  doc->op = Done;

	  unlockLatch(slot->latch);
	  continue;
	}

	next.bits = frame->prev.bits;
	returnFreeFrame(txnMap, addr);
  }

  if (docHndl)
	releaseHandle(docHndl, NULL);

  return result;
}

//	commit txn under snapshot isolation
//	always succeeds

bool snapshotCommit(Txn *txn) {
DbAddr *slot, addr, next;
Handle *docHndl;
int nSlot, idx;
ObjId objId;
Doc *doc;
Ver *ver;

  next.bits = txn->docFirst->bits;
  docHndl = NULL;

  while ((addr.bits = next.bits)) {
	Frame *frame = getObj(txnMap, addr);

	// when we get to the last frame,
	//	pull the count from free head

	if (!frame->prev.bits)
		nSlot = txn->docFrame->nslot;
	else
		nSlot = FrameSlots;

	for (idx = 0; idx < nSlot; idx++) {
	  objId.bits = frame->slots[idx];

	  switch (objId.xtra) {
		case TxnKill:
		  continue;

		case TxnHndl:
		  if (docHndl)
			releaseHandle(docHndl, NULL);
		  
	  	  slot = fetchIdSlot(hndlMap, objId);
		  docHndl = getObj(hndlMap, *slot);
		  continue;

		case TxnDoc:
		  slot = fetchIdSlot(docHndl->map, objId);
		  lockLatch(slot->latch);
		  break;
	  }

	  doc = getObj(docHndl->map, *slot);
	  ver = (Ver *)((uint8_t *)doc + doc->lastVer);

	  installTs(ver->commit, txn->commit);
	  doc->txnId.bits = 0;
	  doc->op = Done;

	  unlockLatch(slot->latch);

	  //	TODO: add previous doc versions to wait queue
	}

	//  return processed docFirst,
	//	advance to next frame

	next.bits = frame->prev.bits;
	returnFreeFrame(txnMap, addr);
  }

  return true;
}

JsStatus commitTxn(Params *params, uint64_t *txnBits, Timestamp *tsGen) {
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

	newTs(txn->commit, tsGen, false);

	switch (txn->isolation) {
	  case Serializable:
		if (SSNCommit(txn))
			break;

	  case SnapShot:
		snapshotCommit(txn);
		break;

	  default:
		return (JsStatus)OK;
	}

	//	TODO: recycle the txnId

	//	remove nested txn
	//	and unlock

	*txnBits = txn->nextTxn;
	*txn->state = Done;
	return (JsStatus)OK;
}

//	display a txn

value_t fcnTxnToString(value_t *args, value_t thisVal, environment_t *env) {
	char buff[64];
	ObjId txnId;
	int len;

	txnId.bits = thisVal.idBits;

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


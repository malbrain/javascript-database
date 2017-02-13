#include "js.h"
#include "js_db.h"
#include "js_dbindex.h"
#include "database/db_frame.h"

void addVerToTxn(DbMap *database, Txn *txn, Ver *ver, TxnCmd cmd) {
	ObjId docId;

	docId.cmd = cmd;
	addSlotToFrame (database, txn->frame, NULL, docId.bits);
}

//  find appropriate document version per txn beginning timestamp

Ver *findDocVer(DbMap *map, Doc *doc, JsMvcc *jsMvcc) {
uint64_t timestamp, txnTs;
uint32_t offset;
Txn *txn = NULL;

  if (jsMvcc->txnId.bits) {
	txn = fetchIdSlot(map->db, jsMvcc->txnId);
	timestamp = txn->beginTs;
  } else
	timestamp = jsMvcc->ts;

  //	examine prior versions

  do {
	uint32_t offset = doc->lastVer;

	while (offset >= sizeof(Doc)) {
	  Ver *ver = (Ver *)((uint8_t *)doc + offset);

	  // same TXN?

	  if (jsMvcc->txnId.bits && ver->txnId.bits == jsMvcc->txnId.bits)
		return ver;

	  // is version committed before our txn began?

	  if (ver->timestamp) {
		if (ver->timestamp < timestamp)
		  return ver;
	  }

	  if (ver->txnId.bits) {
		Txn *verTxn = fetchIdSlot(map->db, ver->txnId);

		if (isCommitted(verTxn->commitTs))
		  if (verTxn->commitTs < txn->beginTs)
			return ver;

		//	advance txn ts past doc version ts
		//	and move onto next doc version

		while (isReader((txnTs = txn->beginTs)) && txnTs < verTxn->commitTs)
		  compareAndSwap(&txn->commitTs, txnTs, verTxn->commitTs);
	  }

	  offset -= ver->prevSize;
	}
  } while ((doc = doc->prevAddr.bits ? getObj(map, doc->prevAddr) : NULL));

  return NULL;
}

ObjId beginTxn(DbHandle hndl[1], Params *params) {
Handle *database;
ObjId txnId;
Txn *txn;

	txnId.bits = 0;

	if (!(database = bindHandle(hndl)))
		return txnId;

	txnId.bits = allocObjId(database->map, listFree(database,0), listWait(database,0), 0);
	txn = fetchIdSlot(database->map, txnId);
	txn->beginTs = allocateTimestamp(database->map, en_reader);

	releaseHandle(database, hndl);
	return txnId;
}

DbStatus rollbackTxn(DbHandle hndl[1], ObjId txnId) {
	return DB_OK;
}

DbStatus commitTxn(DbHandle hndl[1], ObjId txnId) {
	return DB_OK;
}


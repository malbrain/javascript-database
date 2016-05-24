#include "jsdb.h"
#include "jsdb_db.h"

//	Unexpired database transactions are documents in the database arena.
//	The individual step structure addresses are pushed onto a Frame,
//	where each step DbAddr stores the step collection name and documentId.

//	begin a transaction

uint64_t txnBegin (value_t hndl) {
	DbMap *db = lockHandle(hndl);
	uint32_t set = getSet(db);
	DocId txnId;
	Txn *txn;

	txnId.bits = allocDocId(db, &database(db)->freeTxn[set][Txn_id], NULL);
	txn = fetchIdSlot(db, txnId);
	txn->set = set;

	unlockHandle(hndl);
	return txnId.bits;
}

//	add a step to a transaction

Status txnStep (value_t hndl, DocId txnId, DocId docId, TxnStepType type) {
	DbMap *docStore = lockHandle(hndl);
	DbMap *db = docStore->parent;
	Txn *txn = fetchIdSlot(db, txnId);
	uint32_t set = txn->set;
	TxnStep *txnStep;
	DbAddr step;

	if ((step.bits = allocObj(db, &database(db)->freeTxn[set][Txn_step], type, sizeof(TxnStep), false)))
		txnStep = getObj(db, step);
	else
		return ERROR_outofmemory;

	txnStep->hndlIdx = docStore->arena->hndlIdx;
	txnStep->docId.bits = docId.bits;
	unlockHandle(hndl);

	if (addSlotToFrame(db, txn->txnFrame, step.bits))
		return OK;

	return ERROR_outofmemory;
}

// TODO -- implement commit/rollback

Status txnRollback(value_t hndl, DocId txnId) {
	DbMap *db = lockHandle(hndl);
	Txn *txn = fetchIdSlot(db, txnId);
	uint32_t set = txn->set;
	uint64_t addr;
	DbAddr slot;

	addr = txn->txnFrame->bits;

	while (addr) {
		Frame *frame = getObj(db, *txn->txnFrame);
		while (txn->txnFrame->nslot)
			addSlotToFrame(db, &database(db)->freeTxn[set][Txn_step], frame->slots[--txn->txnFrame->nslot].bits);

		addr = frame->next.bits;
		returnFreeFrame(db, *txn->txnFrame);
	}

	memset(txn, 0, sizeof(Txn));

	slot.bits = txnId.bits;
	addSlotToFrame(db, &database(db)->freeTxn[set][Txn_id], slot.bits);
	unlockHandle(hndl);
	return OK;
}

Status txnCommit(value_t hndl, DocId txnId) {
	DbMap *db = lockHandle(hndl);
	Txn *txn = fetchIdSlot(db, txnId);
	uint32_t set = txn->set;
	uint64_t addr;

	addr = txn->txnFrame->bits;

	while (addr) {
		Frame *frame = getObj(db, *txn->txnFrame);
		while (txn->txnFrame->nslot)
			addSlotToFrame(db, &database(db)->freeTxn[set][Txn_step], frame->slots[--txn->txnFrame->nslot].bits);

		addr = frame->next.bits;
		returnFreeFrame(db, *txn->txnFrame);
	}

	memset(txn, 0, sizeof(Txn));

	addSlotToFrame(db, &database(db)->freeTxn[set][Txn_id], txnId.bits);
	unlockHandle(hndl);
	return OK;
}

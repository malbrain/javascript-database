#include "jsdb.h"
#include "jsdb_db.h"

//	Unexpired database transactions are documents in the database arena.
//	The individual step structure addresses are pushed onto a Frame,
//	where each step DbAddr stores the step collection name and documentId.

//	begin a transaction

uint64_t txnBegin (DbMap *db) {
	uint32_t set = getSet(db);
	DocId txnId;
	Txn *txn;

	txnId.bits = allocDocId(db, &database(db)->freeTxn[set][Txn_id], NULL);
	txn = fetchIdSlot(db, txnId);
	txn->set = set;

	return txnId.bits;
}

//	add a step to a transaction

Status txnStep (DbMap *docStore, DocId txnId, DocId docId, TxnStepType type) {
	RedBlack *entry = docStore->entry;
	DbMap *db = docStore->parent;
	Txn *txn = fetchIdSlot(db, txnId);
	uint32_t set = txn->set;
	TxnStep *txnStep;
	ChildMap *child;
	DbAddr step;

	child = (ChildMap *)(entry->key + entry->keyLen);
	if ((step.bits = allocObj(db, &database(db)->freeTxn[set][Txn_step], type, sizeof(TxnStep), false)))
		txnStep = getObj(db, step);
	else
		return ERROR_outofmemory;

	txnStep->hndlId = child->id;
	txnStep->docId.bits = docId.bits;

	if (addSlotToFrame(db, txn->txnFrame, step.bits))
		return OK;

	return ERROR_outofmemory;
}

// TODO -- implement commit/rollback

Status txnRollback(DbMap *db, uint64_t txnBits) {
	uint64_t addr;
	DocId txnId;
	DbAddr slot;
	Txn *txn;

	txnId.bits = txnBits;
	txn = fetchIdSlot(db, txnId);
	addr = txn->txnFrame->bits;

	while (addr) {
		Frame *frame = getObj(db, *txn->txnFrame);
		while (txn->txnFrame->nslot)
			addSlotToFrame(db, &database(db)->freeTxn[txn->set][Txn_step], frame->slots[--txn->txnFrame->nslot].bits);

		addr = frame->next.bits;
		returnFreeFrame(db, *txn->txnFrame);
	}

	memset(txn, 0, sizeof(Txn));

	slot.bits = txnId.bits;
	addSlotToFrame(db, &database(db)->freeTxn[txn->set][Txn_id], slot.bits);
	return OK;
}

Status txnCommit(DbMap *db, uint64_t txnBits) {
	uint64_t addr;
	DocId txnId;
	Txn *txn;

	txnId.bits = txnBits;
	txn = fetchIdSlot(db, txnId);
	addr = txn->txnFrame->bits;

	while (addr) {
		Frame *frame = getObj(db, *txn->txnFrame);
		while (txn->txnFrame->nslot)
			addSlotToFrame(db, &database(db)->freeTxn[txn->set][Txn_step], frame->slots[--txn->txnFrame->nslot].bits);

		addr = frame->next.bits;
		returnFreeFrame(db, *txn->txnFrame);
	}

	memset(txn, 0, sizeof(Txn));

	addSlotToFrame(db, &database(db)->freeTxn[txn->set][Txn_id], txnId.bits);
	return OK;
}

#include "jsdb.h"
#include "jsdb_db.h"
#include "jsdb_dbtxn.h"

uint64_t startTxn(DbMap *map, DocId docId, enum TxnStep step) {
	DbAddr txnAddr, slot;
	Frame *frame;

	slot.bits = step;
	slot.addr = docId.addr;

	txnAddr.bits = allocFrame(map);
	txnAddr.nslot = 1;

	frame = getObj(map, txnAddr);
	frame->slots[0].bits = slot.bits;
	frame->next.bits = 0;
	frame->prev.bits = 0;

	return txnAddr.bits;
}

bool addTxnStep(DbMap *map, DbAddr *head, uint8_t *keyBuff, int keyLen, enum TxnStep step, uint32_t set) {
	uint32_t bits = 3, amt = sizeof(TxnStep) + keyLen;
	DbAddr slot, *free, *tail;
	TxnStep *txnStep;

	while ((1UL << bits) < amt)
		bits++;

	free = docStoreAddr(map)->waitLists[set][bits].free;
	tail = docStoreAddr(map)->waitLists[set][bits].tail;

	if ((slot.bits = allocObj(map, free, tail, step, 1UL << bits)))
		slot.nbits = bits;
	else
		return false;

	txnStep = getObj(map, slot);
	txnStep->timestamp = -1ULL;
	txnStep->keyLen = keyLen;
	memcpy(txnStep->key, keyBuff, keyLen);

	return addNodeToFrame(map, head, NULL, slot);
}

Status rollbackTxn(DbMap *map, DbDoc *doc) {
	return OK;
}
Status commitTxn(DbMap *map, DbDoc *doc) {
	return OK;
}

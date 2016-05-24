#include "jsdb.h"
#include "jsdb_db.h"

// place write, read, or parent lock on requested page_no.

void btreeLockPage(BtreeLatch *latch, BtreeLock mode) {
	switch( mode ) {
	case Btree_lockRead:
		readLock (latch->readwr);
		break;
	case Btree_lockWrite:
		writeLock (latch->readwr);
		break;
	case Btree_lockAccess:
		readLock (latch->access);
		break;
	case Btree_lockDelete:
		writeLock (latch->access);
		break;
	case Btree_lockParent:
		writeLock (latch->parent);
		break;
	case Btree_lockLink:
		writeLock (latch->link);
		break;
	}
}

void btreeUnlockPage(BtreeLatch *latch, BtreeLock mode)
{
	switch( mode ) {
	case Btree_lockWrite:
		rwUnlock (latch->readwr);
		break;
	case Btree_lockRead:
		rwUnlock (latch->readwr);
		break;
	case Btree_lockAccess:
		rwUnlock (latch->access);
		break;
	case Btree_lockDelete:
		rwUnlock (latch->access);
		break;
	case Btree_lockParent:
		rwUnlock (latch->parent);
		break;
	case Btree_lockLink:
		rwUnlock (latch->link);
		break;
	}
}

//	Assign and pin latch table entry

BtreeLatch *btreePinLatch (DbMap *index, DbAddr pageNo) {
	int hashIdx = pageNo.bits % Btree_hashsize;
	BtreeIndex *btree = btreeIndex(index);
	BtreeLatch *latch;
	DbAddr addr;

	if (pageNo.addr == btree->root.addr)
		return btree->rootLatch;

	lockLatch(btree->hashTable[hashIdx].latch);

	if ((addr.bits = btree->hashTable[hashIdx].addr)) do {
		latch = getObj(index, addr);
		if (latch->pageNo.bits == pageNo.bits) {
			latch->pinCnt++;
			unlockLatch(btree->hashTable[hashIdx].latch);
			return latch;
		}
	} while ((addr.bits = latch->next.bits));

	//  our page isn't in the hash table,
	//	make a new entry

	if ((addr.bits = allocObj(index, &btree->freePages[Btree_latchSet], Btree_latchSet, sizeof(BtreeLatch), true) ))
		latch = getObj(index, addr);
	else
		return NULL;

	latch->next.bits = btree->hashTable[hashIdx].addr;
	latch->pageNo.bits = pageNo.bits;
	latch->addr.bits = addr.bits;
	latch->pinCnt = 1;

	//  place latch into hashtable, removing lock

	btree->hashTable[hashIdx].bits = addr.bits;
	return latch;
}

//	unpin latch set and remove if zero

Status btreeUnpinLatch (DbMap *index, BtreeLatch *latch) {
	int hashIdx = latch->pageNo.bits % Btree_hashsize;
	BtreeIndex *btree = btreeIndex(index);
	BtreeLatch *prevEntry, *nextEntry;
	DbAddr addr;

	if (latch->pageNo.addr == btree->root.addr)
		return OK;

	lockLatch(btree->hashTable[hashIdx].latch);

	if (--latch->pinCnt) {
		unlockLatch(btree->hashTable[hashIdx].latch);
		return OK;
	}

	prevEntry = NULL;

	if ((addr.bits = btree->hashTable[hashIdx].addr)) do {
		nextEntry = getObj(index, addr);
		if (nextEntry == latch) {
			if (prevEntry)
				prevEntry->next.bits = latch->next.bits;
			else
				btree->hashTable[hashIdx].bits = latch->next.bits;

			unlockLatch(btree->hashTable[hashIdx].latch);

			if (addSlotToFrame(index, &btree->freePages[Btree_latchSet], addr.bits))
				return OK;
			return ERROR_outofmemory;
		}
		prevEntry = nextEntry;
	} while ((addr.bits = nextEntry->next.bits));

	//	latch not found error

	return ERROR_btreepagelatchnotfound;
}

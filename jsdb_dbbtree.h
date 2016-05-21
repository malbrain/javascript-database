#pragma once

#include "jsdb_rwlock.h"

#define Btree_maxbits		29					// maximum page size in bits
#define Btree_minbits		9					// minimum page size in bits
#define Btree_minpage		(1 << Btree_minbits)	// minimum page size
#define Btree_maxpage		(1 << Btree_maxbits)	// maximum page size
#define Btree_hashsize		4093

//	There are six lock types for each node in four independent sets: 
//	1. (set 1) AccessIntent: Sharable. Going to Read the node. Incompatible with NodeDelete. 
//	2. (set 1) NodeDelete: Exclusive. About to release the node. Incompatible with AccessIntent. 
//	3. (set 2) ReadLock: Sharable. Read the node. Incompatible with WriteLock. 
//	4. (set 2) WriteLock: Exclusive. Modify the node. Incompatible with ReadLock and other WriteLocks. 
//	5. (set 3) ParentModification: Exclusive. Change the node's parent keys. Incompatible with another ParentModification. 
//	6. (set 4) LinkModification: Exclusive. Update of a node's left link is underway. Incompatible with another LinkModification. 

typedef enum {
	Btree_lockAccess = 1,
	Btree_lockDelete = 2,
	Btree_lockRead   = 4,
	Btree_lockWrite  = 8,
	Btree_lockParent = 16,
	Btree_lockLink   = 32
} BtreeLock;

//  The page latches are brought together
//	for post-crash evaluation and recovery

typedef struct {
	DbAddr addr;		// entry address in index map
	DbAddr next;		// next entry in hash table chain
	DbAddr pageNo;		// page number assigned to this entry
	uint32_t pinCnt;	// pin count of threads/processes
	RWLock readwr[1];	// read/write access lock
	RWLock access[1];	// waiting for delete lock
	RWLock parent[1];	// posting of fence key
	RWLock link[1];		// left link update
} BtreeLatch;

//	types of btree pages/allocations

typedef enum{
	Btree_rootPage,
	Btree_interior,
	Btree_leafPage,
	Btree_latchSet,
	Btree_maxType
} BtreePageType;

//	BtreeIndex overlay of DbIndex type

typedef struct {
	DbIndex index[1];		// index definition documents (must be first)
	DbAddr root, leaf;		// btree root and first leaf
	uint32_t pageSize;
	uint32_t pageBits;
	uint32_t leafXtra;
	uint64_t numEntries[1];	// number of keys in btree
	BtreeLatch rootLatch[1];
	DbAddr freePages[Btree_maxType];// free page list
	DbAddr hashTable[Btree_hashsize];
} BtreeIndex;

//	Btree page layout

//	This structure is immediately
//	followed by the key slots

typedef struct {
	uint32_t cnt;		// count of keys in page
	uint32_t act;		// count of active keys
	uint32_t min;		// next page key offset
	uint32_t garbage;	// page garbage in bytes
	uint8_t lvl:6;		// level of page
	uint8_t free:1;		// page is on free chain
	uint8_t kill:1;		// page is being deleted
	DbAddr right;		// page to right
	DbAddr left;		// page to left
} BtreePage;

typedef struct {
	DbAddr pageNo;		// current page address
	BtreePage *page;	// selected page
	BtreeLatch *latch;	// latch set for page
	uint32_t slotIdx;	// slot on page
} BtreeSet;

//	Page key slot definition.

//	Keys are marked dead, but remain on the page until
//	it cleanup is called.

//	Slot types

//	In addition to the Unique keys that occupy slots
//	there are Librarian slots in the key slot array.

//	The Librarian slots are dead keys that
//	serve as filler, available to add new keys.

typedef enum {
	Btree_indexed,		// key was indexed
	Btree_deleted,		// key was deleted
	Btree_librarian,	// librarian slot
	Btree_stopper		// stopper slot
} BtreeSlotType;

typedef union {
	struct {
		uint32_t off:Btree_maxbits;	// page offset for key start
		uint32_t type:2;			// type of key slot
		uint32_t dead:1;			// dead/librarian slot
	};
	uint32_t bits;
} BtreeSlot;

//	BtreeCursor with a cached leaf page
//	from either the main and cache trees

typedef struct {
	uint64_t refCnt[1];				// handle reference count
	uint64_t timestamp;				// cursor snapshot timestamp
	value_t indexHndl;				// handle for the index
	BtreePage *page;				// cached btree page
	DbAddr pageAddr;				// cached btree pageNo
	DbAddr pqAddr;					// priority queue handle
	uint32_t slotIdx;				// current cache index
} BtreeCursor;

#define btreeIndex(index) ((BtreeIndex *)(index->arena + 1))

value_t btreeCursor(value_t indexHndl, bool origin, value_t fields, value_t limits);
value_t btreeCursorKey(BtreeCursor *cursor);

uint64_t btreeNewPage (DbMap *index, uint8_t lvl);
DbAddr *btreeFindKey(DbMap  *map, BtreeCursor *cursor, uint8_t *key, uint32_t keylen);
bool btreeSeekKey(BtreeCursor *cursor, uint8_t *key, uint32_t keylen);
bool btreeNextKey (BtreeCursor *cursor);
bool btreePrevKey (BtreeCursor *cursor);
uint64_t btreeDocId(BtreeCursor *cursor);

#define slotptr(page, slot) (((BtreeSlot *)(page+1)) + (((int)slot)-1))

#define keyaddr(page, off) ((uint8_t *)((unsigned char*)(page) + off))
#define keyptr(page, slot) ((uint8_t *)((unsigned char*)(page) + slotptr(page, slot)->off))
#define keylen(key) ((key[0] & 0x80) ? ((key[0] & 0x7f) << 8 | key[1]) : key[0])
#define keystr(key) ((key[0] & 0x80) ? (key + 2) : (key + 1))
#define keypre(key) ((key[0] & 0x80) ? 2 : 1)

Status btreeInit(DbMap *map);
Status btreeIndexKey (DbMap *index, DbDoc *doc, DocId docId);
Status btreeInsertKey(DbMap *index, uint8_t *key, uint32_t keyLen, uint8_t lvl, BtreeSlotType type);
Status btreeLoadPage(DbMap *index, BtreeSet *set, uint8_t *key, uint32_t keyLen, uint8_t lvl, BtreeLock lock, bool stopper);
Status btreeCleanPage(DbMap *index, BtreeSet *set, uint32_t totKeyLen);
Status btreeSplitPage (DbMap *index, BtreeSet *set);
Status btreeFixKey (DbMap *index, uint8_t *fenceKey, uint8_t lvl, bool stopper);

void btreeLockPage(BtreeLatch *latch, BtreeLock mode);
void btreeUnlockPage(BtreeLatch *latch, BtreeLock mode);

BtreeLatch *btreePinLatch (DbMap *index, DbAddr pageNo);
Status btreeUnpinLatch (DbMap *index, BtreeLatch *entry);

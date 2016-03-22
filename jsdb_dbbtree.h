#pragma once

#include "jsdb_rwlock.h"

typedef enum{
	BtLockAccess = 1,
	BtLockDelete = 2,
	BtLockRead   = 4,
	BtLockWrite  = 8,
	BtLockParent = 16,
	BtLockLink   = 32
} BtreeLock;

//	permanent BtreeIndex on disk

typedef struct {
	DbAddr rootPage;			// address of page level zero
	uint32_t pageSize;			// size of pages in bytes
	uint32_t pageBits;			// size of pages in bits
	uint64_t numEntries[1];		// number of keys in btree
	DbAddr freePages[MAX_set];	// free page list
} BtreeIndex;

//	Btree page on disk and cached in cursor

typedef struct {
	uint32_t cnt;				// count of keys in page
	uint32_t act;				// count of active keys
	uint32_t min;				// next key offset
	uint32_t garbage;			// page garbage in bytes
	unsigned char bits:7;		// page size in bits
	unsigned char free:1;		// page is on free chain
	unsigned char lvl:7;		// level of page
	unsigned char kill:1;		// page is being deleted
	RWLock readwr[1];			// read/write access
	RWLock access[1];			// waiting for delete lock
	RWLock parent[1];			// posting of fence key
	RWLock link[1];				// left link update in progress
	DbAddr right;				// page to right
	DbAddr left;				// page to left
} BtreePage;

//	BtreeCursor with cached leaf page following

typedef struct {
	uint64_t timestamp;
	uint32_t slot;
	bool direction;
	DbMap *index;
	BtreePage page[0];
} BtreeCursor;

//  Btree index local storage w/
//	temporary page frame following

typedef struct {
	uint64_t idxListVer;		// last check of created indexes
	DbAddr idxList;				// list of created indexes
	BtreePage frame[0];
} BtreeLocal;

#define btreeIndexAddr(map)((BtreeIndex *)(map->arena + 1))
#define btreeCursorPage(cursor)((BtreePage *)(cursor + 1))
#define btreeLocalData(map)((BtreeLocal *)(map + 1))

value_t btreeCursor(DbMap *btree, bool origin);
value_t btreeCursorKey(BtreeCursor *cursor);

DbAddr *btreeFindKey(DbMap  *map, BtreeCursor *cursor, uint8_t *key, uint32_t keylen);
bool btreeSeekKey(BtreeCursor *cursor, uint8_t *key, uint32_t keylen);

void btreeInit(DbMap *index);

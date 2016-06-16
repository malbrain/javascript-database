#pragma once

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include "jsdb_dbpq.h"

#define MAX_segs  1000
#define MIN_segsize  131072

#define MAX_path  4096
#define MAX_blk		24	// max arena blk size in bits

//  on disk segment

typedef struct {
	uint64_t off;		// file offset of the segment
	uint64_t size;		// size of the segment
	DocId nextDoc;		// highest document ID in use
} DbSeg;

//  on disk/mmap arena in seg zero

typedef struct {
	DbSeg segs[MAX_segs]; 	// segment meta-data
	DbAddr freeBlk[MAX_blk];// Arena free block frames
	DbAddr freeFrame[1];	// next free frame address
	DbAddr nextObject;		// next Object address
	DbAddr childRoot[1];	// red/black tree of active child names
	DbAddr childIdRoot[1];	// red/black tree of accessable child Id
	uint64_t arenaGUID[2];	// arena parent/child match-up GUID
	RWLock childIdLock[1];	// latch for accessing child ID tree
	RWLock childLock[1];	// latch for accessing child trees
	uint64_t childId;		// highest child ID issued
	uint8_t currSeg;		// index of highest segment
	uint8_t idSize;			// size of the Id array element
	char mutex;				// object allocation lock
	char type;				// arena hndl type
	char drop;				// arena dropped
} DbArena;

//  in memory arena maps

struct DbMap_ {
	char *base[MAX_segs];	// pointers to mapped segment memory
	DbAddr hndlTree[1];		// red/black tree of child handles
	RWLock hndlLock[1];		// latch for accessing child handles
#ifndef _WIN32
	int hndl[1];			// OS file handle
#else
	HANDLE hndl[MAX_segs];
	HANDLE maphndl[MAX_segs];
#endif
	struct RedBlack *entry;	// our child entry in parent
	uint32_t cpuCount;		// number of CPUS
	uint32_t maxSeg;		// maximum segment array index in use
	DbArena *arena;			// ptr to mapped seg zero
	DbMap *parent;			// parent map
	DbMap *db;				// database
	char created;			// new arena file created
	char onDisk;			// on disk bool flag
	char mutex;				// mapping lock
};

//	the database arena

typedef struct {
	DbPQ pq[1];					// timestamp priority queue
	uint32_t verCnt;			// version for handle names
	uint32_t maxVer;			// maximum version table idx
	DbAddr freePQ[MAX_set]; 	// available priority queue entries
	DbAddr freeTxn[MAX_set][Txn_max];
} DataBase;

#define database(db) ((DataBase *)(db->arena + 1))

value_t createMap(value_t name, DbMap *parent, uint32_t baseSize, uint64_t initSize, bool onDisk);
void returnFreeFrame(DbMap *map, DbAddr slot);
uint64_t allocBlk (DbMap *map, uint32_t size);

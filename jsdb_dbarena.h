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
	DbSeg segs[MAX_segs]; 		// segment meta-data
	DbAddr freeBlk[MAX_blk];	// Arena free block frames
	DbAddr freeFrame[1];		// next free frame address
	DbAddr nextObject;			// next Object address
	DbAddr childChange;			// list of changed child slots
	DbAddr childSlots;			// list of available child slots
	DbAddr childRoot;			// red/black tree of active children
	RWLock childLock[1];		// latch for accessing child lists
	uint32_t childIdx;			// highest child List handle idx
	uint32_t childCnt;			// number of children
	uint32_t hndlIdx;			// our idx in parent's handle array
	uint32_t idSize;			// docId/txnId element size
	char currSeg;				// index of highest segment
	char mutex;					// object allocation lock
	char type;					// arena hndl type
	char drop;					// arena dropped
} DbArena;

//  in memory arena maps

struct DbMap_ {
	uint64_t refCnt[1];		// handle reference count (must be first)
	char *base[MAX_segs];	// pointers to mapped segment memory
#ifndef _WIN32
	int hndl[1];			// OS file handle
#else
	HANDLE hndl[MAX_segs];
	HANDLE maphndl[MAX_segs];
#endif
	uint32_t cpuCount;		// number of CPUS
	uint32_t maxSeg;		// maximum segment array index in use
	DbArena *arena;			// pointer to first part of seg zero
	value_t hndls;			// base of handle array
	value_t name;			// arena name
	DbMap *parent;			// parent map
	DbMap *db;				// database
	char created;			// new arena file created
	char onDisk;			// on disk bool flag
	char mutex;				// mapping lock
};

//	child red/black tree entry

typedef struct {
	uint32_t nameLen, hndlIdx;	// handle index assigned
	DbAddr next, left, right;	// next nodes down
	char red;					// is tree node red?
	char name[1];				// name zero terminator
} RedBlack;

//	the database arena

typedef struct {
	DbPQ pq[1];					// timestamp priority queue
	uint32_t verCnt;			// version for handle names
	uint32_t maxVer;			// maximum version table idx
	DbAddr freePQ[MAX_set]; 	// available priority queue entries
	DbAddr freeTxn[MAX_set][Txn_max];
} DataBase;

#define database(db) ((DataBase *)(db->arena + 1))

DbMap *createMap(value_t name, DbMap *parent, uint32_t baseSize, uint64_t initSize, bool onDisk);
DbMap *openMap(value_t name, DbMap *parent, uint32_t hndlIdx);
void returnFreeFrame(DbMap *map, DbAddr slot);
uint64_t allocBlk (DbMap *map, uint32_t size);

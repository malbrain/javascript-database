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

//	name ID LIFO list

typedef struct {
	DbAddr next;	// next entry in ID LIFO list
	uint64_t id;	// entry auto-incrementing ID
	uint64_t ref;	// entry delete reference (0 == add)
	uint32_t len;	// length of name
	uint8_t name[0];// name characters
} IdEntry;

//  on disk arena segment

typedef struct {
	uint64_t off;		// file offset of the segment
	uint64_t size;		// size of the segment
	DocId nextDoc;		// highest document ID in use
} DbSeg;

//  on disk/mmap arena seg zero

typedef struct {
	DbSeg segs[MAX_segs]; 	// segment meta-data
	DbAddr freeBlk[MAX_blk];// Arena free block frames
	DbAddr freeFrame[1];	// next free frame address
	DbAddr nextObject;		// next Object address
	DbAddr childIdRoot;		// child Id LIFO list, no read lock
	uint64_t childId;		// highest child ID issued
	uint32_t idSize;		// size of the Id array element
	uint8_t currSeg;		// index of highest segment
	char mutex;				// object allocation lock
	char type;				// arena hndl type
	char drop;				// arena dropped
} DbArena;

//  in memory children object

typedef struct {
	uint64_t childId;		// child id
	DbMap *child;			// loaded map
} DbChild;

//	handle value types

typedef enum {
	Hndl_newarena = 0,
	Hndl_database,
	Hndl_docStore,
	Hndl_btreeIndex,
	Hndl_artIndex,
	Hndl_colIndex,
	Hndl_iterator,
	Hndl_btreeCursor,
	Hndl_artCursor,
	Hndl_docVersion
} HandleSubType;

//	user's arena handle

typedef struct {
	RWLock latch[1];	// handle use counter
	uint64_t id;		// child ID in parent
	DbMap *map;			// arena -- NULL if dropped
	void *next;			// next handle for arena
} Handle;
	
//	in memory arena map

struct DbMap_ {
	char *base[MAX_segs];	// pointers to mapped segment memory
	value_t children[1];	// object containing child handles
	uint64_t childId;		// highest child id in children
#ifndef _WIN32
	int hndl[1];			// OS file handle
#else
	HANDLE hndl[MAX_segs];
	HANDLE maphndl[MAX_segs];
#endif
	DbMap *parent, *db;		// parent and database map
	IdEntry *myEntry;		// pointer to IdEntry in parent
	uint32_t cpuCount;		// number of CPUS
	uint32_t maxSeg;		// maximum segment array index in use
	DbArena *arena;			// ptr to mapped seg zero
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

DbMap *createMap(IdEntry *entry, DbMap *parent, uint32_t baseSize, uint64_t initSize, bool onDisk);
void returnFreeFrame(DbMap *map, DbAddr slot);
uint64_t allocBlk (DbMap *map, uint32_t size);

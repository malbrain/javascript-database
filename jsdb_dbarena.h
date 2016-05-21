#pragma once

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include "jsdb_dbpq.h"

#define MAX_segs  1000

#define MIN_segsize  131072
#define MAX_path  4096

//  on disk segment

typedef struct {
	uint64_t off;		// file offset of the segment
	uint64_t size;		// size of the segment
	DocId nextDoc;		// highest document ID in use
} DbSeg;

//  on disk/mmap arena in seg zero

typedef struct {
	DbSeg segs[MAX_segs]; 	// segment meta-data
	DbAddr freeFrame[1];	// next free frame address
	DbAddr nextObject;		// next Object address
	DbAddr childList;		// linked list of children names
	RWLock childLock[1];	// latch for accessing child list
	uint64_t childSeq;		// sequence number for child list
	uint32_t childCnt;		// number of children
	uint32_t hndlIdx;		// index in parent's handle array
	uint32_t idSize;		// docId/txnId element size
	char currSeg;			// index of highest segment
	char mutex;				// object allocation lock
	char type;				// arena hndl type
	char drop;				// arena dropped
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
	DbMap *parent;			// parent map for group
	DbMap *child;			// first child pointer
	DbMap *next;			// next group sibling
	DbMap *db;				// pointer to database
	object_t *names;		// child handle by name map
	array_t *hndls;			// ptr to handle array
	value_t name;			// arena name
	char created;			// new arena file created
	char onDisk;			// on disk bool flag
	char mutex;				// mapping lock
};

//	child list entry

typedef struct {
	uint64_t refCnt[1];		// number of txn entries
	uint32_t hndlVer;		// handle table version
	uint32_t hndlIdx;		// handle table index
	DbAddr next;			// next name in list
	char name[1];			// allocate zero terminator
} HndlNameList;

//	the database arena

typedef struct {
	DbPQ pq[1];					// timestamp priority queue
	uint32_t verCnt;			// version for handle names
	uint32_t maxVer;			// maximum version table idx
	DbAddr hndlList;			// first handle in chain
	DbAddr freeHndlIdx[1];		// free handle index numbers
	DbAddr freePQ[MAX_set]; 	// available priority queue entries
	DbAddr freeName[MAX_name];	// free handle namelist entries
	DbAddr freeTxn[MAX_set][Txn_max];
} DataBase;

#define database(db) ((DataBase *)(db->arena + 1))

DbMap *createMap(value_t name, DbMap *parent, uint32_t baseSize, uint64_t initSize, bool onDisk);
DbMap *openMap(value_t name, DbMap *parent);
void returnFreeFrame(DbMap *map, DbAddr slot);

#pragma once

#include "jsdb_dbpq.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#define MAX_segs  1024

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
	uint16_t localSize;		// amount of local memory
	uint16_t currSeg;		// index of highest segment
	DbPQ pq[1];				// timestamp priority queue
	char onDisk;			// arena is on disk
	char mutex;				// object allocation lock
	char type;				// arena hndl type
	char drop;				// arena dropped
} DbArena;

//  in memory arena maps

struct DbMap_ {
#ifndef _WIN32
	int hndl[1];			// OS file handle
#else
	HANDLE hndl[MAX_segs];
	HANDLE maphndl[MAX_segs];
#endif
	char *base[MAX_segs];	// pointers to mapped segment memory
	struct DbMap_ *parent;	// parent map for group
	struct DbMap_ *child;	// first child pointer
	struct DbMap_ *next;	// next group sibling
	uint32_t cpuCount;		// number of CPUS
	uint32_t maxSeg;		// maximum segment array index in use
	DbArena *arena;			// pointer to first part of seg zero
	value_t name;			// arena name
	char created;			// new arena file created
	char onDisk;			// on disk bool flag
	char mutex;				// mapping lock
};

//	child list entry

typedef struct {
	uint64_t seq;			// list sequence number
	DbAddr next;			// next name in list
	uint8_t name[1];		// allocate zero terminator
} NameList;

DbMap *createMap(value_t name, DbMap *parent, uint32_t baseSize, uint32_t localSize, uint64_t initSize, bool onDisk);
DbMap *openMap(value_t name, DbMap *parent);

#pragma once

#include "jsdb_pq.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#endif

#define MAX_segs  1024

#define SEGZERO_size (16 * 1024 * 1024)

//  on disk segment

typedef struct {
	uint64_t offset;	// offset of the segment
	uint64_t segSize;	// size of the segment
	DocId nextDoc;		// highest document ID in use
} DbSeg;

//  on disk/mmap arena in seg zero

typedef struct {
	DbSeg segs[MAX_segs]; 	// segment meta-data
	DbAddr nextObject;		// next Object address
	DbAddr freeFrame;		// next free frame address
	uint16_t currSeg;		// index of highest segment
	uint8_t maxDbl;			// maximum segment exponent
	char mutex[1];			// object allocation lock
	DbPQ pq[1];				// timestamp priority queue
	char drop;				// arena dropped
} DbArena;

//  in memory arena maps

struct DbMap_ {
#ifdef _WIN32
	HANDLE hndl, maphndl[MAX_segs];
#else
	int hndl;
#endif
	char *base[MAX_segs];	// pointers to mapped segment memory
	uint32_t cpuCount;		// number of CPUS
	uint32_t maxSeg;		// maximum segment array index in use
	DbArena *arena;			// pointer to first part of seg zero
	char mutex[1];			// mapping lock
	char inMem[1];			// in memory bool
	char fName[1];			// zero terminator
};


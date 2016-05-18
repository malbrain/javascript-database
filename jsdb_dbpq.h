#pragma once

/**
 *  data structure to be embedded in target
 */

typedef struct {
	DbAddr next;		// next priority queue entry
	DbAddr prev;		// prev priority queue entry
	uint64_t value;		// timestamp value
	uint32_t set;
} PQEntry;

/**
 *  doubly linked list of DbPQ::Entries
 */

typedef struct {
	uint64_t minValue;	// minimum queue timestamp
	DbAddr queueHead;	// head of the priority queue
	char mutex[1];		// queue manipultion lock
} PQList;

/*
 * Thread-safe bag of timestamps, with getMin()
 */

typedef struct {
	PQList entryLists[MAX_set];  // EntryList per set
	volatile uint64_t pqTime[1]; // the priority queue timestamp
	uint64_t globalMin;			 // minimum of the EntryList minimums
	uint32_t cpuCount;			 // count of cpus
	char mutex[1];				 // latch for computeBagMin
} DbPQ;


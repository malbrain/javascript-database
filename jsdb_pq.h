#pragma once

/**
 *  data structure to be embedded in target
 */

struct Entry_ {
    struct Entry_* next;
    struct Entry_* prev;
    uint64_t value;
    uint32_t set;
};

/**
 *  doubly linked list of DbPQ::Entries
 */

typedef struct {
    Entry* head;
    uint64_t minValue;
    char mutex[1];
} EntryList;

/*
 * Thread-safe bag of timestamps, with getMin()
 */

typedef struct {
    EntryList entryLists[MAX_set];  // EntryList per set
    volatile uint64_t pqTime[1];	// the priority queue timestamp
    uint64_t globalMin;             // minimum of the EntryList minimums
	uint32_t cpuCount;				// count of cpus
    char mutex[1];                  // latch for computeBagMin
} DbPQ;


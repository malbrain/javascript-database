#include "jsdb.h"
#include "jsdb_db.h"

bool isReader(uint64_t ts) {
	return !(ts & 1);
}

bool isWriter(uint64_t ts) {
	return (ts & 1);
}

void computePQMin(DbPQ *pq) {
	uint64_t min = 0;
	uint32_t i;

	lockLatch(pq->mutex);
	min--;

	for (i=0; i < pq->cpuCount; ++i) {
		if (pq->entryLists[i].minValue)
			if (min > pq->entryLists[i].minValue)
				min = pq->entryLists[i].minValue;
	}

	pq->globalMin = min;
	unlockLatch(pq->mutex);
}

uint64_t allocateTimestamp(DbMap *map, enum ReaderWriterEnum e) {
	uint64_t ts = *map->arena->pq->pqTime;

	if (!ts)
		ts = atomicAdd64(map->arena->pq->pqTime, 1);

	switch (e) {
	case en_reader:
		while (!isReader(ts))
			ts = atomicAdd64(map->arena->pq->pqTime, 1);
		break;
	case en_writer:
		while (!isWriter(ts))
			ts = atomicAdd64(map->arena->pq->pqTime, 1);
		break;

	default: break;
	}

	return ts;
}

void addPQEntry(DbMap *map, uint32_t set, Entry* entry, enum ReaderWriterEnum e) {
	lockLatch((char *)map->arena->pq->entryLists[set].mutex);
	entry->value = allocateTimestamp(map, e);
	entry->set = set;
	entry->prev = 0;

	if ( (entry->next = map->arena->pq->entryLists[set].head) ) {
		Entry* next = map->arena->pq->entryLists[set].head;
		next->prev = entry;
	} else
		map->arena->pq->entryLists[set].minValue = entry->value;

	map->arena->pq->entryLists[set].head = entry;

	if (map->arena->pq->globalMin > entry->value)
		computePQMin(map->arena->pq);

	unlockLatch((char *)map->arena->pq->entryLists[set].mutex);
}

void removePQEntry(DbMap *map, Entry* entry) {

	lockLatch(map->arena->pq->entryLists[entry->set].mutex);
	Entry* prev = entry->prev;
	Entry* next = entry->next;

	if (next)
		next->prev = prev;
	else if (prev)
		map->arena->pq->entryLists[entry->set].minValue = prev->value;
	else
		map->arena->pq->entryLists[entry->set].minValue = 0;

	if (prev)
		prev->next = next;
	else
		map->arena->pq->entryLists[entry->set].head = next;

	unlockLatch(map->arena->pq->entryLists[entry->set].mutex);
}

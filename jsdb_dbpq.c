#include "jsdb.h"
#include "jsdb_db.h"
#include "jsdb_dbpq.h"

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

uint64_t getTimestamp(DbMap *map, DbAddr addr) {
	PQEntry *entry = getObj(map->db, addr);

	return entry->value;
}

uint64_t allocateTimestamp(DbMap *map, enum ReaderWriterEnum e) {
	DataBase *db = database(map->db);
	uint64_t ts;

	ts = *db->pq->pqTime;

	if (!ts)
		ts = atomicAdd64(db->pq->pqTime, 1);

	switch (e) {
	case en_minimum:
		ts = db->pq->globalMin;
		break;

	case en_reader:
		while (!isReader(ts))
			ts = atomicAdd64(db->pq->pqTime, 1);
		break;
	case en_writer:
		while (!isWriter(ts))
			ts = atomicAdd64(db->pq->pqTime, 1);
		break;

	default: break;
	}

	return ts;
}

uint64_t addPQEntry(DbMap *map, uint32_t set, enum ReaderWriterEnum e) {
	DataBase *db = database(map->db);
	PQEntry *entry;
	DbAddr addr;

	lockLatch((char *)db->pq->entryLists[set].mutex);

	if ((addr.bits = allocObj(map->db, db->freePQ, 0, sizeof(PQEntry), true) ))
		entry = getObj(map->db, addr);
	else
		return 0;

	entry->value = allocateTimestamp(map->db, e);
	entry->prev.bits = 0;
	entry->set = set;

	if ( (entry->next.bits = db->pq->entryLists[set].queueHead.bits) ) {
		PQEntry* next = getObj(map->db, db->pq->entryLists[set].queueHead);
		next->prev.bits = addr.bits;
	} else
		db->pq->entryLists[set].minValue = entry->value;

	db->pq->entryLists[set].queueHead.bits = addr.bits;

	if (db->pq->globalMin > entry->value)
		computePQMin(db->pq);

	unlockLatch(db->pq->entryLists[set].mutex);
	return addr.bits;
}

void removePQEntry(DbMap *map, DbAddr addr) {
	DataBase *db = database(map->db);
	PQEntry *prev, *next, *entry;

	entry = getObj(map->db, addr);
	lockLatch(db->pq->entryLists[entry->set].mutex);

	prev = getObj(map->db, entry->prev);
	next = getObj(map->db, entry->next);

	if (entry->next.bits)
		next->prev.bits = entry->prev.bits;
	else if (entry->prev.bits)
		db->pq->entryLists[entry->set].minValue = prev->value;
	else
		db->pq->entryLists[entry->set].minValue = 0;

	if (entry->prev.bits)
		prev->next.bits = entry->next.bits;
	else
		db->pq->entryLists[entry->set].queueHead.bits = entry->next.bits;

	addSlotToFrame(map->db, &db->freePQ[entry->set], addr.bits);
	unlockLatch(db->pq->entryLists[entry->set].mutex);
}

#pragma once

typedef struct {
	uint64_t timestamp; 	// iterator timestamp
	DbMap *docStore;		// DocStore arena
	DocId docId;			// current DocID
} Iterator;

typedef struct {
	DbAddr olderDoc;		// next older document
	DbAddr snapShot;		// document commit time
} DbDoc;

typedef struct {
	uint64_t docSize;		// overall size of documents
	uint64_t docCount;		// overall number of documents
	uint64_t idxListVer;	// docIdxList version number
	DbAddr docIdxList;		// list of collection indexes
	FreeList waitLists[MAX_set][MaxDocType];	// documents waiting reclamation
} DocStore;

#define docStoreAddr(map)((DocStore *)(map->arena + 1))

Status storeVal(DbMap *map, DbAddr docAddr, DocId *docId, uint32_t set);
void *allocateDoc(DbMap *map, uint32_t size, DbAddr *addr, uint32_t set);
void *findDoc(DbMap *map, DocId docId);

void *iteratorSeek(Iterator *it, DocId docId);
void *iteratorNext(Iterator *it, DocId *docId);
void *iteratorPrev(Iterator *it, DocId *docId);

value_t createIterator(DbMap *map, bool atEnd);

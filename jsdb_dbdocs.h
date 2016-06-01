#pragma once

typedef struct {
	uint64_t refCnt[1];		// handle reference count (must be first)
	uint64_t timestamp; 	// iterator timestamp
	value_t docStore;		// DocStore handle
	DbAddr pqAddr;			// iterator snapshop
	DocId docId;			// current DocID
} Iterator;

//  document stored in a docStore

typedef struct {
	uint64_t refCnt[1];		// handle reference count (must be first)
	uint32_t docSize;		// size of document
	uint64_t docVer;		// document version sequence or zero if not in use
	uint64_t docTs;			// copy of commit timestamp
	DbAddr oldDoc;			// next older document version
	DocId txnId;			// document txn (zeroed after expiration)
	DocId docId;			// document Id
} DbDoc;

typedef struct {
	uint64_t docSize;		// overall size of documents
	uint64_t docCount;		// overall number of documents
	uint64_t idxListVer;	// docIdxList version number
	DbAddr docIdxList;		// list of collection indexes
	FreeList docWait[MAX_set][MaxDocType];	// documents waiting reclamation
} DbStore;

#define docStoreAddr(map)((DbStore *)(map->arena + 1))

uint64_t get64(uint8_t *from);
void store64(uint8_t *to, uint64_t what);
uint64_t marshal_doc(DbMap *map, value_t document);
void *allocateDoc(DbMap *map, uint32_t size, DbAddr *addr, uint32_t set);
void *findDoc(value_t docStore, DocId docId);

value_t iteratorSeek(Iterator *it, DbMap *map, DocId docId);
value_t iteratorNext(Iterator *it, DbMap *map);
value_t iteratorPrev(Iterator *it, DbMap *map);

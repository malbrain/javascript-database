#pragma once

typedef struct {
	uint64_t timestamp; 	// iterator timestamp
	DbMap *docStore;		// DocStore arena
	DbAddr pqAddr;			// snapshop handle
	DocId docId;			// current DocID
} Iterator;

//  document content follows DbDoc

typedef struct {
	uint64_t docVer;		// document version sequence or zero if not in use
	DbAddr olderDoc;		// next older document version
	DocId txnId;			// document txn (zeroed after expiration)
} DbDoc;

typedef struct {
	uint64_t docSize;		// overall size of documents
	uint64_t docCount;		// overall number of documents
	uint64_t idxListVer;	// docIdxList version number
	DbAddr docIdxList;		// list of collection indexes
	FreeList waitLists[MAX_set][MaxDocType];	// documents waiting reclamation
} DbStore;

#define docStoreAddr(map)((DbStore *)(map->arena + 1))

uint64_t get64(uint8_t *from);
void store64(uint8_t *to, uint64_t what);
uint64_t marshal_doc(DbMap *map, value_t document);
void *allocateDoc(DbMap *map, uint32_t size, DbAddr *addr, uint32_t set);
void *findDoc(DbMap *map, DocId docId);

void *iteratorSeek(Iterator *it, DocId docId);
void *iteratorNext(Iterator *it, DocId *docId);
void *iteratorPrev(Iterator *it, DocId *docId);

#pragma once

typedef struct {
	uint64_t timestamp; 	// iterator timestamp
	DbMap *docStore;		// DocStore arena
	DocId docId;			// current DocID
} Iterator;

//  document content follows DbDoc

typedef struct {
	DbAddr olderDoc;		// next older document
	DbAddr docTxn;			// document txn (zeroed after expiration)
	uint64_t txnId;			// transaction sequence ID or zero if not in use
} DbDoc;

typedef struct {
	uint64_t txnId;			// transaction sequence number
	uint64_t docSize;		// overall size of documents
	uint64_t docCount;		// overall number of documents
	uint64_t idxListVer;	// docIdxList version number
	DbAddr docIdxList;		// list of collection indexes
	FreeList waitLists[MAX_set][MaxDocType];	// documents waiting reclamation
} DbStore;

#define docStoreAddr(map)((DbStore *)(map->arena + 1))

uint64_t get64(uint8_t *from);
void store64(uint8_t *to, uint64_t what);
uint64_t marshal_doc(DbMap *map, value_t document, uint32_t set);
Status storeVal(array_t *docStore, DbAddr docAddr, DocId *docId, uint32_t set);
Status deleteDoc(array_t *docStore, DocId docId, uint32_t set);
void *allocateDoc(DbMap *map, uint32_t size, DbAddr *addr, uint32_t set);
void *findDoc(DbMap *map, DocId docId);

void *iteratorSeek(Iterator *it, DocId docId);
void *iteratorNext(Iterator *it, DocId *docId);
void *iteratorPrev(Iterator *it, DocId *docId);

value_t createDocStore(value_t name, DbMap *catalog, uint64_t size, enum HandleType type, bool onDisk);
value_t createIterator(DbMap *map, bool atEnd);

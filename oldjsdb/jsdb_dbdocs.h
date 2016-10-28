#pragma once

typedef struct {
	uint64_t refCnt[1];		// handle reference count (must be first)
	uint64_t timestamp; 	// iterator timestamp
	value_t docStore;		// DocStore handle
	DbAddr pqAddr;			// iterator snapshop
	DocId docId;			// current DocID
	DocId txnId;			// owning txn
} Iterator;

//  document stored in a docStore
//	document body follows

#define IndexMapCnt 8

typedef struct {
	struct {
		uint64_t docVer;		// key inserted at doc version
		uint32_t id;			// index ID
		uint32_t cnt;			// count of map entries
		DbAddr next;			// next map segment
	} entry[IndexMapCnt];
} IndexMap;

typedef struct {
	uint64_t docVer;		// document version sequence or zero if not in use
	uint64_t docTs;			// copy of commit timestamp
	DbAddr oldDoc[1];		// next older document version
	DbAddr indexMap;		// Key IndexID to DocVer map
	DocId txnId;			// document txn (zeroed after expiration)
	DocId docId;			// document Id
	uint32_t docSize;		// size of document
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
void *findDoc(value_t docStore, uint64_t docBits);
uint64_t findDocVer(DbMap *docStore, DocId docId, DocId txnId, uint64_t ts);

Status deleteDoc(DbMap *map, uint64_t docId, uint64_t txnBits);
extern value_t createIterator(value_t docStore, DbMap *map, bool atEnd);

value_t iteratorSeek(Iterator *it, DbMap *map, uint64_t docBits);
value_t iteratorNext(Iterator *it, DbMap *map);
value_t iteratorPrev(Iterator *it, DbMap *map);

value_t createDatabase (value_t dbname, bool onDisk);
value_t createDocStore(DbMap *map, value_t name, uint64_t size, bool onDisk, bool *created);

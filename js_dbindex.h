#pragma once

//	maximum number of nested array fields in an index key

#define MAX_array_fields 8
#define INT_key 12		// max extra bytes store64 creates

enum KeyType {
	key_undef = 0,
	key_bool,
	key_int,
	key_dbl,
	key_str,
	key_mask = 7,
	key_first = 8,
	key_reverse = 16
};

//	Key field specification
//	with nested field names
//	for compound object lookup

typedef struct {
	uint8_t numFlds;		// number of field names present
	uint8_t fldType;		// type of field

	//	field element array
	//	field name bytes follow

	struct Field {
		uint64_t hash;		// field name hash value
		uint32_t len[1];	// length of field name
		uint8_t name[1];	// field name
	} field[];
} IndexKeySpec;

//  index Key stored in docStore
//	and inserted into the index

typedef struct {
	int64_t refCnt[1];	// number of versions having this key
	uint64_t idxId;
	uint16_t keyLen;	// len of base key
	uint16_t keyIdx;	// idxHndls vector idx
	uint8_t docIdLen;	// size of the DocId extension
	uint8_t addrLen;	// size of the DbAddr extension
	uint8_t unique;		// index is unique
	uint8_t deferred;	// uniqueness deferred
	uint8_t bytes[];	// bytes of the key
} IndexKeyValue;

//  javascript-database cursor/iterator extension

typedef struct {
	uint64_t ts;
	ObjId txnId;
	DbAddr deDup[1];	// de-duplication set membership
	DbHandle hndl[1];	// docStore DbHandle
} JsMvcc;

value_t js_closeHandle(uint32_t args, environment_t *env);
void js_deleteHandle(value_t hndl);

/**
 * even =>  reader timestamp
 * odd  =>  writer timestamp
 */

typedef enum {
	en_reader,
	en_writer,
	en_current
} ReaderWriterEnum;

uint64_t allocateTimestamp(ReaderWriterEnum e);

typedef struct {
	uint16_t keyLen;	// size of key at this step
	uint16_t off;		// offset of the IndexKey
	uint16_t cnt;		// size of array
	uint16_t idx;		// next idx
	value_t *values;	// array vals
} KeyStack;

void buildKeys(Handle **idxHndls, uint16_t keyIdx, value_t val, DbAddr *keys, ObjId docId, Ver *prevVer, uint32_t idxCnt);
bool installKeys(Handle **idxHndls, Ver *ver);

Ver *findCursorVer(DbCursor *dbCursor, DbMap *map, JsMvcc *jsMvcc);
void marshalDoc(value_t document, uint8_t *doc, uint32_t offset, DbAddr addr, uint32_t docSize, value_t *val, bool fullClone);
uint64_t allocDocStore(Handle *docHndl, uint32_t size, bool zeroit);
DbAddr compileKeys(DbHandle docStore[1], value_t spec);
uint32_t calcSize (value_t doc, bool fullClone);
Ver *findDocVer(DbMap *docStore, Doc *doc, JsMvcc *jsMvcc);
extern uint64_t updateDoc(Handle **idxHndls, document_t *document, ObjId txnId);
uint64_t insertDoc(Handle **idxHndls, value_t val, uint64_t prevAddr, ObjId docId, ObjId txnId, Ver *prevVer);
extern Handle **bindDocIndexes(Handle *docHndl);

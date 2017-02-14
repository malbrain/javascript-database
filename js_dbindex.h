#pragma once

//	maximum number of nested array fields in an index key

#define MAX_array_fields 8

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
	uint8_t numFlds;			// number of field names present
	uint8_t fldType;			// type of field

	//	field element array
	//	field name bytes follow

	struct Field {
		uint64_t hash;			// field name hash value
		uint32_t len[1];		// length of field name
		uint8_t name[1];		// field name
	} field[];
} IndexKeySpec;

typedef enum {
	index_unique = 1,
	index_sparse = 2,
} IndexOptions;

//  Key string stored in docStore

typedef struct {
	volatile int64_t refCnt[1];

	uint8_t suffix;		// size of the versionId
	uint8_t prefix;		// size of the indexId

	// next is a string_t structure for the version's key object
	// where keyLen omits the docId & versionId

	uint32_t keyLen[1];
	uint8_t keyBytes[];
} IndexKeyValue;

//  javascript cursor/iterator extension

typedef struct {
	uint64_t ts;
	ObjId txnId;
	DbHandle hndl[1];
} JsMvcc;

value_t js_closeHandle(uint32_t args, environment_t *env);
void js_deleteHandle(value_t hndl);

typedef struct {
	uint32_t keyLen;	// size of key at this step
	uint32_t off;		// offset of the IndexKey
	uint32_t cnt;		// size of array
	uint32_t idx;		// next idx
	value_t *values;	// array vals
} KeyStack;

DbAddr *buildKeys(Handle *docHndl, Handle *idxHndl, object_t *oval, ObjId docId, Ver *prevVer);

void marshalDoc(value_t document, uint8_t *doc, uint32_t offset, dbaddr_t addr, uint32_t docSize, value_t *val, bool fullClone);
DbAddr compileKeys(DbHandle docStore[1], value_t spec);
uint32_t calcSize (value_t doc, bool fullClone);
Ver *findDocVer(DbMap *docStore, Doc *doc, JsMvcc *jsMvcc);
extern value_t installKeys (value_t update, Handle **idxHndls, ObjId docId, Ver *prevVer);
extern uint64_t updateDoc(Handle **idxHndls, document_t *document, value_t keys, ObjId txnId);
uint64_t insertDoc(Handle **idxHndls, value_t val, value_t keys, uint64_t prevAddr, Ver *prevVer, ObjId docId, ObjId txnId);
extern Handle **bindDocIndexes(Handle *docHndl);

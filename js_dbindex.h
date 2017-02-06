#pragma once

#include "js.h"
#include "database/db.h"
#include "database/db_api.h"
#include "database/db_arena.h"
#include "database/db_map.h"
#include "database/db_object.h"
#include "database/db_handle.h"
#include "database/db_index.h"

//	maximum number of array fields in an index key

#define MAX_array_fields 16

enum KeyType {
	key_undef = 0,
	key_bool,
	key_int,
	key_dbl,
	key_str,
	key_objId,
	key_mask = 7,
	key_first = 8,
	key_reverse = 16
};

//	Key field specification

typedef struct {
	uint8_t fldType;
	uint64_t hash;

	// below is a string_t structure

	uint32_t nameLen[1];
	char fldName[];
} IndexKeySpec;

typedef enum {
	index_unique = 1,
	index_sparse = 2,
} IndexOptions;

//  Key string stored in docStore

typedef struct {
	uint64_t refCnt[1];

	// indexLen includes the versionId
	uint32_t keyLen;

	// next is a string_t structure
	// where keyLen omits the versionId

	uint32_t baseLen[1];
	char keyBytes[];
} IndexKeyValue;

value_t js_closeHandle(uint32_t args, environment_t *env);
void js_deleteHandle(value_t hndl);

typedef struct {
	uint32_t keyLen;	// size of key at this step
	uint32_t off;		// offset of the IndexKey
	uint32_t cnt;		// size of array
	uint32_t idx;		// next idx
	value_t *values;	// array vals
} KeyStack;

DbAddr *buildKeys(Handle *docHndl, Handle *idxHndl, value_t document, ObjId docId, Ver *prevVer);
void marshalDoc(value_t document, uint8_t *doc, uint32_t offset, DbAddr addr, uint32_t docSize, value_t *val);
DbAddr compileKeys(DbMap *map, Params *params);
uint32_t calcSize (value_t doc);

#pragma once

//  maximum number of nested array fields in an index key

#define MAX_array_fields 8
#define INT_key 12  // max extra bytes store64 creates

typedef union {
  char charVal;
  bool boolVal;
  short shortVal;
  int intVal;
  int64_t longVal;
  float floatVal;
  double dblVal;
  char strVal[1];
} KeyVal;

typedef enum {
	key_undef = 0,
	key_bool,
	key_int,
	key_dbl,
	key_str,
	key_mask = 7,
	key_first = 8,
	key_reverse = 16
} KeyType;

//	one Key field specification
//	with nested field names
//	for compound object lookup

typedef struct {
	uint8_t numFlds;		// number of compound field names present
	uint8_t fldType;		// type of field

	//	field element array
	//	field name bytes follow

	struct Field {
		uint64_t hash;		// field name hash value
		uint32_t len[1];	// length of field name
		uint8_t name[1];	// field name
	} field;
} KeySpec;

typedef struct {
	uint16_t keyLen;	// size of key at this step
	uint16_t off;		// offset of the IndexKey
	uint16_t cnt;		// size of array
	uint16_t idx;		// next idx
	value_t *values;	// array vals
} KeyStack;

value_t js_closeHandle(uint32_t args, environment_t *env);
void js_deleteHandle(value_t hndl);

DbAddr compileKey(Handle *docHndl, value_t spec);

DbStatus addKeyField(DbHandle* idxHndl, KeySpec* spec,
                     struct Field* field);
uint16_t appendKeyField(Handle* idxHndls, KeySpec* spec,
                        struct Field* field, uint8_t* keyDest, uint16_t keyRoom,
                        void* cbEnv);
DbStatus installKey(KeyValue * keyValue, ObjId * docId, Handle * docHndl, Handle * idxHndl);
DbStatus idxBldKeyHelper(value_t baseArg, ObjId *docId, Handle *docHndl, Handle *idxHndl, value_t *keys);

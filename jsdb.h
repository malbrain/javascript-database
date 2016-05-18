#pragma once

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#ifdef _WIN32
#define strcasecmp _strnicmp
#endif

//
// typedefs from structures
//

typedef union ParseNode Node;
typedef struct Object object_t;
typedef struct Value value_t;
typedef struct ValueFrame frame_t;
typedef struct Document document_t;
typedef struct DocArray docarray_t;
typedef struct ValueFrame *valueframe_t;
typedef struct Closure closure_t;
typedef struct Symbol symbol_t;
typedef struct Array array_t;

//
//	reference counting
//

void *jsdb_realloc(void *old, uint32_t size, bool zero);
uint64_t jsdb_rawalloc(uint32_t amt, bool zeroit);
void *jsdb_rawaddr(uint64_t rawAddr);
void jsdb_rawfree(uint64_t rawAddr);
void *jsdb_alloc(uint32_t amt, bool zero);
uint32_t jsdb_size (void *obj);
void jsdb_free (void *obj);

bool decrRefCnt (value_t val);
void incrRefCnt (value_t val);
void abandonValue(value_t val);

#include "jsdb_rwlock.h"
#include "jsdb_vector.h"
#include "jsdb_parse.h"

typedef enum {
	OK,
	ERROR_outofmemory,
	ERROR_handleclosed,
	ERROR_badhandle,
	ERROR_badrecid,
	ERROR_endoffile,
	ERROR_notbasever,
	ERROR_recorddeleted,
	ERROR_recordnotvisible,
	ERROR_notcurrentversion,
	ERROR_cursornotpositioned,
	ERROR_invaliddeleterecord,
	ERROR_cursorbasekeyerror,
	ERROR_writeconflict,
	ERROR_duplicatekey,
	ERROR_keynotfound,
	ERROR_badtxnstep,
	ERROR_rollbackidxkey,
	ERROR_arena_already_closed,
	ERROR_script_internal,
	ERROR_script_unrecognized_function,
	ERROR_tcperror,
	ERROR_bsonformat,
	ERROR_notobject_or_array,
	ERROR_mathdomain,
	ERROR_keytoolong,
	ERROR_doesnot_exist,
	ERROR_btreepagelatchnotfound,
	ERROR_btreestruct,
	BTREE_needssplit,
} Status;

typedef union {
	struct {
		uint32_t index;		// record ID in the segment
		uint16_t segment;	// arena segment number
		uint16_t filler;
	};
	uint64_t bits;
	struct {
		uint64_t addr:48;
		uint64_t fill:16;
	};
} DocId;

//	built-in property functions

typedef struct Value (*propFcn)(struct Value *args, struct Value thisVal);
typedef struct Value (*propVal)(struct Value val);

//
// Values
//

typedef enum {
	vt_undef = 0,
	vt_bool,
	vt_int,
	vt_dbl,
	vt_infinite,
	vt_number,
	vt_string,
	vt_nan,
	vt_null,
	vt_file,
	vt_date,
	vt_status,
	vt_control,
	vt_document,
	vt_docarray,
	vt_handle,
	vt_closure,
	vt_endlist,
	vt_docId,
	vt_lval,
	vt_ref,
	vt_centi,
	vt_array,
	vt_object,
	vt_binary,
	vt_function,
	vt_uuid,
	vt_md5,
	vt_objId,
	vt_user,
	vt_fcndef,
	vt_propfcn,
	vt_propval,
	vt_weakref,
	vt_MAX
} valuetype_t;

struct Value {
	union {
		struct {
			valuetype_t type:8;
			uint32_t subType:8;
			uint32_t aux:24;		// string len
			uint32_t lvalue:1;		// value is in an lvalue
			uint32_t readonly:1;	// value is read-only
			uint32_t refcount:1;	// value is reference counted.
			uint32_t weakcount:1;	// value is weak reference.
			uint32_t rebaseptr:1;	// value is in a document
		};
		uint64_t bits;				// set bits to vt_type to initialize
	};
	union {
		void *hndl;
		uint8_t *str;
		symbol_t *sym;
		propFcn propfcn;
		propVal propval;
		uint64_t offset;
		uint8_t *rebase;
		int64_t nval;
		double dbl;
		FILE *file;
		DocId docId;
		bool boolean;
		bool negative;
		value_t *lval;
		Status status;
		uint8_t key[8];
		fcnDeclNode *fcn;
		value_t *ref;
		int64_t date;
		char *string;
		char *slot;
		array_t *aval;
		object_t *oval;
		enum flagType ctl;
		document_t *document;
		docarray_t *docarray;
		closure_t *closure;
		struct RawObj *raw;
	};
};

//
// Symbols
//

struct Symbol {
	uint32_t depth;			// frame depth
	uint32_t frameidx;		// var value
	uint32_t nameLen;
	char *symbolName;		// symbol name
};

typedef struct SymTab {
	struct SymTab *parent;
	symbol_t *entries;
	uint32_t depth;
} symtab_t;

//
// Interpreter environment
//

typedef struct {
	valueframe_t topFrame;
	closure_t *closure;
	Node *table;
} environment_t;

value_t fcnCall (value_t fcnClosure, value_t args, value_t thisVal);
value_t newClosure(fcnDeclNode *fn, environment_t *env);

//
//  Strings
//
value_t newString(
	void *value,
	uint32_t len);

//
// Object/Document key/value pairs
//

typedef struct {
	value_t name;
	value_t value;
} pair_t;

//
// Documents in Storage
//

struct Document {
	uint32_t base;		// offset in record
	uint32_t count;		// size of kv array
	uint32_t capacity;  // size of the hash table
	pair_t pairs[0];	// hash table follows name/value array
};

struct DocArray {
	uint32_t base;		// offset in record
	uint32_t count;		// number of array elements
	value_t values[0];	// the values in the array
};

//
// Objects
//

uint64_t hashStr(value_t name);

struct Object {
	uint32_t capacity;
	object_t *proto;
	RWLock lock[1];
	pair_t *pairs;
	void *hashTbl;		// hash table of 8, 16 or 32 bit entries
	value_t base;
};

value_t newObject();

value_t *lookup(object_t *obj, value_t name, bool addBit);
value_t *deleteField(object_t *obj, value_t name);
value_t lookupDoc(document_t *doc, value_t name);
value_t getDocArray(docarray_t *doc, uint32_t idx);
value_t getDocValue(document_t *doc, uint32_t idx);
value_t getDocName(document_t *doc, uint32_t idx);

//
// Closures
//

struct Closure {
	int count;
	Node *table;
	fcnDeclNode *fcn;
	object_t proto[1];
	object_t props[1];
	valueframe_t frames[0];
};

//
// Arrays
//

extern int ArraySize[];

struct Array {
	union {
		value_t *values;
		char *array;
	};
	RWLock lock[1];
	object_t obj[1];
};
	
enum ArrayType {
	array_value = 0,
	array_int8,
	array_uint8,
	array_int16,
	array_uint16,
	array_int32,
	array_uint32,
	array_float32,
	array_float64
};

value_t newArray(enum ArrayType subType);

//  function call/local frames

struct ValueFrame {
	uint32_t count;
	value_t thisVal;
	value_t nextThis;
	value_t arguments;
	value_t values[1]; // first slot is return value
};

void incrFrameCnt (frame_t *frame);
void abandonFrame(frame_t *frame);

//
// Interpreter dispatch
//

typedef value_t (*dispatchFcn)(Node *hdr, environment_t *env);

extern dispatchFcn dispatchTable[node_MAX];
extern value_t builtinObj[vt_MAX];

value_t eval_arg(uint32_t *args, environment_t *env);
value_t replaceValue(value_t lval, value_t value);
void storeArrayValue(value_t left, value_t right);
char *strtype(valuetype_t);
void printValue(value_t, uint32_t depth);

#define dispatch(slot, env) ((dispatchTable[env->table[slot].type])(&env->table[slot], env))

//
// Status
//

char *strstatus(Status);
void installStatus(char *, Status, symtab_t *);

//
// Post-parse pass
//

void compileSymbols(fcnDeclNode *fcn, Node *table, symtab_t *symtab, uint32_t depth);

//
// install function closures
//
void installFcns(uint32_t decl, environment_t *env);

//
// value conversions
//

int value2Str(value_t v, value_t **array, int depth);
value_t valueCat(value_t left, value_t right);

value_t convArray2Value(void *lval, enum ArrayType type);
value_t conv2ObjId(value_t, bool);
value_t conv2Bool(value_t, bool);
value_t conv2Int(value_t, bool);
value_t conv2Dbl(value_t, bool);
value_t conv2Str(value_t, bool);

//
// Errors
//
value_t makeError(Node *node, environment_t *env, char *msg);
void errorText(Status s);

#pragma once

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <inttypes.h>

#ifdef _WIN32
#define strcasecmp _strnicmp
#endif

extern bool MathNums;
extern bool debug;
extern bool parseDebug;

//	database object reference

typedef union {
  struct {
	uint64_t addr:48;
	uint64_t storeId:16;
  };
  uint64_t bits;
} dbaddr_t;

//  Strings

typedef struct {
	uint32_t len;
	uint8_t val[];
} string_t;

//  memory allocation

void *js_realloc(void *old, uint32_t *size, bool zeroit);
void *js_alloc(uint32_t len, bool zeroit);
uint32_t js_size (void *obj);
void js_free(void *obj);

// typedefs from structures

typedef struct Value value_t;
typedef struct Closure closure_t;
typedef struct SymTable symtab_t;
typedef struct FcnDeclNode fcnDeclNode;

//	reference counting

bool abandonValueIfDiff(value_t val, value_t test);
void incrRefCnt (value_t val);
void abandonValue(value_t val);
void deleteValue(value_t val);
void abandonSlot(value_t *slot);
void deleteSlot(value_t *slot);
bool decrRefCnt (value_t val);

typedef enum {
	OK,
	ERROR_outofmemory,
	ERROR_script_internal,
	ERROR_script_unrecognized_function,
	ERROR_tcperror,
	ERROR_bsonformat,
	ERROR_notobject_or_array,
	ERROR_mathdomain,
	ERROR_endoffile,
	ERROR_doesnot_exist,
	ERROR_script_parse,
	ERROR_json_parse,
	ERROR_not_docid,
	ERROR_not_found,
} Status;

// Symbols

typedef struct {
	uint32_t frameIdx;		// var value
	uint16_t depth;			// frame depth
	uint8_t scoped;			// symbol is scoped
} symbol_t;

typedef enum {
	ctl_none,
	ctl_return,
	ctl_continue,
	ctl_break,
	ctl_error,
	ctl_throw,
	ctl_delete,
} ctlType;

typedef enum {
	aux_none,
	aux_newobj,
	aux_endstmt,
} auxType;

enum flagType {
	flag_decl		= 1,	// node is a symbol declaration
	flag_lval		= 2,	// node produces lval
	flag_frame		= 4,	// for node creates new frame
	flag_scope		= 4,	// variable declared in block scope
};

#ifdef apple
#define Status int
#endif

value_t newString(void *value, int len); 

// Values

typedef enum {
	vt_undef = 0,
	vt_bool,
	vt_int,
	vt_dbl,
	vt_date,
	vt_infinite,
	vt_number,
	vt_string,
	vt_nan,
	vt_null,
	vt_file,
	vt_status,
	vt_control,
	vt_closure,
	vt_endlist,
	vt_document,
	vt_docId,
	vt_txnId,		// 64 bit immediate
	vt_lval,
	vt_centi,
	vt_array,
	vt_object,
	vt_binary,
	vt_function,
	vt_uuid,		// 16 byte string
	vt_md5,
	vt_propfcn,
	vt_propval,
	vt_weakref,
	vt_db,
	vt_store,
	vt_index,
	vt_cursor,
	vt_iter,
	vt_txn,
	vt_key,
	vt_MAX
} valuetype_t;

struct Value {
	union {
		struct {
			valuetype_t type:8;
			uint32_t offset:24;		// offset from document base
			uint32_t subType:8;
			uint32_t refcount:1;	// value is reference counted.
			uint32_t weakcount:1;	// value is weak reference.
			uint32_t marshaled:1;	// value is marshaled in a document
			uint32_t objvalue:1;	// object value occurs at ptr
			uint32_t readonly:1;	// value is read-only
			uint32_t ishandle:1;
			uint32_t lvalue:1;		// value is in an lvalue
			uint32_t filler:17;		// available bits
		};
		uint64_t bits;				// set bits to valueType to initialize
	};
	union {
		symbol_t sym[1];
		string_t *str;
		int64_t nval;
		double dbl;
		void *addr;
		FILE *file;
		ctlType ctl;
		value_t *lval;
		Status status;
		uint8_t key[8];
		uint64_t boolean;
		uint64_t negative;
		uint64_t txnBits;
		uint64_t docBits;
		uint64_t keyBits;
		int64_t date;
		uint64_t *hndl;
		dbaddr_t arenaAddr;
		struct FcnDeclNode *fcn;
		closure_t *closure;
		struct RawObj *raw;
	};
};

//  convert dbaddr_t to void *

void *js_addr(value_t val);

//	Document version retrieved from a docStore

typedef struct {
	value_t update[1];	// document update object
	uint64_t hndl[1];	// docStore DbHandle
	struct Ver_ *ver;	// pointer to doc version
} document_t;
	
// Objects

typedef struct {
	value_t name;
	value_t value;
} pair_t;

typedef struct {
	value_t protoChain;		// the prototype chain
	value_t base[1];		// primitive value
	uint8_t protoBase;		// base prototype type
	uint8_t marshaled;		// object is marshaled
	union {
	  pair_t *pairsPtr;		// key/value pairs followed by
							// hash table of 8, 16, or 32 bit indicies
	  struct {
		uint32_t cap;		// maximum number of key/value pairs
		uint32_t cnt;		// number of pair entries in use
	  };
	};
	pair_t pairArray[];		// if marshaled, pairs & hash table follow
} object_t;

// Symbol tables

struct SymTable {
	uint32_t depth;
	uint32_t scopeCnt;
	uint32_t frameIdx;
	uint32_t childFcns;
	symtab_t *parent;
	object_t entries;
};

#include "js_parse.h"
#include "js_vector.h"

//  function call/local frames of variables

typedef struct {
	uint32_t count;
	value_t thisVal;
	value_t nextThis;
	value_t arguments;
	symtab_t *symbols;	// frame symbols
	value_t values[1];	// first slot is return value
} frame_t;

//	block scope variables sub-frame

typedef struct {
	uint32_t count;		// current block scope variables
	frame_t *frame;		// frame (regular) variables
	symtab_t *symbols;	// current block scope symbols
	value_t values[1];	// current block scope slots
} scope_t;

// Closures

struct Closure {
	value_t obj;		// Function object
	Node *table;
	fcnDeclNode *fd;
	value_t protoObj;	// the prototype property
	symtab_t *symbols;	// block scope symbols
	uint32_t depth;		// depth of the scopes
	scope_t *scope[];	// lexical variable scopes
};

value_t newObject(valuetype_t protoBase);

// Interpreter environment

typedef struct {
	scope_t *scope;		// current block scope variables
	firstNode *first;	// first node of current script
	frame_t *topFrame;	// top level varable frame
	value_t *literals;
	closure_t *closure;
	Node *table;
} environment_t;

//	new literal handling

void abandonLiterals(environment_t *env);

//	lookup fields in objects

value_t lookupAttribute(value_t obj, value_t field, bool lVal, value_t *original);
value_t *lookup(object_t *obj, value_t name, bool addBit, uint64_t hash);
void hashStore(void *table, uint32_t hashEnt, uint32_t idx, uint32_t val);
uint32_t hashEntry(void *table, uint32_t hashEnt, uint32_t idx);
value_t *deleteField(object_t *obj, value_t name);
uint64_t hashStr(uint8_t *str, uint32_t len);

value_t *lookup(object_t *obj, value_t name, bool addBit, uint64_t hash);
void hashStore(void *table, uint32_t hashEnt, uint32_t idx, uint32_t val);
uint32_t hashEntry(void *table, uint32_t hashEnt, uint32_t idx);
value_t *deleteField(object_t *obj, value_t name);

value_t fcnCall (value_t fcnClosure, value_t args, value_t thisVal, bool rtnVal);
value_t newClosure( fcnDeclNode *fcn, environment_t *env);

// Built-in property and fcns

value_t callFcnFcn(value_t fcn, value_t *args, environment_t *env);
value_t callFcnProp(value_t prop, value_t arg, bool lVal);

value_t callObjFcn(value_t *obj, string_t *name, bool abandon);
value_t getPropFcnName(value_t slot);

// Arrays

extern int ArraySize[];

typedef struct {
	value_t obj;		// Array object
	union {
		value_t *valuePtr;
		uint8_t *array;
		struct {
			uint32_t cnt;
			uint32_t max;
		};
	};
	uint8_t marshaled;	// Array is marshaled
	value_t valueArray[0];
} array_t;
	
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

void incrScopeCnt (scope_t *scope);
void abandonScope(scope_t *scope);

// Interpreter dispatch

typedef value_t (*dispatchFcn)(Node *hdr, environment_t *env);

extern dispatchFcn dispatchTable[node_MAX];
extern value_t builtinProto[vt_MAX];

value_t eval_arg(uint32_t *args, environment_t *env);
value_t replaceValue(value_t lval, value_t value);
void storeArrayValue(value_t left, value_t right);
void replaceSlot(value_t *slot, value_t value);
void printValue(value_t, uint32_t depth);
char *strtype(valuetype_t);

#define dispatch(slot, env) ((dispatchTable[env->table[slot].type])(&env->table[slot], env))

// Status

char *strstatus(Status);
void installStatus(uint8_t *, Status, symtab_t *);

// Post-parse pass

void compileScripts(uint32_t max, Node *table, symtab_t *symbols, symtab_t *block);

// install function closures

void installFcns(uint32_t decl, environment_t *env);

//	execute script modules

void execScripts(Node *table, uint32_t size, value_t args, symtab_t *symbols, environment_t *env);

// value conversions

void valueCat(value_t *left, value_t right, bool abandon);
void valueCatStr (value_t *left, uint8_t *rightval, uint32_t rightlen);

value_t value2Str(value_t v, bool json, bool raw);

value_t convArray2Value(void *lval, enum ArrayType type);
value_t conv2Str(value_t, bool, bool);
value_t conv2ObjId(value_t, bool);
value_t conv2Bool(value_t, bool);
value_t conv2Int(value_t, bool);
value_t conv2Dbl(value_t, bool);

value_t convDocument(value_t val, bool lVal);

// Errors

value_t makeError(Node *node, environment_t *env, char *msg);
void errorText(Status s);

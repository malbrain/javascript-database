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

extern bool parseDebug;
extern bool hoistDebug;
extern bool evalDebug;
extern bool mathNums;
extern bool debug;

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
typedef struct Document document_t;
typedef struct DbObject dbobject_t;
typedef struct Object object_t;
typedef struct DbArray dbarray_t;
typedef struct Array array_t;

//	reference counting

bool abandonValueIfDiff(value_t val, value_t test);
void incrRefCnt (value_t val);
void abandonValue(value_t val);
void deleteValue(value_t val);
void abandonSlot(value_t *slot);
void deleteSlot(value_t *slot);
bool decrRefCnt (value_t val);

#include "js_error.h"

//	Symbols
//	must fit within pointer in value_t

typedef struct {
	uint32_t frameIdx:32;	// var frame idx
	uint32_t depth:16;		// frame depth
	uint32_t scoped:1;		// symbol is scoped
	uint32_t fixed:1;		// scope fix-up done
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
	flag_scope		= 4,	// variable declared in block scope
	flag_operand	= 8		// node produces operand
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
	vt_symbol,
	vt_uuid,		// 16 byte string
	vt_md5,
	vt_propfcn,
	vt_propval,
	vt_weakref,
	vt_catalog,
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
		uint64_t addrBits;
		uint64_t idBits;
		uint64_t *hndl;
		int64_t date;
		object_t *oval;
		array_t *aval;
		closure_t *closure;
		struct RawObj *raw;
		struct FcnDeclNode *fcn;
	};
};

//  convert dbaddr_t to void *

#define js_addr(val) ((val).marshaled ? js_dbaddr((val),(val).addr) : (val).addr)
void *js_dbaddr(value_t val, void *addr);

#pragma pack(push, 4)

typedef struct {
	value_t name;
	value_t value;
} pair_t;

// database marshaled Objects

struct DbObject {
	uint32_t cnt;		// number of pair entries
	pair_t pairs[];		// pairs & hash table follow
};
	
//	Objects

struct Object {
	value_t protoChain;		// the prototype chain
	value_t baseVal[1];		// primitive value
	pair_t *pairsPtr;		// key/value pairs followed by hash table
	uint8_t protoBase;		// base prototype type
};

value_t *setAttribute(object_t *oval, value_t name, uint32_t h);
value_t cloneObject(value_t obj, void *addr);
value_t newObject(valuetype_t protoBase);
value_t *baseObject(value_t obj);


// Symbol tables

struct SymTable {
	uint32_t depth;
	uint32_t baseIdx;
	uint32_t frameIdx;
	uint32_t scopeCnt;
	uint32_t childFcns;
	symtab_t *parent;
	object_t entries;
};

// Arrays

extern int ArraySize[];

struct DbArray {
	uint32_t cnt;
	value_t valueArray[0];
};

struct Array {
	value_t obj;		// Array object -- must be first as objvalue
	union {
		value_t *valuePtr;
		uint8_t *array;
	};
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

value_t newArray(enum ArrayType subType, uint32_t initSize);
value_t cloneArray(value_t value, void *addr);
#pragma pack(pop)

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
	value_t obj;		// Function object -- must be first as objvalue
	Node *table;
	fcnDeclNode *fd;
	value_t protoObj;	// the prototype property
	symtab_t *symbols;	// block scope symbols
	uint32_t depth;		// depth of the scopes
	scope_t *scope[];	// lexical variable scopes
};

// Interpreter environment

typedef struct {
	scope_t *scope;			// current block scope variables
	firstNode *first;		// first node of current script
	frame_t *topFrame;		// top level varable frame
	value_t *literals;		// vector of evaluation literals
	closure_t *closure;		// current function closure
	uint64_t txnBits[1];	// current nested transaction
	void *timestamp;		// thread timestamp generator
	Node *table;			// current function node table
} environment_t;

void *newTsGen (void);

//	new literal handling

void abandonLiterals(environment_t *env);

//	lookup fields in objects

value_t lookupAttribute(value_t obj, value_t field, bool lVal, bool eval);
int lookupValue(value_t obj, value_t name, uint64_t hash, bool find);

void hashStore(void *table, uint32_t hashEnt, uint32_t idx, uint32_t val);
uint32_t hashEntry(void *table, uint32_t hashEnt, uint32_t idx);
value_t *deleteField(object_t *obj, value_t name);
uint64_t hashStr(uint8_t *str, uint32_t len);

value_t lookup(value_t obj, value_t name, bool addBit, uint64_t hash);
void hashStore(void *table, uint32_t hashEnt, uint32_t idx, uint32_t val);
uint32_t hashEntry(void *table, uint32_t hashEnt, uint32_t idx);
value_t *deleteField(object_t *obj, value_t name);

value_t fcnCall (value_t fcnClosure, value_t args, value_t thisVal, bool rtnVal, environment_t *env);
value_t newClosure( fcnDeclNode *fcn, environment_t *env);

// Built-in property and fcns

value_t callFcnFcn(value_t fcn, value_t *args, environment_t *env);
value_t callFcnProp(value_t prop, value_t arg, value_t baseVal, bool lval);

value_t callObjFcn(value_t obj, string_t *name, bool abandon, environment_t *env);
value_t getPropFcnName(value_t slot);

void incrScopeCnt (scope_t *scope);
void abandonScope(scope_t *scope);

// Interpreter dispatch

typedef value_t (*dispatchFcn)(Node *hdr, environment_t *env);

extern dispatchFcn dispatchTable[node_MAX];
extern value_t builtinProto[vt_MAX];

value_t eval_arg(uint32_t *args, environment_t *env);
value_t replaceValue(value_t lval, value_t value);
value_t cloneValue(value_t value, void *addr);

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

value_t includeDocument(value_t val, void *dest, environment_t *env);
value_t convDocument(value_t val, bool lVal);

// Errors

value_t makeError(Node *node, environment_t *env, char *msg);
void errorText(Status s);

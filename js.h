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

//  memory allocation

void *js_realloc(void *old, uint32_t size, bool zeroit);
void *js_alloc(uint32_t len, bool zeroit);
uint32_t js_size (void *obj);
void js_free(void *obj);

//
// typedefs from structures
//

typedef union ParseNode Node;
typedef struct Value value_t;
typedef struct ValueFrame frame_t;
typedef struct Document document_t;
typedef struct DocArray docarray_t;
typedef struct ValueFrame *valueframe_t;
typedef struct FcnDeclNode fcnDeclNode;
typedef struct Closure closure_t;
typedef struct Object object_t;
typedef struct Array array_t;
typedef struct ObjPair pair_t;

//
//	reference counting
//

bool abandonValueIfDiff(value_t val, value_t test);
void incrRefCnt (value_t val);
void abandonValue(value_t val);
void deleteValue(value_t val);
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
} Status;

//
// Symbols
//

typedef struct {
	uint32_t depth;			// frame depth
	uint32_t frameIdx;		// var value
} symbol_t;

enum flagType {
	flag_return		= 0,
	flag_continue	= 1,
	flag_break		= 2,
	flag_error		= 3,
	flag_throw		= 4,
	flag_delete		= 5,
	flag_newobj		= 6,
	flag_typemask	= 7,
	flag_decl		= 8,	// node is a symbol declaration
	flag_lval		= 16,	// node produces lval
};

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
	vt_txnId,
	vt_lval,
	vt_centi,
	vt_array,
	vt_object,
	vt_binary,
	vt_function,
	vt_uuid,
	vt_md5,
	vt_objId,
	vt_user,
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
			uint32_t objvalue:1;	// object value occurs at ptr
		};
		uint64_t bits;				// set bits to valueType to initialize
	};
	union {
		char *string;
		char *slot;
		char *str;
		symbol_t sym[1];
		uint64_t offset;
		uint8_t *rebase;
		int64_t nval;
		double dbl;
		FILE *file;
		bool boolean;
		bool negative;
		value_t *lval;
		Status status;
		uint8_t key[8];
		uint64_t txnBits;
		uint64_t docBits;
		value_t *ref;
		int64_t date;
		array_t *aval;
		object_t *oval;
		enum flagType ctl;
		uint64_t handle[1];
		struct FcnDeclNode *fcn;
		document_t *document;
		docarray_t *docarray;
		closure_t *closure;
		struct RawObj *raw;
	};
};

//
// Objects
//

uint64_t hashStr(value_t name);

struct Object {
	value_t protoChain;		// the prototype chain
	pair_t *pairs;
	void *hashTbl;			// hash table of 8, 16 or 32 bit entries
	value_t base[1];			// primitive value
	uint32_t capacity;
	uint8_t protoBase;		// base prototype type
};

value_t newObject(valuetype_t protoBase);

value_t *lookup(object_t *obj, value_t name, bool addBit, bool noProps);
value_t *deleteField(object_t *obj, value_t name);
value_t lookupDoc(document_t *doc, value_t name);
value_t getDocArray(docarray_t *doc, uint32_t idx);
value_t getDocValue(document_t *doc, uint32_t idx);
value_t getDocName(document_t *doc, uint32_t idx);

// Symbol tables

typedef struct {
	object_t entries[1];
	void *parent;
	uint32_t depth;
	uint32_t frameIdx;
	uint32_t childFcns;
} symtab_t;

#include "js_parse.h"
#include "js_vector.h"

//
// Interpreter environment
//

typedef struct {
	valueframe_t topFrame;
	closure_t *closure;
	firstNode *first;
	Node *table;
	bool lVal;		// return l-values if possible
} environment_t;

value_t fcnCall (value_t fcnClosure, value_t args, value_t thisVal);
value_t newClosure( fcnDeclNode *fcn, environment_t *env);

// Built-in property and fcns

value_t callFcnFcn(value_t fcn, value_t *args, environment_t *env);
value_t callFcnProp(value_t prop, value_t arg, bool lVal);
value_t callObjFcn(value_t *obj, char *name, bool abandon);
value_t getPropFcnName(value_t slot);

//
//  Strings
//

value_t newString(
	void *value,
	uint32_t len);

//
// Object/Document key/value pairs
//

struct ObjPair {
	value_t name;
	value_t value;
};

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
// Closures
//

struct Closure {
	value_t obj;		// Function object
	value_t protoObj;	// the prototype property
	symtab_t *symbols;
	fcnDeclNode *fd;
	Node *table;
	int count;
	valueframe_t frames[0];
};

//
// Arrays
//

extern int ArraySize[];

struct Array {
	value_t obj;		// Array object
	union {
		value_t *values;
		char *array;
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
extern value_t builtinProto[vt_MAX];

value_t eval_arg(uint32_t *args, environment_t *env);
value_t replaceValue(value_t lval, value_t value);
void storeArrayValue(value_t left, value_t right);
void replaceSlot(value_t *slot, value_t value);
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

void compileScripts(uint32_t max, Node *table, symtab_t *symbols);

//
// install function closures
//
void installFcns(uint32_t decl, environment_t *env);

//
//	execute script modules
//

void execScripts(Node *table, uint32_t size, value_t args, symtab_t *symbols, environment_t *env);

//
// value conversions
//

void valueCat(value_t *left, value_t right, bool abandon);

value_t value2Str(value_t v, bool json, bool raw);

value_t convArray2Value(void *lval, enum ArrayType type);
value_t conv2Str(value_t, bool, bool);
value_t conv2ObjId(value_t, bool);
value_t conv2Bool(value_t, bool);
value_t conv2Int(value_t, bool);
value_t conv2Dbl(value_t, bool);

//
// Errors
//
value_t makeError(Node *node, environment_t *env, char *msg);
void errorText(Status s);

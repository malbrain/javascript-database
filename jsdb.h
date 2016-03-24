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
// Reference Counting
//

typedef struct {
	uint64_t weakCnt[1];
	uint64_t refCnt[1];
} rawobj_t;

#include "jsdb_parse.h"
#include "jsdb_vector.h"

//
// Symbols
//

typedef struct {
	uint32_t level;			// determines frame level
	uint32_t frameidx;		// determines var value
	stringNode *name;		// symbol name
} symbol_t;

typedef struct {
	void *parent;
	symbol_t *entries;
} symtab_t;

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

//
// Values
//

typedef enum {
	vt_null,
	vt_endlist,
	vt_docId,
	vt_string,
	vt_dbl,
	vt_int,
	vt_bool,
	vt_file,
	vt_status,
	vt_control,
	vt_document,
	vt_docarray,
	vt_handle,
	vt_closure,
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
	vt_weakref,
} valuetype_t;

typedef struct Value {
	union {
		struct {
			valuetype_t type;
			uint32_t aux:24;		// string len
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
		void *propfcn;
		uint64_t offset;
		uint8_t *rebase;
		int64_t nval;
		double dbl;
		rawobj_t *raw;
		FILE *file;
		DocId docId;
		bool boolean;
		flagType ctl;
		Status status;
		uint8_t key[8];
		fcnDeclNode *fcn;
		struct Value *ref;
		struct Value *lval;
		struct Array *aval;
		struct Object *oval;
		struct Document *document;
		struct DocArray *docarray;
		struct Closure *closure;
	};
} value_t;

//  function call/local frames

typedef struct {
	value_t rtnVal;
	uint32_t name;
	uint32_t count;
	value_t values[0];
} frame_t;

typedef frame_t *valueframe_t;

void incrFrameCnt (frame_t *frame);
void abandonFrame(frame_t *frame);

//	reference counting

void *jsdb_alloc(uint32_t amt, bool zero);
void jsdb_free (void *obj);

bool decrRefCnt (value_t val);
void incrRefCnt (value_t val);
void abandonValue(value_t val);

//
// Interpreter environment
//

typedef struct {
	valueframe_t *framev;
	value_t propBase;
	Node *table;
} environment_t;

void printNode(uint32_t slot, Node *table);

//
// Interpreter dispatch
//

typedef value_t (*dispatchFcn)(Node *hdr, environment_t *env);

extern dispatchFcn dispatchTable[node_MAX];
value_t eval_arg(uint32_t *args, environment_t *env);
value_t replaceSlotValue(value_t *slot, value_t value);
char *strtype(valuetype_t);
void printValue(value_t, uint32_t depth);

#define dispatch(slot, env) ((dispatchTable[env->table[slot].type])(&env->table[slot], env))

//	built-in property functions

typedef value_t (*propFcnEval)(uint32_t args, environment_t *env);

//
//  Strings
//
value_t newString(
	uint8_t *value,
	uint32_t len);

//
// Closures
//

typedef struct Closure {
	int count;
	Node *table;
	fcnDeclNode *fcn;
	valueframe_t frames[0];
} closure_t;

value_t newClosure(fcnDeclNode *fn, uint32_t level, Node *table, valueframe_t *oldscope);

//
// Arrays
//

typedef struct Array {
	value_t *array;
} array_t;
	
value_t newArray();

//
// Documents in Storage
//

typedef struct Document {
	uint32_t base;		// offset in record
	uint32_t count;		// size of kv array
	uint32_t capacity;  // size of the hash table
	value_t names[0];	// hash table follows names and values arrays
} document_t;

typedef struct DocArray {
	uint32_t base;		// offset in record
	uint32_t count;		// number of array elements
	value_t array[0];	// the values in the array
} docarray_t;

//
// Objects
//

value_t builtinProp(value_t obj, value_t prop, environment_t *env);
uint64_t hashStr(value_t name);

typedef struct Object {
	uint32_t capacity;
	uint32_t *hash;
	value_t *values;
	value_t *names;
} object_t;

value_t newObject();
void deleteArray(value_t obj);
void deleteObject(value_t obj);
void deleteSlotValue(value_t slot);

value_t *lookup(value_t obj, value_t name, bool addBit);
value_t *deleteField(value_t obj, value_t name);
value_t lookupDoc(document_t *doc, value_t name);
value_t indexDoc(document_t *doc, uint32_t idx);

//
// Status
//

char *strstatus(Status);
void installStatus(char *, Status, symtab_t *);

//
// Post-parse passes
//
void compile(fcnDeclNode *fcn, Node *table, symtab_t *symtab, uint32_t level);
void assign(uint32_t slot, Node *table, symtab_t *symtab, uint32_t level);

//
// install function closures
//
void installFcns(uint32_t decl, Node *table, valueframe_t frame);

//
// value conversions
//
value_t conv2ObjId(value_t);
value_t conv2Bool(value_t);
value_t conv2Int(value_t);
value_t conv2Dbl(value_t);
value_t conv2Str(value_t);

//
// Errors
//
value_t makeError(Node *node, environment_t *env, char *msg);
void errorText(Status s);

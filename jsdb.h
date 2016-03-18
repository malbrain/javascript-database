#pragma once

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

//
// Reference Counting
//

typedef struct {
	uint64_t weakCnt[1];
	uint64_t refCnt[1];
} rawobj_t;

void *jsdb_alloc(uint32_t amt, bool zero);
void jsdb_free (void *obj);

bool zero_ref_cnt(struct Value val);
bool decr_ref_cnt(struct Value val);
void incr_ref_cnt(struct Value val);
bool decrRefCnt (void *obj);
void incrRefCnt (void *obj);

#include "jsdb_db.h"
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

//
// Values
//

typedef enum {
	vt_uninitialized,
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
	vt_objid,
	vt_user,
	vt_fcndef,
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
		void *h;
		DocId docId;
		uint8_t *str;
		symbol_t *sym;
		uint64_t offset;
		uint8_t *rebase;
		int64_t nval;
		double dbl;
		rawobj_t *raw;
		FILE *file;
		bool boolean;
		flagType ctl;
		Status status;
		fcnDeclNode *f;
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
	uint32_t count;
	value_t values[0];
} frame_t;

typedef frame_t *valueframe_t;

void deleteFrame(frame_t *frame);

//
// Interpreter environment
//

typedef struct {
	valueframe_t *framev;
	Node *table;
} environment_t;

void printNode(uint32_t slot, Node *table);

//
// Interpreter dispatch
//

typedef value_t (*dispatchFcn)(Node *hdr, environment_t *env);

extern dispatchFcn dispatchTable[node_MAX];
value_t eval_arg(uint32_t *args, environment_t *env);
value_t replaceSlotValue(value_t *slot, value_t *value);
char *strtype(valuetype_t);
void printValue(value_t, uint32_t depth);

#define dispatch(slot, env) ((dispatchTable[env->table[slot].type])(&env->table[slot], env))

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
	fcnDeclNode *func;
	valueframe_t *frames;
} closure_t;

value_t newClosure(fcnDeclNode *fn, uint32_t level, valueframe_t *oldscope);

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

typedef struct Object {
	uint32_t capacity;
	uint32_t *table;
	value_t *values;
	value_t *names;
} object_t;

value_t newObject();
void deleteArray(value_t obj);
void deleteObject(value_t obj);
void deleteSlotValue(value_t slot);

value_t *lookup(value_t obj, value_t name, bool addBit);
value_t *deleteField(value_t obj, value_t name);
value_t lookupDoc(value_t obj, value_t name);

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
void installFcns(uint32_t decl, Node *table, valueframe_t *framev);

//
// value conversions
//
value_t conv2Bool(value_t);
value_t conv2Int(value_t);

//
// Errors
//
value_t makeError(Node *node, environment_t *env, char *msg);
void errorText(Status s);

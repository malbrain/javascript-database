#pragma once
#define MAX_set 32		// maximum thread count
#define MAX_cursor 4096 // maximum cursor nodes
#define MAX_flds 64		// number of fields in compound index
#define MAX_key 4096	// maximum key length

enum DocType {
	FrameType,
	DocIdType,		// DocId value
	MinDocType = 3,	// minimum document size in bits
	MaxDocType = 24	// each power of two, 3 - 24
};

typedef union {
	struct {
		uint32_t offset;	// offset in the segment
		uint16_t segment;	// arena segment number
		union {
			struct {
				uint8_t mutex:1;	// mutex bit;
				uint8_t dead:1;		// mutex bit, node type
				uint8_t type:6;		// object type
			};
			volatile char latch[1];
		};
		union {
			uint8_t nbyte;		// number of bytes in a span node
			uint8_t nslot;		// number of slots of frame in use
			uint8_t nbits;		// power of two for object size
			uint8_t ttype;		// index transaction type
			int8_t rbcmp;		// red/black comparison
		};
	};
	uint64_t bits;
	struct {
		uint64_t addr:48;
		uint64_t fill:16;
	};
} DbAddr;

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

#include "jsdb_malloc.h"

typedef struct {
	DbAddr head[1];		// earliest frame waiting to be recycled
	DbAddr tail[1];		// location of latest frame to be recycle
	DbAddr free[1];		// frames of free objects
} FreeList;

typedef enum {
	Txn_add,
	Txn_upd,
	Txn_del
} TxnStepType;

typedef struct {
	uint64_t hndlId;	// docStore handle id
	DocId docId;		// document Id
} TxnStep;

typedef struct {
	DbAddr txnFrame[1];	// Frame chain of txn steps
	uint64_t ts;		// committed txn timestamp
	uint32_t set;		// set for the transaction
} Txn;

typedef enum {
	Txn_id,
	Txn_step,
	Txn_max
} TxnType;

typedef struct {
	uint64_t timestamp;	// txn timestamp
	uint64_t keyVer;	// current cursor document version
	DbAddr pqAddr;		// priority queue handle
	value_t hndl;		// cursor index handle
	DbAddr docAddr;		// current cursor document address
	DocId docId;		// current cursor document ID
	DocId txnId;		// current cursor transaction
} DbCursor;

#include "jsdb_dbarena.h"
#include "jsdb_dbdocs.h"
#include "jsdb_dbindex.h"
#include "jsdb_dbart.h"
#include "jsdb_dbbtree.h"

//	handle value types

typedef enum {
	Hndl_newarena = 0,
	Hndl_database,
	Hndl_docStore,
	Hndl_btreeIndex,
	Hndl_artIndex,
	Hndl_colIndex,
	Hndl_iterator,
	Hndl_btreeCursor,
	Hndl_artCursor,
	Hndl_docVersion
} HandleType;

typedef struct {
	rawobj_t raw[1];		// memory allocation header
	int64_t entryCnt[1];	// number of current entries
	void *object;			// pointer to handle object
} Handle;

#define FrameSlots 64

typedef struct {
	DbAddr next;			// next frame in queue
	DbAddr prev;			// prev frame in queue
	uint64_t timestamp;		// latest timestamp
	DbAddr slots[FrameSlots];// array of waiting/free slots
} Frame;

//	red-black tree descent stack

#define RB_bits		24

typedef struct {
	uint64_t lvl;			// height of the stack
	DbAddr entry[RB_bits];	// stacked tree nodes
} PathStk;

typedef struct {
	uint32_t keyLen;		// length of key
	DbAddr left, right;		// next nodes down
	DbAddr addr;			// entry addr in parent
	char red;				// is tree node red?
	char key[0];			// entry key
} RedBlack;

typedef struct {
	uint64_t ver;			// version number that inserted key value
} KeyVersion;

typedef struct {
	uint64_t id;			// child ID number
} ChildMap;

typedef struct {
	DbAddr addr;			// child RedBlack entry address
} ChildIdMap;

RedBlack *rbFind (DbMap *map, DbAddr root, uint8_t *key, uint32_t len, PathStk *path);
void *rbAdd (DbMap *parent, RWLock *lock, DbAddr *root, void *key, uint32_t keyLen, uint32_t amt);
typedef Status (*RbFcnPtr)(DbMap *parent, RedBlack *entry, void *params);
Status rbList(DbMap *parent, DbAddr *root, RbFcnPtr fcn, void *key, uint32_t keyLen, void *params);

enum ReaderWriterEnum {
	en_reader,
	en_writer,
	en_current,
	en_minimum
};

uint64_t addPQEntry(DbMap *map, uint32_t set, enum ReaderWriterEnum e);
void removePQEntry(DbMap *map, DbAddr addr);

/**
 * even =>  reader timestamp
 * odd  =>  writer timestamp
 */

bool isReader(uint64_t ts);
bool isWriter(uint64_t ts);
bool isCommitted(uint64_t ts);

uint64_t allocateTimestamp(DbMap *map, enum ReaderWriterEnum e);
uint64_t getTimestamp(DbMap *map, DbAddr addr);

uint64_t allocMap(DbMap *map, uint32_t size);
uint64_t allocObj(DbMap *map, DbAddr *free, int type, uint32_t size, bool zeroit);
void *getObj(DbMap *map, DbAddr addr); 
void closeMap(DbMap *map);

bool newSeg(DbMap *map, uint32_t minSize);
void mapSegs(DbMap *map);

uint64_t getNodeFromFrame (DbMap *map, DbAddr *queue);
bool getNodeWait (DbMap *map, DbAddr *queue, DbAddr *tail);
bool initObjFrame (DbMap *map, DbAddr *queue, uint32_t type, uint32_t size);
bool addSlotToFrame(DbMap *map, DbAddr *head, uint64_t addr);

void *fetchIdSlot (DbMap *map, DocId docId);
uint64_t allocDocId(DbMap *map, DbAddr *free, DbAddr *tail);
uint64_t getFreeFrame(DbMap *map);
uint64_t allocFrame(DbMap *map);

Status txnStep (DbMap *docStore, DocId txnId, DocId docId, TxnStepType type);

void *lockHandle(value_t hndl);
void unlockHandle(value_t hndl);

value_t cursorNext(DbCursor *cursor, DbMap *index);
value_t cursorPrev(DbCursor *cursor, DbMap *index);

uint64_t txnBegin (DbMap *db);
Status txnRollback (DbMap *db, uint64_t txnBits);
Status txnCommit (DbMap *db, uint64_t txnBits);


#pragma once

#define MAX_set 32		// maximum thread count
#define MAX_cursor 4096 // maximum cursor nodes
#define MAX_flds 64		// number of fields in compound index
#define MAX_key 4096	// maximum key length

typedef struct DbMap_ DbMap;
typedef struct Entry_ Entry;

enum DocType {
	FrameType,
	DocIdType,		// DocId value
	ChildType,		// child name list type
	MinDocType = 4,	// minimum document size in bits
	MaxDocType = 24	// each power of two, 4 - 24
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
			uint8_t nbyte;	// number of bytes in a span node
			uint8_t nslot;	// number of slots of frame in use
			uint8_t nbits;	// power of two for object size
			uint8_t ttype;	// index transaction type
		};
	};
	uint64_t bits;
	struct {
		uint64_t addr:48;
		uint64_t fill:16;
	};
} DbAddr;

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
	DbAddr collection;		// collection name
	DocId docId;			// document Id
} TxnStep;

typedef struct {
	uint64_t timestamp;		// committed txn timestamp
	DbAddr txnFrame[1];		// Frame chain of txn steps
	uint32_t set;			// set for the transaction
} Txn;

typedef enum {
	Txn_id,
	Txn_step,
	Txn_max
} TxnType;

#include "jsdb_dbarena.h"
#include "jsdb_dbdocs.h"
#include "jsdb_dbindex.h"
#include "jsdb_dbart.h"
#include "jsdb_dbbtree.h"

enum HandleType {
	hndl_newarena = 0,
	hndl_database,
	hndl_docStore,
	hndl_btreeIndex,
	hndl_artIndex,
	hndl_colIndex,
	hndl_iterator,
	hndl_btreeCursor,
	hndl_artCursor
};

#define FrameSlots 64

typedef struct {
	DbAddr next;			// next frame in queue
	DbAddr prev;			// prev frame in queue
	uint64_t timestamp;		// latest timestamp
	DbAddr slots[FrameSlots];// array of waiting/free slots
} Frame;

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

uint64_t allocateTimestamp(DbMap *map, enum ReaderWriterEnum e);
uint64_t getTimestamp(DbMap *map, DbAddr addr);

uint64_t allocMap(DbMap *map, uint32_t size);
uint64_t allocObj(DbMap *map, DbAddr *free, DbAddr *tail, int type, uint32_t size, bool zeroit);
void *getObj(DbMap *map, DbAddr addr); 
void closeMap(DbMap *map);

bool newSeg(DbMap *map, uint32_t minSize);
void mapSegs(DbMap *map);

uint64_t getNodeFromFrame (DbMap *map, DbAddr *queue);
bool getNodeWait (DbMap *map, DbAddr *queue, DbAddr *tail);
bool initObjFrame (DbMap *map, DbAddr *queue, uint32_t type, uint32_t size);
bool addSlotToFrame(DbMap *map, DbAddr *head, DbAddr *tail, uint64_t addr);

void *fetchIdSlot (DbMap *map, DocId docId);
uint64_t allocDocId(DbMap *map, DbAddr *free, DbAddr *tail);
uint64_t getFreeFrame(DbMap *map);
uint64_t allocFrame(DbMap *map);

Status txnStep (DbMap *collection, DocId txnId, DocId docId, TxnStepType type);

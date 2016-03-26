#pragma once

#define MAX_set 32		// maximum thread count
#define MAX_cursor 4096
#define MAX_key 4096

typedef struct DbMap_ DbMap;
typedef struct Entry_ Entry;

enum DocType {
	FrameType,
	DocIdType,		// DocId value
	ChildType,		// child name list type
	MaxDocType = 24	// each power of two, 3 - 24
};

enum TxnType {
	AddDoc,
	UpdDoc,
	DelDoc,
	AddKey,
	DelKey
};

typedef union {
	struct {
		uint32_t offset;	// offset in the segment
		uint16_t segment;	// arena segment number
		union {
			struct {
				uint8_t mutex:1;	// mutex bit;
				uint8_t dead:1;	// mutex bit, node type
				uint8_t type:6;	// object type
			};
			volatile char latch[1];
		};
		union {
			uint8_t nbyte;	// number of bytes in a span node
			uint8_t nslot;	// number of slots of frame in use
			uint8_t nbits;	// number of bits log_2 in document
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
	hndl_iterator,
	hndl_btreeCursor,
	hndl_artCursor
};

#define FrameSlots 40

typedef struct {
	DbAddr next;			// next frame in queue
	DbAddr prev;			// prev frame in queue
	uint64_t timestamp;		// latest timestamp
	DbAddr slots[FrameSlots];// array of waiting/free slots
} Frame;

enum ReaderWriterEnum {
	en_reader,
	en_writer,
	en_current
};

void addPQEntry(DbMap *map, uint32_t set, Entry* queue, enum ReaderWriterEnum e);
void removePQEntry(DbMap *map, Entry* queue);

/**
 * even =>  reader timestamp
 * odd  =>  writer timestamp
 */

bool isReader(uint64_t ts);
bool isWriter(uint64_t ts);

uint64_t allocateTimestamp(DbMap *map, enum ReaderWriterEnum e);

uint64_t allocMap(DbMap *map, uint32_t size);
uint64_t allocObj(DbMap *map, DbAddr *free, DbAddr *tail, int type, uint32_t size);
void *getObj(DbMap *map, DbAddr addr); 
void closeMap(DbMap *map);

bool newSeg(DbMap *map, uint32_t minSize);
void mapSegs(DbMap *map);

uint64_t getNodeFromFrame (DbMap *map, DbAddr *queue);
bool getNodeWait (DbMap *map, DbAddr *queue, DbAddr *tail);
bool initObjectFrame (DbMap *map, DbAddr *queue, uint32_t type, uint32_t size);
bool addNodeToFrame(DbMap *map, DbAddr *head, DbAddr *tail, DbAddr slot);

uint64_t allocDocId(DbMap *map, DbAddr *free, DbAddr *tail);
uint64_t getFreeFrame(DbMap *map);
uint64_t allocFrame(DbMap *map);

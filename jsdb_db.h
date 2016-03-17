#pragma once

#include "jsdb.h"

#define MAX_set 32		// maximum thread count
#define MAX_cursor 4096
#define MAX_key 4096

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
		};
	};
	uint64_t bits;
	struct {
		uint64_t addr:48;
		uint64_t fill:16;
	};
} DbAddr;

enum DocType {
	FrameType,
	DocIdType,		// DocId value
	ChildType,		// child name list type
	MaxDocType = 24	// each power of two, 3 - 24
};

enum HandleType {
	hndl_database,
	hndl_docStore,
	hndl_btreeIndex,
	hndl_artIndex,
	hndl_iterator,
	hndl_btreeCursor,
	hndl_artCursor
};

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

#define FrameSlots 40

typedef struct {
	DbAddr next;			// next frame in queue
	DbAddr prev;			// prev frame in queue
	uint64_t timestamp;		// latest timestamp
	DbAddr slots[FrameSlots];// array of waiting/free slots
} Frame;

typedef struct {
	DbAddr head[1];		// earliest frame waiting to be recycled
	DbAddr tail[1];		// location of latest frame to be recycle
	DbAddr free[1];		// frames of free objects
} FreeList;

typedef struct DbMap_ DbMap;
typedef struct Entry_ Entry;

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

uint64_t arenaAlloc(DbMap *map, uint32_t size);
uint64_t allocObj(DbMap *map, DbAddr *free, DbAddr *tail, int type, uint32_t size);
void *getObj(DbMap *map, DbAddr addr); 
void closeMap(DbMap *map);

bool newSeg(DbMap *map, uint32_t minSize);
void mapSegs(DbMap *map);

uint64_t getNodeFromFrame (DbMap *map, DbAddr *queue);
bool getNodeWait (DbMap *map, DbAddr *queue, DbAddr *tail);
bool initObjectFrame (DbMap *map, DbAddr *queue, uint32_t type, uint32_t size);
bool addNodeToFrame(DbMap *map, DbAddr *head, DbAddr *tail, DbAddr slot);

uint64_t getFreeFrame(DbMap *map);
uint64_t allocFrame( DbMap *map);

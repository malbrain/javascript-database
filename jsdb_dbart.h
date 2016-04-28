#pragma once

// Artree interior nodes

enum ARTNodeType {
	UnusedSlot = 0,	// slot is not yet in use
	SpanNode,		// node contains up to 8 key bytes
	Array4,			// node contains 4 radix slots
	Array14,		// node contains 14 radix slots
	Array64,		// node contains 64 radix slots
	Array256,		// node contains 256 radix slots
	FldEnd,			// node end of a key field
	Suffix,			// node end field/start of suffix
	KeyEnd,			// node end of the complete key
	MaxARTType
};

/**
 * radix node with four slots and their key bytes
 */

typedef struct {
	uint64_t timestamp;
	volatile uint8_t alloc;
	volatile uint8_t keys[4];
	uint8_t filler[3];
	DbAddr radix[4];
} ARTNode4;

/**
 * radix node with fourteen slots and their key bytes
 */

typedef struct {
	uint64_t timestamp;
	volatile uint16_t alloc;
	volatile uint8_t keys[14];
	DbAddr radix[14];
} ARTNode14;

/**
 * radix node with sixty-four slots and a 256 key byte array
 */

typedef struct {
	uint64_t timestamp;
	volatile uint64_t alloc;
	volatile uint8_t keys[256];
	DbAddr radix[64];
} ARTNode64;

/**
 * radix node all 256 slots
 */

typedef struct {
	uint64_t timestamp;
	volatile uint64_t alloc[4];
	DbAddr radix[256];
} ARTNode256;

/**
 * span node containing up to 8 consecutive key bytes
 * span nodes are used to compress linear chains of key bytes
 */

typedef struct {
	uint64_t timestamp;
	uint8_t bytes[8];
	DbAddr next[1];		// next node under span
} ARTSpan;

/**
 * Suffix/FldEnd Key node
 */

typedef struct {
	DbAddr next[1];		// begin next key field
	DbAddr pass[1];		// continue current field key
} ARTEnd;

typedef struct {
	DbIndex idx[1];		// keys and partial
	DbAddr root[1];		// root of the tree
	uint64_t numEntries[1];
	FreeList freeLists[MAX_set][MaxARTType]; // nodes awaiting reclamation
} ArtIndex;

typedef struct {
	DbAddr slot[1];	// slot that points to node
	DbAddr *addr;	// tree addr of slot
	uint16_t off;	// offset within key
	int16_t ch;		// character of key
	bool dir;
} CursorStack;

typedef struct {
	uint32_t off;
	uint32_t len;
} CursorFld;

typedef struct {
	DbMap *index;					// cursor index
	bool atLeftEOF;					// needed to support 'atEOF()'
	bool atRightEOF;				// needed to support 'atEOF()'
	bool direction;					// true is forward
	uint32_t depth;					// current depth of cursor
	uint32_t keySize;				// current size of the key
	uint32_t keyFlds;				// number of key fields
	uint32_t limitFlds;				// number of limit fields
	uint64_t timestamp;				// cursor snapshot timestamp
	uint8_t key[MAX_key];			// current cursor key
	uint8_t limit[MAX_key];			// limiting key field values
	CursorFld fields[MAX_flds];		// current cursor key components
	CursorFld limits[MAX_flds];		// cursor limit key fields
	CursorStack stack[MAX_cursor];	// cursor stack
} ArtCursor;

#define artIndexAddr(map)((ArtIndex *)(map->arena + 1))

value_t artCursor(DbMap *index, bool direction, value_t start, value_t limit);
value_t artCursorKey(ArtCursor *cursor);
uint64_t artDocId(ArtCursor *cursor);
bool artNextKey(ArtCursor *cursor);
bool artPrevKey(ArtCursor *cursor);

DbAddr *artFindNxtFld( DbMap *index, ArtCursor *cursor, DbAddr *slot, uint8_t *key, uint32_t keylen);
bool artSeekKey(ArtCursor *cursor, uint8_t *key, uint32_t keylen);
DbAddr *artAppendKeyFld( DbMap *index, DbAddr *base, uint32_t set, uint8_t *key, uint32_t keylen);
Status createArtIndex(DbMap *docStore, value_t keys, value_t name, uint32_t size, bool onDisk, bool unique, value_t partial, uint32_t set);
Status artIndexKey (DbMap *map, DbMap *index, DbAddr docAddr, DocId docId, uint32_t set, uint64_t txnId);
uint64_t artAllocateNode(DbMap *index, uint32_t set, int type, uint32_t size);


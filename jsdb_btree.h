#pragma once

typedef struct {
	DbAddr btreePage;
	uint64_t timestamp;
	uint32_t slot;
	bool direction;
	DbMap *index;
} BtreeCursor;

typedef struct {
	DbAddr rootPage;
} BtreeIndex;

#define btreeIndexAddr(map)((BtreeIndex *)(map->arena + 1))

value_t btreeCursor(DbMap *btree, bool origin);
value_t btreeCursorKey(BtreeCursor *cursor);

DbAddr *btreeFindKey(DbMap  *map, BtreeCursor *cursor, uint8_t *key, uint32_t keylen);
bool btreeSeekKey(BtreeCursor *cursor, uint8_t *key, uint32_t keylen);

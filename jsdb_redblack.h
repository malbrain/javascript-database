#pragma once
 
#define RB_bits		24
#define getRb(x,y)	((RedBlack *)getObj(x,y))

//	red-black tree descent stack

typedef struct {
	int lvl;				// height of the stack
	int gt;					// node is > given key
	DbAddr entry[RB_bits];	// stacked tree descent
} PathStk;

uint64_t rbFind (DbMap *map, uint8_t *key, uint32_t len, PathStk *path);
void rbInsert (DbMap *map, DbAddr slot, PathStk *path);
void rbRemove (DbMap *map, DbAddr slot, PathStk *path);

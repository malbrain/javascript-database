#include "jsdb.h"
#include "jsdb_arena.h"
#include "jsdb_btree.h"

Status createBtreeIndex(DbMap *docStore, value_t keys, value_t name, uint32_t size, bool onDisk, bool unique, value_t partial) {

	DbMap *index = openMap(name.str, name.aux, docStore, sizeof(BtreeIndex), size, sizeof(BtreeLocal), onDisk);

	return OK;
}

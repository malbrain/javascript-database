#include "jsdb.h"
#include "jsdb_db.h"

Status createBtreeIndex(DbMap *docStore, value_t keys, value_t name, uint32_t size, bool onDisk, bool unique, value_t partial, uint32_t set) {

	DbMap *index = openMap(name.str, name.aux, docStore, sizeof(BtreeIndex), sizeof(BtreeLocal), size, onDisk);

	//  mark initialization finished

	index->arena->type = hndl_btreeIndex;
	return OK;
}

#include "jsdb.h"
#include "jsdb_arena.h"
#include "jsdb_art.h"

Status createArtIndex(DbMap *docStore, value_t keys, value_t name, uint32_t size, bool onDisk, bool unique, value_t partial) {

	DbMap *index = openMap(name.str, name.aux, docStore, sizeof(ArtIndex), 0, size, onDisk);
	return OK;
}

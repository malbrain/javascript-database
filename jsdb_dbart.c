#include "jsdb.h"
#include "jsdb_db.h"

Status createArtIndex(DbMap *docStore, value_t keys, value_t name, uint32_t size, bool onDisk, bool unique, value_t partial, uint32_t set) {

	DbMap *index = openMap(name.str, name.aux, docStore, sizeof(ArtIndex), 0, size, onDisk);
	artIndexAddr(index)->idx->keys.bits = marshal_doc(index, keys, set);
	index->arena->type = hndl_artIndex;
	return OK;
}

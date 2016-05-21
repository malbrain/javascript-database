#include "jsdb.h"
#include "jsdb_db.h"

//  construct a db cursor
//  given the index
//  and a field array

value_t makeCursor(value_t indexHndl, bool reverse, value_t start, value_t limit) {
	DbMap *index = indexHndl.hndl;
	value_t result, s;

	s.bits = vt_status;

	switch (index->arena->type) {
	case Hndl_btreeIndex:
		result = btreeCursor(indexHndl, reverse, start, limit);
		break;
		
	case Hndl_artIndex:
		result = artCursor(indexHndl, reverse, start, limit);
		break;

	default:
		fprintf(stderr, "Error: makeCursor => invalid index type => %d\n", index->arena->type);
		return s.status = ERROR_script_internal, s;
	}

	return result;
}

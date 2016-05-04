#include "jsdb.h"
#include "jsdb_db.h"

//  construct a db cursor
//  given the index
//  and a field array

value_t makeCursor(DbMap *index, bool direction, value_t start, value_t limit) {
	value_t result, s;

	s.bits = vt_status;

	switch (index->arena->type) {
	case hndl_btreeIndex:
		result = btreeCursor(index, direction, start, limit);
		break;
		
	case hndl_artIndex:
		result = artCursor(index, direction, start, limit);
		break;

	default:
		fprintf(stderr, "Error: makeCursor => invalid index type => %d\n", index->arena->type);
		return s.status = ERROR_script_internal, s;
	}

	return result;
}

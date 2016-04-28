#include "jsdb.h"
#include "jsdb_db.h"

//  construct a db cursor
//  given the index
//  and a field array

value_t makeCursor(DbMap *index, bool direction, value_t fields, value_t limit) {
	value_t result, s;

	s.bits = vt_status;

	switch (index->arena->type) {
	case hndl_btreeIndex:
		result = btreeCursor(index, direction, fields, limit);
		break;
		
	case hndl_artIndex:
		result = artCursor(index, direction, fields, limit);
		break;
	default:
		fprintf(stderr, "Error: makeCursor => invalid index type => %d\n", index->arena->type);
		return s.status = ERROR_script_internal, s;
	}

	return result;
}

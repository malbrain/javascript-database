#include "jsdb.h"
#include "jsdb_db.h"

uint64_t btreeDocId(BtreeCursor *cursor) {
//	return get64(cursor->key + cursor->keySize - sizeof(SuffixBytes));
	return 0;
}

value_t btreeCursor(DbMap *index, bool direction, value_t fields, value_t limits) {
	BtreeCursor *cursor;
	value_t val;

	val.bits = vt_handle;
	val.aux = hndl_btreeCursor;
	val.hndl = jsdb_alloc(sizeof(BtreeCursor), true);
	val.refcount = 1;

	cursor = val.hndl;
	cursor->index = index;
	cursor->direction = direction;
	cursor->timestamp = allocateTimestamp(index, en_reader);
	return val;
}

value_t btreeCursorKey(BtreeCursor *cursor) {
	value_t val;

	val.bits = vt_string;
//	val.aux = cursor->keySize;
//	val.str = cursor->key;
	return val;
}

bool btreeSeekKey (BtreeCursor *cursor, uint8_t *key, uint32_t keylen) {
	return true;
}

bool btreeNextKey (BtreeCursor *cursor) {
	return true;
}

bool btreePrevKey (BtreeCursor *cursor) {
	return true;
}

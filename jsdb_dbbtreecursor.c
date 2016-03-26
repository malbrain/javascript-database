#include "jsdb.h"
#include "jsdb_db.h"

uint64_t btreeDocId(BtreeCursor *cursor) {
//	uint8_t *suffix = cursor->key + cursor->keySize - sizeof(SuffixBytes);
	uint64_t docId = 0, txnId, docTxn;

//	docId = get64(suffix);
//	docId = get64(suffix + sizeof(uint64_t));
//	docTxn = get64(suffix + 2 * sizeof(uint64_t));
	return docId;
}

value_t btreeCursor(DbMap *index, bool direction) {
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

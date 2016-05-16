#include "jsdb.h"
#include "jsdb_db.h"

uint64_t btreeDocId(BtreeCursor *cursor) {
	uint8_t *ptr = keyptr(cursor->page, cursor->slotIdx);
	KeySuffix *suffix = (KeySuffix *)(ptr + keypre(ptr) + keylen(ptr) - sizeof(KeySuffix));
	return get64(suffix->docId);
}

value_t btreeCursor(value_t indexHndl, bool reverse, value_t fields, value_t limits) {
	DbMap *index = indexHndl.hndl;
	BtreeCursor *cursor;
    BtreeIndex *btree;
	BtreePage *page;
	value_t val;

    btree = btreeIndex(index);
	incrRefCnt(indexHndl);

	val.bits = vt_handle;
	val.aux = hndl_btreeCursor;
	val.hndl = jsdb_alloc(sizeof(BtreeCursor), true);
	val.refcount = 1;

	cursor = val.hndl;
	cursor->indexHndl = indexHndl;
	cursor->timestamp = allocateTimestamp(index, en_reader);
	cursor->pageAddr.bits = jsdb_rawalloc(btree->pageSize << btree->leafXtra, false);
	cursor->page = jsdb_rawaddr(cursor->pageAddr.bits);

	if (!reverse) {
		page = getObj(index, btree->leaf);
		memcpy (cursor->page, page, btree->pageSize << btree->leafXtra);
		cursor->slotIdx = 0;
	}

	return val;
}

value_t btreeCursorKey(BtreeCursor *cursor) {
	uint8_t *key = keyptr(cursor->page, cursor->slotIdx);
	value_t val;

	val.bits = vt_string;
	val.aux = keylen(key);
	val.str = key + keypre(key);
	return val;
}

bool btreeSeekKey (BtreeCursor *cursor, uint8_t *key, uint32_t keylen) {
	return true;
}

bool btreeNextKey (BtreeCursor *cursor) {
	DbMap *index = cursor->indexHndl.hndl;
	BtreeIndex *btree = btreeIndex(index);
	BtreePage *page;

	while (true) {
	  uint32_t max = cursor->page->cnt;

	  if (!cursor->page->right.bits)
		max--;

	  while (cursor->slotIdx < max)
		if (slotptr(cursor->page, ++cursor->slotIdx)->dead)
		  continue;
		else
		  return true;

	  if (cursor->page->right.bits)
		page = getObj(index, cursor->page->right);
	  else
		return false;

	  memcpy (cursor->page, page, btree->pageSize << btree->leafXtra);
	  cursor->slotIdx = 0;
	}
}

bool btreePrevKey (BtreeCursor *cursor) {
	DbMap *index = cursor->indexHndl.hndl;
	BtreeIndex *btree = btreeIndex(index);
	BtreePage *page;

	while (true) {
	  if (cursor->slotIdx)
		if (slotptr(cursor->page, --cursor->slotIdx)->dead)
		  continue;
		else
		  return true;

	  if (cursor->page->left.bits)
		page = getObj(index, cursor->page->left);
	  else
		return false;

	  memcpy (cursor->page, page, btree->pageSize << btree->leafXtra);
	  cursor->slotIdx = cursor->page->cnt;
	}
}

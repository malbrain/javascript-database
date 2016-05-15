#include "jsdb.h"
#include "jsdb_db.h"

uint64_t btreeDocId(uint8_t *ptr) {
	KeySuffix *suffix = (KeySuffix *)(ptr + keylen(ptr) - sizeof(KeySuffix));
	return get64(suffix->docId);
}

value_t btreeCursor(value_t indexHndl, bool direction, value_t fields, value_t limits) {
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
	cursor->direction = direction;
	cursor->indexHndl.bits = indexHndl.bits;
	cursor->timestamp = allocateTimestamp(index, en_reader);
	cursor->page = jsdb_alloc(btree->pageSize, false);

	if (direction) {
		page = getObj(index, btree->root);
		memcpy (cursor->page, page, btree->pageSize);
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

	while (cursor->direction) {
	  while (cursor->slotIdx < cursor->page->cnt)
		if (slotptr(cursor->page, ++cursor->slotIdx)->dead)
		  continue;
		else
		  return true;

	  if (cursor->page->right.bits)
		page = getObj(index, cursor->page->right);
	  else
		return false;

	  memcpy (cursor->page, page, btree->pageSize);
	  cursor->slotIdx = 0;
	}

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

	  memcpy (cursor->page, page, btree->pageSize);
	  cursor->slotIdx = cursor->page->cnt;
	}
}

bool btreePrevKey (BtreeCursor *cursor) {
	DbMap *index = cursor->indexHndl.hndl;
	BtreeIndex *btree = btreeIndex(index);
	BtreePage *page;

	while (!cursor->direction) {
	  while (cursor->slotIdx < cursor->page->cnt)
		if (slotptr(cursor->page, ++cursor->slotIdx)->dead)
		  continue;
		else
		  return true;

	  if (cursor->page->right.bits)
		page = getObj(index, cursor->page->right);
	  else
		return false;

	  memcpy (cursor->page, page, btree->pageSize);
	  cursor->slotIdx = 0;
	}

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

	  memcpy (cursor->page, page, btree->pageSize);
	  cursor->slotIdx = cursor->page->cnt;
	}
}

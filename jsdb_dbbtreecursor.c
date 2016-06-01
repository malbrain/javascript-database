#include "jsdb.h"
#include "jsdb_db.h"

uint64_t btreeDocId(BtreeCursor *cursor) {
	uint8_t *ptr = keyptr(cursor->page, cursor->slotIdx);
	KeySuffix *suffix = (KeySuffix *)(ptr + keypre(ptr) + keylen(ptr) - sizeof(KeySuffix));
	return get64(suffix->docId);
}

value_t btreeCursor(value_t hndl, DbMap *index, bool reverse, value_t fields, value_t limits) {
	BtreeCursor *cursor;
    BtreeIndex *btree;
	BtreePage *page;
	value_t val;

    btree = btreeIndex(index);

	val.bits = vt_handle;
	val.subType = Hndl_btreeCursor;
	val.handle = jsdb_alloc(sizeof(BtreeCursor), true);
	val.refcount = 1;
	incrRefCnt(val);

	cursor = val.handle;
	cursor->hdr->hndl = hndl;
	cursor->hdr->pqAddr.bits = addPQEntry(index, getSet(index), en_reader);
	cursor->hdr->timestamp = getTimestamp(index, cursor->hdr->pqAddr);

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

bool btreeSeekKey (BtreeCursor *cursor, DbMap *index, uint8_t *key, uint32_t keylen) {
	return true;
}

uint64_t btreeNextKey (BtreeCursor *cursor, DbMap *index) {
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
		  return btreeDocId(cursor);

	  if (cursor->page->right.bits)
		page = getObj(index, cursor->page->right);
	  else
		return 0;

	  memcpy (cursor->page, page, btree->pageSize << btree->leafXtra);
	  cursor->slotIdx = 0;
	}
}

uint64_t btreePrevKey (BtreeCursor *cursor, DbMap *index) {
	BtreeIndex *btree = btreeIndex(index);
	BtreePage *page;

	while (true) {
	  if (cursor->slotIdx) {
		if (slotptr(cursor->page, --cursor->slotIdx)->dead)
		  continue;
		else
		  return btreeDocId(cursor);
	  }

	  if (cursor->page->left.bits)
		page = getObj(index, cursor->page->left);
	  else
		return 0;

	  memcpy (cursor->page, page, btree->pageSize << btree->leafXtra);
	  cursor->slotIdx = cursor->page->cnt;
	}
}

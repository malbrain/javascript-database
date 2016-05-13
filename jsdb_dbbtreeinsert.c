#include "jsdb.h"
#include "jsdb_db.h"

Status btreeInsertKey(BtreeIndex *btree, uint8_t *key, uint32_t keyLen, uint8_t lvl) {
	uint32_t totKeyLen = keyLen;
	BtreeSlotType type;
	BtreeSet set[1];
	BtreeSlot *slot;
	uint8_t *ptr;
	Status stat;

	if (lvl)
		type = Btree_interior;
	else
		type = Btree_leafPage;

	if (keyLen < 128)
		totKeyLen += 1;
	else
		totKeyLen += 2;

	while (true) {
	  if ((stat = btreeLoadPage(btree, set, key, keyLen, lvl, Btree_lockWrite)))
		return stat;

	  slot = slotptr(set->page, set->slotIdx);
	  ptr = keyptr(set->page, set->slotIdx);

	  // if librarian slot

	  if (slot->type == Btree_librarian) {
		slot = slotptr(set->page, ++set->slotIdx);
		ptr = keyptr(set->page, set->slotIdx);
	  }

	  if ((stat = btreeCleanPage(btree, set, totKeyLen))) {
		if (stat == BTREE_needssplit) {
		  if ((stat = btreeSplitPage(btree, set)))
			return stat;
		  else
			continue;
	    } else
			return stat;
	  }

	  // add the key to the page

	  return btreeInsertSlot (btree, set, key, keyLen, type);
	}
	return OK;
}

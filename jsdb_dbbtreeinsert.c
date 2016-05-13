#include "jsdb.h"
#include "jsdb_db.h"

void btreeInsertSlot (BtreeIndex *btree, BtreeSet *set, uint8_t *key, uint32_t keyLen, BtreeSlotType type);

Status btreeInsertKey(DbMap *index, uint8_t *key, uint32_t keyLen, uint8_t lvl) {
	BtreeIndex *btree = btreeIndexAddr(index);
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
	  if ((stat = btreeLoadPage(index, set, key, keyLen, lvl, Btree_lockWrite)))
		return stat;

	  slot = slotptr(set->page, set->slotIdx);
	  ptr = keyptr(set->page, set->slotIdx);

	  // if librarian slot

	  if (slot->type == Btree_librarian) {
		slot = slotptr(set->page, ++set->slotIdx);
		ptr = keyptr(set->page, set->slotIdx);
	  }

	  if ((stat = btreeCleanPage(index, set, totKeyLen))) {
		if (stat == BTREE_needssplit) {
		  if ((stat = btreeSplitPage(index, set)))
			return stat;
		  else
			continue;
	    } else
			return stat;
	  }

	  // add the key to the page

	  btreeInsertSlot (btree, set, key, keyLen, type);
	  return OK;
	}

	return OK;
}

//	update page's fence key in its parent

Status btreeFixKey (DbMap *index, uint8_t *fenceKey, uint8_t lvl) {
	uint32_t totKeyLen = keylen(fenceKey) + keypre(fenceKey);
	uint32_t keyLen = keylen(fenceKey);
	BtreeSet set[1];
	BtreeSlot *slot;
	uint8_t *ptr;
	Status stat;

	if ((stat = btreeLoadPage(index, set, fenceKey + keypre(fenceKey), keyLen - sizeof(uint64_t), lvl, Btree_lockWrite)))
		return stat;

	slot = slotptr(set->page, set->slotIdx);
	ptr = keyptr(set->page, set->slotIdx);

	// if librarian slot

	if (slot->type == Btree_librarian) {
		slot = slotptr(set->page, ++set->slotIdx);
		ptr = keyptr(set->page, set->slotIdx);
	}

	// update child pointer value

	memcpy(ptr + keypre(ptr) + keylen(ptr) - sizeof(uint64_t), fenceKey + keypre(fenceKey) + keylen(fenceKey) - sizeof(uint64_t), sizeof(uint64_t));

	// release write lock

	btreeUnlockPage (set->page, Btree_lockWrite);
	return OK;
}

//	install new key onto page
//	page must already be checked for
//	adequate space

void btreeInsertSlot (BtreeIndex *btree, BtreeSet *set, uint8_t *key, uint32_t keyLen, BtreeSlotType type) {
	uint32_t idx, librarian;
	uint32_t prefixLen;
	BtreeSlot *slot;
	uint8_t *ptr;

	//	if found slot > desired slot and previous slot
	//	is a librarian slot, use it

	if( set->slotIdx > 1 )
	  if( slotptr(set->page, set->slotIdx-1)->type == Btree_librarian )
		set->slotIdx--;

	//	calculate key length

	prefixLen = keyLen < 128 ? 1 : 2;

	// copy key onto page

	set->page->min -= prefixLen + keyLen;
	ptr = keyaddr(set->page, set->page->min);

	if( keyLen < 128 )	
		*ptr++ = keyLen;
	else
		*ptr++ = keyLen/256 | 0x80, *ptr++ = keyLen;

	memcpy (ptr, key, keyLen);
	
	//	find first empty slot

	for( idx = set->slotIdx; idx < set->page->cnt; idx++ )
	  if( slotptr(set->page, idx)->dead )
		break;

	// now insert key into array before slot

	if( idx == set->page->cnt )
		idx += 2, set->page->cnt += 2, librarian = 2;
	else
		librarian = 1;

	set->page->act++;

	while( idx > set->slotIdx + librarian - 1 )
		*slotptr(set->page, idx) = *slotptr(set->page, idx - librarian), idx--;

	//	add librarian slot

	if( librarian > 1 ) {
		slot = slotptr(set->page, set->slotIdx++);
		slot->off = set->page->min;
		slot->type = Btree_librarian;
		slot->dead = 1;
	}

	//	fill in new slot

	slot = slotptr(set->page, set->slotIdx);
	slot->off = set->page->min;
	slot->type = type;
	slot->dead = 0;

	btreeUnlockPage (set->page, Btree_lockWrite);
}


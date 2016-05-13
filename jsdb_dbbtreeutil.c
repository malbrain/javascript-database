#include "jsdb.h"
#include "jsdb_db.h"

// place write, read, or parent lock on requested page_no.

void btreeLockPage(BtreePage *page, BtreeLock mode)
{
	switch( mode ) {
	case Btree_lockRead:
		readLock (page->readwr);
		break;
	case Btree_lockWrite:
		writeLock (page->readwr);
		break;
	case Btree_lockAccess:
		readLock (page->access);
		break;
	case Btree_lockDelete:
		writeLock (page->access);
		break;
	case Btree_lockParent:
		writeLock (page->parent);
		break;
	case Btree_lockLink:
		writeLock (page->link);
		break;
	}
}

void btreeUnlockPage(BtreePage *page, BtreeLock mode)
{
	switch( mode ) {
	case Btree_lockWrite:
		rwUnlock (page->readwr);
		break;
	case Btree_lockRead:
		rwUnlock (page->readwr);
		break;
	case Btree_lockAccess:
		rwUnlock (page->access);
		break;
	case Btree_lockDelete:
		rwUnlock (page->access);
		break;
	case Btree_lockParent:
		rwUnlock (page->parent);
		break;
	case Btree_lockLink:
		rwUnlock (page->link);
		break;
	}
}

//  split already locked full node
//	return unlocked.

Status btreeSplitPage (BtreeIndex *btree, BtreeSet *set) {
	uint32_t cnt = 0, idx = 0, max, nxt, size = btree->pageSize;
	uint8_t leftKey[MAX_key], rightKey[MAX_key];
	DbMap *index = btreeMapAddr(btree);
	BtreePage *frame, *rightPage;
	uint8_t lvl = set->page->lvl;
	DbAddr right, addr;
	BtreeSlotType type;
	uint32_t totLen;
	uint8_t *key;
	Status stat;

	if( !set->page->lvl ) {
		size <<= btree->leafXtra;
		type = Btree_leafPage;
	} else {
		type = Btree_interior;
	}

	if( (addr.bits = allocObj(index, &btree->freePages[type], NULL, size, type, true)) )
		frame = getObj(index, addr);
	else
		return ERROR_outofmemory;

	//  split higher half of keys to temp frame

	max = set->page->cnt;
	cnt = max / 2;
	nxt = size;
	idx = 0;

	while( cnt++ < max ) {
		if( cnt < max || set->page->lvl )
		  if( slotptr(set->page, cnt)->dead && cnt < max )
			continue;

		key = keyptr(set->page, cnt);
		totLen = keylen(key) + keypre(key);
		nxt -= totLen;

		memcpy ((uint8_t *)frame + nxt, key, totLen);

		//	add librarian slot

		slotptr(frame, ++idx)->off = nxt;
		slotptr(frame, idx)->type = Btree_librarian;
		slotptr(frame, idx)->dead = 1;

		//  add actual slot

		slotptr(frame, ++idx)->off = nxt;
		slotptr(frame, idx)->type = slotptr(set->page, cnt)->type;

		if( !(slotptr(frame, idx)->dead = slotptr(set->page, cnt)->dead) )
			frame->act++;
	}

	// remember existing fence key for new page to the right

	memcpy (rightKey, key, totLen);

	frame->min = nxt;
	frame->cnt = idx;
	frame->lvl = lvl;

	// link right node

	if( set->page_no.type != Btree_rootPage ) {
		frame->right.bits = set->page->right.bits;
		frame->left.bits = set->page_no.bits;
	}

	//	get new page and write higher keys to it.

	if( (right.bits = btreeNewPage(index, lvl, type)) )
		rightPage = getObj(index, right);
	else
		return ERROR_outofmemory;

	memcpy (rightPage, frame, size);

	if( set->page_no.type != Btree_rootPage && !lvl )
	  if( (stat = btreeLinkLeft (btree, right, set->page->right) ))
		return stat;

	//	update lower keys to continue in old page

	memcpy (frame, set->page, size);
	memset (set->page+1, 0, size - sizeof(*set->page));

	set->page->garbage = 0;
	set->page->act = 0;
	nxt = size;
	max /= 2;
	cnt = 0;
	idx = 0;

	if( slotptr(frame, max)->type == Btree_librarian )
		max--;

	//  assemble page of smaller keys

	while( cnt++ < max ) {
		if( slotptr(frame, cnt)->dead )
			continue;

		key = keyptr(frame, cnt);
		totLen = keylen(key) + keypre(key);
		nxt -= totLen;

		memcpy ((uint8_t *)set->page + nxt, key, totLen);

		//	add librarian slot

		slotptr(set->page, ++idx)->off = nxt;
		slotptr(set->page, idx)->type = Btree_librarian;
		slotptr(set->page, idx)->dead = 1;

		//	add actual slot

		slotptr(set->page, ++idx)->off = nxt;
		slotptr(set->page, idx)->type = slotptr(frame, cnt)->type;
		set->page->act++;
	}

	// remember fence key for smaller page

	memcpy(leftKey, key, totLen);

	set->page->right.bits = right.bits;
	set->page->min = nxt;
	set->page->cnt = idx;

	// if current page is the root page, split it

	if( set->page_no.type == Btree_rootPage )
		return btreeSplitRoot (btree, set, leftKey, right);

	// insert new fences in their parent pages

	btreeLockPage (rightPage, Btree_lockParent);
	btreeLockPage (set->page, Btree_lockParent);
	btreeLockPage (set->page, Btree_lockWrite);

	// insert new fence for reformulated left block of smaller keys

	totLen = keylen(leftKey) + keypre(leftKey);
	store64(leftKey + totLen - sizeof(uint64_t), set->page_no.bits);

	if( (stat = btreeInsertKey (btree, leftKey + keypre(leftKey), keylen(leftKey), lvl+1, Btree_interior) ))
		return stat;

	// switch fence for right block of larger keys to new right page

	totLen = keylen(rightKey) + keypre(rightKey);
	store64(rightKey + totLen - sizeof(uint64_t), right.bits);

	if( (stat = btreeFixKey (btree, rightKey, lvl+1, Btree_interior)) )
		return stat;

	btreeUnlockPage (set->page, Btree_lockParent);
	btreeUnlockPage (rightPage, Btree_lockParent);
	return OK;
}

//	check page for space available,
//	clean if necessary and return
//	false - page needs splitting
//	true  - ok to insert

Status btreeCleanPage(BtreeIndex *btree, BtreeSet *set, uint32_t totKeyLen) {
	DbMap *index = btreeMapAddr(btree);
	uint32_t size = btree->pageSize;
	BtreePage *page = set->page;
	uint32_t cnt = 0, idx = 0;
	uint32_t max = page->cnt;
	uint32_t newslot = max;
	BtreeSlotType type;
	BtreePage *frame;
	uint8_t *key;
	uint32_t len;
	DbAddr addr;

	if( !page->lvl ) {
		size <<= btree->leafXtra;
		type = Btree_leafPage;
	} else {
		type = Btree_interior;
	}

	if( page->min >= (max+2) * sizeof(BtreeSlot) + sizeof(*page) + totKeyLen )
		return OK;

	//	skip cleanup and proceed directly to split
	//	if there's not enough garbage
	//	to bother with.

	if( page->garbage < size / 5 )
		return BTREE_needssplit;

	if( (addr.bits = allocObj(index, &btree->freePages[type], NULL, size, type, false)) )
		frame = getObj(index, addr);
	else
		return ERROR_outofmemory;

	memcpy (frame, page, size);

	// skip page info and set rest of page to zero

	memset (page+1, 0, size - sizeof(*page));
	page->garbage = 0;
	page->act = 0;

	// clean up page first by
	// removing deleted keys

	while( cnt++ < max ) {
		if( cnt == set->slotIdx )
			newslot = idx + 2;

		if( cnt < max || frame->lvl )
		  if( slotptr(frame,cnt)->dead )
			continue;

		// copy the active key across

		key = keyptr(frame, cnt);
		len = keylen(key) + keypre(key);
		size -= len;

		memcpy ((uint8_t *)page + size, key, len);

		// make a librarian slot

		slotptr(page, ++idx)->off = size;
		slotptr(page, idx)->type = Btree_librarian;
		slotptr(page, idx)->dead = 1;

		// set up the slot

		slotptr(page, ++idx)->off = size;
		slotptr(page, idx)->type = slotptr(frame, cnt)->type;

		if( !(slotptr(page, idx)->dead = slotptr(frame, cnt)->dead) )
			page->act++;
	}

	page->min = size;
	page->cnt = idx;

	//  return temporary frame

	addNodeToFrame(index, &btree->freePages[type], NULL, addr);

	//	see if page has enough space now, or does it still need splitting?

	if( page->min >= (idx+2) * sizeof(BtreeSlot) + sizeof(*page) + totKeyLen )
		return OK;

	return BTREE_needssplit;
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
		*ptr++ = keyLen/128 | 0x80, *ptr++ = keyLen;

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

//  compare two keys, return > 0, = 0, or < 0
//  =0: keys are same
//  -1: key2 > key1
//  +1: key2 < key1

int btreeKeyCmp (uint8_t *key1, uint8_t *key2, uint32_t len2) {
	uint32_t len1 = keylen(key1);
	uint32_t fld1, fld2;
	int ans;

	key1 += keypre(key1);

	//  check key fields

	while( (fld1 = keylen(key1)) ) {
		if( (fld2 = keylen(key2)) ) {
			key1 += keypre(key1);
			key2 += keypre(key2);
		}
		else
			return 1;

		if( ans = memcmp (key1, key2, fld1 > fld2 ? fld2 : fld1) )
			return ans;

		if( fld1 > fld2 )
			return 1;

		if( fld1 < fld2 )
			return -1;

		key1 += fld1;
		key2 += fld2;
		len2 -= keypre(key2) + fld2;
	}

	return 0;
}

//  find slot in page for given key at a given level

uint32_t btreeFindSlot (BtreePage *page, uint8_t *key, uint32_t keyLen)
{
uint32_t diff, higher = page->cnt, low = 1, slot;
uint32_t good = 0;

	//	  make stopper key an infinite fence value

	if( page->right.bits )
		higher++;
	else
		good++;

	//	low is the lowest candidate.
	//  loop ends when they meet

	//  higher is already
	//	tested as .ge. the passed key.

	while( diff = higher - low ) {
		slot = low + ( diff >> 1 );
		if( btreeKeyCmp (keyptr(page, slot), key, keyLen) < 0 )
			low = slot + 1;
		else
			higher = slot, good++;
	}

	//	return zero if key is on next right page

	return good ? higher : 0;
}

//  find and load page at given level for given key
//	leave page rd or wr locked as requested

Status btreeLoadPage(BtreeIndex *btree, BtreeSet *set, uint8_t *key, uint32_t keyLen, uint8_t lvl, BtreeLock lock) {
  DbMap *index = btreeMapAddr(btree);
  DbAddr page_no, prevpage_no;
  uint8_t drill = 0xff, *ptr;
  BtreePage *prevpage = NULL;
  BtreeLock mode, prevmode;
  uint32_t slotIdx;

  page_no.bits = btree->root.bits;
  prevpage_no.bits = 0;

  //  start at our idea of the root level of the btree and drill down

  do {
	// determine lock mode of drill level
	mode = (drill == lvl) ? lock : Btree_lockRead; 
	set->page = getObj(index, page_no);

 	// obtain access lock using lock chaining with Access mode

	if( drill < 0xff )
	  btreeLockPage(set->page, Btree_lockAccess);

	//	release & unpin parent or left sibling page

	if( prevpage_no.bits ) {
	  btreeUnlockPage(prevpage, prevmode);
	  prevpage_no.bits = 0;
	}

 	// obtain mode lock using lock chaining through AccessLock

	btreeLockPage(prevpage, mode);

	if( set->page->free )
		return BTERR_struct;

	if( drill < 0xff )
	  btreeUnlockPage(set->page, Btree_lockAccess);

	// re-read and re-lock root after determining actual level of root

	if( set->page->lvl != drill) {
		drill = set->page->lvl;

		if( lock != Btree_lockRead && drill == lvl ) {
		  btreeUnlockPage(set->page, mode);
		  continue;
		}
	}

	prevpage_no.bits = set->page_no.bits;
	prevmode = mode;

	//  find key on page at this level
	//  and descend to requested level

	if( !set->page->kill )
	 if( (slotIdx = btreeFindSlot (set->page, key, keyLen)) ) {
	  if( drill == lvl )
		return set->slotIdx = slotIdx, OK;

	  // find next non-dead slot -- the fence key if nothing else

	  while( slotptr(set->page, slotIdx)->dead )
		if( slotIdx++ < set->page->cnt )
		  continue;
		else
  		  return BTERR_struct;

	  // get next page down

	  drill--;
	  ptr = keyptr(set->page, slotIdx);
	  page_no.bits = get64(ptr + keylen(ptr) - sizeof(uint64_t));
	  continue;
	 }

	//  or slide right into next page

	page_no.bits = set->page->right.bits;
  } while( page_no.bits );

  // return error on end of right chain

  return BTERR_struct;
}


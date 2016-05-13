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

// split the root and raise the height of the btree

Status btreeSplitRoot(DbMap *index, BtreeSet *root, DbAddr right, uint8_t *leftKey) {
	BtreeIndex *btree = btreeIndexAddr(index);
	BtreePage *leftPage, *rightPage;
	uint32_t nxt = btree->pageSize;
	uint32_t totLen;
	uint8_t *ptr;
	DbAddr left;

	//  Obtain an empty page to use, and copy the current
	//  root contents into it, e.g. lower keys

	if( (left.bits = btreeNewPage(index, root->page->lvl)) )
		leftPage = getObj(index, left);
	else
		return ERROR_outofmemory;

	memcpy (leftPage, root->page, btree->pageSize);
	rightPage = getObj(index, right);
	rightPage->left.bits = left.bits;

	// preserve the page info at the bottom
	// of higher keys and set rest to zero

	memset(root->page+1, 0, btree->pageSize - sizeof(*root->page));

	// insert stopper key of root page
	// and increase the root height

	nxt -= 2 + sizeof(uint64_t);
	slotptr(root->page, 2)->off = nxt;

	ptr = keyaddr(root->page, nxt);
	ptr[0] = 2 + sizeof(uint64_t);
	ptr[1] = 0;
	store64(ptr + 2, right.bits);

	// insert lower keys page fence key on newroot page as first key

	totLen = keylen(leftKey) + keypre(leftKey);
	nxt -= totLen;

	slotptr(root->page, 1)->off = nxt;
	memcpy (keyaddr(root->page, nxt), leftKey, totLen);
	
	root->page->right.bits = 0;
	root->page->min = nxt;
	root->page->cnt = 2;
	root->page->act = 2;
	root->page->lvl++;

	// release root pages

	btreeUnlockPage(root->page, Btree_lockWrite);
	return OK;
}

//  split already locked full node
//	return unlocked.

Status btreeSplitPage (DbMap *index, BtreeSet *set) {
	BtreeIndex *btree = btreeIndexAddr(index);
	uint32_t cnt = 0, idx = 0, max, nxt, size = btree->pageSize;
	uint8_t leftKey[MAX_key], rightKey[MAX_key];
	BtreeSlotType type = Btree_leafPage;
	BtreePage *frame, *rightPage;
	uint8_t lvl = set->page->lvl;
	DbAddr right, addr;
	uint32_t totLen;
	uint8_t *key;
	Status stat;

	if( !set->page->lvl )
		size <<= btree->leafXtra;
	else
		type = Btree_interior;

	//	get new page and write higher keys to it.

	if( (right.bits = btreeNewPage(index, lvl)) )
		rightPage = getObj(index, right);
	else
		return ERROR_outofmemory;

	max = set->page->cnt;
	cnt = max / 2;
	nxt = size;
	idx = 0;

	while( cnt++ < max ) {
		if( cnt < max || lvl )
		  if( slotptr(set->page, cnt)->dead && cnt < max )
			continue;

		key = keyptr(set->page, cnt);
		totLen = keylen(key) + keypre(key);
		nxt -= totLen;

		memcpy ((uint8_t *)rightPage + nxt, key, totLen);

		//	add librarian slot

		slotptr(rightPage, ++idx)->off = nxt;
		slotptr(rightPage, idx)->type = Btree_librarian;
		slotptr(rightPage, idx)->dead = 1;

		//  add actual slot

		slotptr(rightPage, ++idx)->off = nxt;
		slotptr(rightPage, idx)->type = slotptr(set->page, cnt)->type;

		if( !(slotptr(rightPage, idx)->dead = slotptr(set->page, cnt)->dead) )
			rightPage->act++;
	}

	//	remember right fence key for larger page
	//	extend right leaf fence key with
	//	the right page number.

	if( set->page->lvl)
		memcpy (rightKey, key, totLen);
	else {
		totLen = keylen(key) + sizeof(uint64_t);

		if( totLen < 128 )
			rightKey[0] = totLen;
		else
			rightKey[0] = totLen / 256 | 0x80, rightKey[1] = totLen;

		totLen += keypre(rightKey);
		memcpy (rightKey + keypre(rightKey), key + keypre(key), keylen(key));
	}

	store64(rightKey + totLen - sizeof(uint64_t), right.bits);

	rightPage->min = nxt;
	rightPage->cnt = idx;
	rightPage->lvl = lvl;

	// link right node

	if( set->pageNo.type != Btree_rootPage ) {
		rightPage->right.bits = set->page->right.bits;
		rightPage->left.bits = set->pageNo.bits;

		if( !lvl && rightPage->right.bits ) {
			BtreePage *farRight = getObj(index, rightPage->right);
			btreeLockPage (farRight, Btree_lockLink);
			farRight->left.bits = right.bits;
			btreeUnlockPage (farRight, Btree_lockLink);
		}
	}

	//	update lower keys to continue in old page

	if( (addr.bits = btreeNewPage(index, lvl)) )
		frame = getObj(index, addr);
	else
		return ERROR_outofmemory;

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

	set->page->right.bits = right.bits;
	set->page->min = nxt;
	set->page->cnt = idx;

	//	remember left fence key for smaller page
	//	extend left leaf fence key with
	//	the left page number.

	if( set->page->lvl)
		memcpy (leftKey, key, totLen);
	else {
		totLen = keylen(key) + sizeof(uint64_t);

		if( totLen < 128 )
			leftKey[0] = totLen;
		else
			leftKey[0] = totLen / 256 | 0x80, leftKey[1] = totLen;

		totLen += keypre(leftKey);
		memcpy (leftKey + keypre(leftKey), key + keypre(key), keylen(key));
	}

	store64(leftKey + totLen - sizeof(uint64_t), set->pageNo.bits);

	//  return temporary frame

	addNodeToFrame(index, &btree->freePages[type], NULL, addr);

	// if current page is the root page, split it

	if( set->pageNo.type == Btree_rootPage )
		return btreeSplitRoot (index, set, right, leftKey);

	// insert new fences in their parent pages

	btreeLockPage (rightPage, Btree_lockParent);
	btreeLockPage (set->page, Btree_lockParent);
	btreeUnlockPage (set->page, Btree_lockWrite);

	// insert new fence for reformulated left block of smaller keys

	if( (stat = btreeInsertKey(index, leftKey + keypre(leftKey), keylen(leftKey), lvl+1) ))
		return stat;

	// switch fence for right block of larger keys to new right page

	if( (stat = btreeFixKey(index, rightKey, lvl+1) ))
		return stat;

	btreeUnlockPage (set->page, Btree_lockParent);
	btreeUnlockPage (rightPage, Btree_lockParent);
	return OK;
}

//	check page for space available,
//	clean if necessary and return
//	false - page needs splitting
//	true  - ok to insert

Status btreeCleanPage(DbMap *index, BtreeSet *set, uint32_t totKeyLen) {
	BtreeIndex *btree = btreeIndexAddr(index);
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

	if( (addr.bits = allocObj(index, &btree->freePages[type], NULL, type, size, false)) )
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

//  compare two keys, return > 0, = 0, or < 0
//  =0: all key fields are same
//  -1: key2 > key1
//  +1: key2 < key1

int btreeKeyCmp (uint8_t *key1, uint8_t *key2, uint32_t len2) {
	uint32_t len1 = keylen(key1);
	uint32_t fld1, fld2;
	int ans;

	key1 += keypre(key1);

	//  check key fields

	while( len2 - keypre(key2) > 0 ) {
		if( (fld1 = keylen(key1)) )
			key1 += keypre(key1);
		else
			fld1 = len1 - keypre(key1);

		if( (fld2 = keylen(key2)) )
			key2 += keypre(key2);
		else
			fld2 = len2 - keypre(key2);

		if( ans = memcmp (key1, key2, fld1 > fld2 ? fld2 : fld1) )
			return ans;

		if( fld1 > fld2 )
			return 1;

		if( fld1 < fld2 )
			return -1;

		key1 += fld1;
		key2 += fld2;
		len1 -= keypre(key1) + fld1;
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

Status btreeLoadPage(DbMap *index, BtreeSet *set, uint8_t *key, uint32_t keyLen, uint8_t lvl, BtreeLock lock) {
  BtreeIndex *btree = btreeIndexAddr(index);
  uint8_t drill = 0xff, *ptr;
  BtreePage *prevPage = NULL;
  BtreeLock mode, prevMode;
  DbAddr prevPageNo;
  uint32_t slotIdx;

  set->pageNo.bits = btree->root.bits;
  prevPageNo.bits = 0;

  //  start at our idea of the root level of the btree and drill down

  do {
	// determine lock mode of drill level
	mode = (drill == lvl) ? lock : Btree_lockRead; 
	set->page = getObj(index, set->pageNo);

 	// obtain access lock using lock chaining with Access mode

	if( set->pageNo.type != Btree_rootPage )
	  btreeLockPage(set->page, Btree_lockAccess);

	//	release parent or left sibling page

	if( prevPageNo.bits ) {
	  btreeUnlockPage(prevPage, prevMode);
	  prevPageNo.bits = 0;
	}

 	// obtain mode lock using lock chaining through AccessLock

	btreeLockPage(set->page, mode);

	if( set->page->free )
		return BTERR_struct;

	if( set->pageNo.type != Btree_rootPage )
	  btreeUnlockPage(set->page, Btree_lockAccess);

	// re-read and re-lock root after determining actual level of root

	if( set->page->lvl != drill) {
		drill = set->page->lvl;

		if( lock != Btree_lockRead && drill == lvl ) {
		  btreeUnlockPage(set->page, mode);
		  continue;
		}
	}

	prevPageNo.bits = set->pageNo.bits;
	prevPage = set->page;
	prevMode = mode;

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
	  set->pageNo.bits = get64(ptr + keypre(ptr) + keylen(ptr) - sizeof(uint64_t));
	  continue;
	 }

	//  or slide right into next page

	set->pageNo.bits = set->page->right.bits;
  } while( set->pageNo.bits );

  // return error on end of right chain

  return BTERR_struct;
}


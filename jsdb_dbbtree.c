#include "jsdb.h"
#include "jsdb_db.h"

#define pagebits 13		// btree interior page size
#define leafxtra 0		// btree leaf extra bits

#if (pagebits > Btree_maxbits)
#error btree interior pages too large
#endif

#if (pagebits + leafxtra > Btree_maxbits)
#error btree leaf pages too large
#endif

//	create an empty page

uint64_t btreeNewPage (DbMap *index, uint8_t lvl) {
    BtreeIndex *btree = btreeIndex(index);
	BtreePageType type;
	BtreePage *page;
	uint32_t size;
    DbAddr addr;

	size = btree->pageSize;

	if (lvl)
		type = Btree_interior;
	else {
		type = Btree_leafPage;
		size <<= btree->leafXtra;
	}

	if ((addr.bits = allocObj(index, &btree->freePages[type], type, size, true)))
		page = getObj(index, addr);
	else
		return 0;

	page->lvl = lvl;
	page->min = size;
	return addr.bits;
}

Status btreeInit(DbMap *index) {
	BtreeIndex *btree = btreeIndex(index);
	BtreePage *page;
	BtreeSlot *slot;
	uint8_t *buff;

	btree->pageSize = 1 << pagebits;
	btree->pageBits = pagebits;
	btree->leafXtra = leafxtra;

	//	initial btree root & leaf pages

	if ((btree->leaf.bits = btreeNewPage(index, 0)))
		page = getObj(index, btree->leaf);
	else
		return ERROR_outofmemory;

	//  set up new leaf page with stopper key

	page->min -= 1;
	page->cnt = 1;
	page->act = 1;

	buff = keyaddr(page, page->min);
	buff[0] = 0;

	//  set up stopper slot

	slot = slotptr(page, 1);
	slot->type = Btree_stopper;
	slot->off = page->min;

	if ((btree->root.bits = btreeNewPage(index, 1)))
		page = getObj(index, btree->root);
	else
		return ERROR_outofmemory;

	//  set up new root page with stopper key

	btree->root.type = Btree_rootPage;
	page->min -= 2 + sizeof(uint64_t);
	page->cnt = 1;
	page->act = 1;

	//  set up stopper key

	buff = keyaddr(page, page->min);
	buff[0] = 1 + sizeof(uint64_t);
	buff[1] = 0;
	store64(buff + 2, btree->leaf.bits);

	//  set up slot

	slot = slotptr(page, 1);
	slot->type = Btree_stopper;
	slot->off = page->min;
	return OK;
}

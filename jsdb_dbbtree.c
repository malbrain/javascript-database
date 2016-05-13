#include "jsdb.h"
#include "jsdb_db.h"

#define pagebits 16		// btree interior page size
#define leafxtra 4		// btree leaf extra bits

#if (pagebits > Btree_maxbits)
#error btree interior pages too large
#endif

#if (pagebits + leafxtra > Btree_maxbits)
#error btree leaf pages too large
#endif

//	create an empty page

uint64_t btreeNewPage (DbMap *index, uint8_t lvl, int type) {
    BtreeIndex *btree = btreeIndexAddr(index);
	BtreePage *page;
	uint32_t size;
    DbAddr addr;

	size = btree->pageSize;

	if (!lvl)
		size <<= btree->leafXtra;

	if ((addr.bits = allocObj(index, &btree->freePages[type], NULL, size, type, true)))
		page = getObj(index, addr);
	else
		return 0;

	page->min = size;
	return addr.bits;
}

//	initialize a new btree page

uint64_t btreeInitPage (DbMap *index, uint64_t child, uint8_t lvl, int type) {
	BtreePage *page;
	BtreeSlot *slot;
	uint8_t *buff;
    DbAddr addr;

	//  get new page address

	if ((addr.bits = btreeNewPage(index, lvl, type)))
		page = getObj(index, addr);
	else
		return 0;

	//  set up new page

	page->min -= 2 + sizeof(uint64_t);
	page->cnt = 1;
	page->act = 1;

	//  set up key

	buff = keyaddr(page, page->min);
	buff[0] = 2 + sizeof(uint64_t);
	buff[1] = 0;	// mark last key fld
	store64(buff + 2, child);

	//  set up slot

	slot = slotptr(page, 1);
	slot->type = Btree_stopper;
	slot->off = page->min;

	return addr.bits;
}

Status btreeInit(DbMap *index) {
	BtreeIndex *btree = btreeIndexAddr(index);
	BtreePage *page;
	BtreeSlot *slot;
	uint8_t *buff;

	btree->pageSize = 1 << pagebits;
	btree->pageBits = pagebits;
	btree->leafXtra = leafxtra;

	//	initial btree root & leaf pages

	if ((btree->leaf.bits = btreeNewPage(index, 0, Btree_leafPage)))
		page = getObj(index, btree->leaf);
	else
		return ERROR_outofmemory;

	//  set up new leaf page

	page->min -= 2 + sizeof(KeySuffix);
	page->cnt = 1;
	page->act = 1;

	//  set up key

	buff = keyaddr(page, page->min);
	buff[0] = 2 + sizeof(KeySuffix);
	buff[1] = 0;	// mark last key fld

	//  set up slot

	slot = slotptr(page, 1);
	slot->type = Btree_stopper;
	slot->off = page->min;

	if ((btree->root.bits = btreeNewPage(index, 0, Btree_rootPage)))
		page = getObj(index, btree->root);
	else
		return ERROR_outofmemory;

	//  set up new root page

	page->min -= 2 + sizeof(uint64_t);
	page->cnt = 1;
	page->act = 1;

	//  set up key

	buff = keyaddr(page, page->min);
	buff[0] = 2 + sizeof(uint64_t);
	buff[1] = 0;	// mark last key fld
	store64(buff + 2, btree->leaf.bits);

	//  set up slot

	slot = slotptr(page, 1);
	slot->type = Btree_stopper;
	slot->off = page->min;
	return OK;
}

#include "jsdb.h"
#include "jsdb_db.h"
#include "jsdb_arena.h"
#include "jsdb_docs.h"

DbAddr *fetchSlot (DbMap *map, DocId docId)
{
    if (!docId.index) {
		fprintf (stderr, "Invalid zero document index: %s\n", map->fName);
		exit(1);
	}

	return (DbAddr *)(map->base[docId.segment] + map->arena->segs[docId.segment].segSize) - docId.index;
}

void *findDoc(DbMap *map, DocId docId) {
	DbAddr *addr = fetchSlot(map, docId);
	DbDoc *doc = getObj(map, *addr);
	return doc + 1;
}

//
//  Write Value into collection
//  return new DocId

Status storeVal(DbMap *map, DbAddr docAddr, DocId *docId, uint32_t set) {
    DbAddr *slot, *free, *tail;
    Status error;

    if ((docId->bits = allocDocId(map, set)) )
        slot = fetchSlot(map, *docId);
    else
        return ERROR_outofmemory;

    //  store the address of the new document
    //  and bring the document to life.

    slot->bits = docAddr.bits;
    return OK;
}

//  update document
//  return OK if no error

Status updateDoc(DbMap *map, DbAddr docAddr, DocId docId, uint32_t set) {
    DbAddr *slot, *prev;
    Status error;

    slot = fetchSlot(map, docId);
    lockLatch(slot->latch);

	// TODO: put the old document on waitlist

    prev = fetchSlot(map, docId);

    //  install new document in array and unlock

    slot->bits = docAddr.bits;
    return OK;
}

Status deleteDoc(DbMap *map, DocId docId, uint32_t set) {
    Status error;
    DbAddr *slot;

    slot = fetchSlot(map, docId);
    lockLatch(slot->latch);

	// TODO: put the old document on waitlist

    slot->bits = 0;
    return OK;
}

void *allocateDoc(DbMap *map, uint32_t size, DbAddr *addr, uint32_t set) {
    uint32_t amt = size + sizeof(DbDoc), bits = 3;
    DbAddr *free, *tail;
    Status error;
    DbDoc *doc;

    while ((1UL << bits) < amt)
        bits++;

    free = docStoreAddr(map)->waitLists[set][bits].free;
    tail = docStoreAddr(map)->waitLists[set][bits].tail;

    if ((addr->bits = allocObj(map, free, tail, bits, 1UL << bits)))
        addr->nbits = bits;
	else
        return NULL;

    doc = getObj(map, *addr);
    return doc + 1;
}

DbDoc *findDocVersion(DbMap *map, DocId docId) {
	return NULL;
}

//
// position the start of an iterator
//

void iteratorStart(DbMap *map, Iterator *it, bool fromMin) {
	it->docStore = map;

    if (fromMin) {
        it->docId.bits = 0;
    } else {
        it->docId = map->arena->segs[map->arena->currSeg].nextDoc;
        it->docId.index++;
    }
}

value_t createIterator(DbMap *map, bool fromMin) {
	value_t val;

	val.bits = vt_handle;
	val.h = jsdb_alloc(sizeof(Iterator), true);
	val.aux = hndl_iterator;

	iteratorStart(map, val.h, fromMin);
	return val;
}

//
// increment a segmented DocId
//

bool incrDocId(Iterator *it) {
DocId start = it->docId;

    while (it->docId.segment <= it->docStore->arena->currSeg) {
        if (++it->docId.index <= it->docStore->arena->segs[it->docId.segment].nextDoc.index)
            return true;

        it->docId.index = 0;
        it->docId.segment++;
    }

    it->docId = start;
    return false;
}

//
// decrement a segmented recordId
//

bool decrDocId(Iterator *it) {
DocId start = it->docId;

    while (it->docId.index) {
        if (--it->docId.index)
            return true;
        if (!it->docId.segment)
            break;

        it->docId.segment--;
        it->docId.index = it->docStore->arena->segs[it->docId.segment].nextDoc.index + 1;
    }

    it->docId = start;
    return false;
}

//
//  advance iterator forward
//

void *iteratorNext(Iterator *it, DocId *docId) {
    DbDoc *doc;

    while (incrDocId(it))
        if ((doc = findDocVersion(it->docStore, it->docId)) ) {
			docId->bits = it->docId.bits;
            return doc + 1;
		}

    return NULL;
}

//
//  advance iterator backward
//

void *iteratorPrev(Iterator *it, DocId *docId) {
    DbDoc *doc;

    while (decrDocId(it))
        if ((doc = findDocVersion(it->docStore, it->docId)) ) {
			docId->bits = it->docId.bits;
            return doc + 1;
		}

    return NULL;
}

//
//  set iterator to specific recordId
//  return NULL if it doesn't exist.
//

void *iteratorSeek(Iterator *it, DocId docId) {
	DbDoc *doc;

    if (( doc = findDocVersion(it->docStore, it->docId))) {
        it->docId = docId;
		return doc + 1;
	}

    return NULL;
}

#include "jsdb.h"
#include "jsdb_db.h"
#include "jsdb_dbtxn.h"

value_t createDocStore(value_t name, DbMap *database, uint64_t size, bool onDisk) {
	DbMap *docStore;
	DbAddr child;
	value_t val;

	docStore = openMap(name.str, name.aux, database, sizeof(DbStore), 0, size, onDisk);

	//  open the document indexes

	if (child.bits = docStore->arena->childList.bits) do {
		NameList *entry = getObj(docStore, child);
		DbMap *index = openMap (entry->name, entry->len, docStore, 0, 0, 0, false);
		child.bits = entry->next.bits;
	} while (child.bits);

	//  return the docStore handle

	val.bits = vt_handle;
	val.refcount = true;
	val.aux = hndl_docStore;
	val.h = docStore;

	return val;
}

DbAddr *fetchSlot (DbMap *map, DocId docId)
{
    if (!docId.index) {
		fprintf (stderr, "Invalid zero document index: %s\n", map->fName);
		exit(1);
	}

	return (DbAddr *)(map->base[docId.segment] + map->arena->segs[docId.segment].size) - docId.index;
}

void *findDoc(DbMap *map, DocId docId) {
	DbAddr *addr = fetchSlot(map, docId);
	DbDoc *doc = getObj(map, *addr);
	return doc + 1;
}

//  fill in 64 bit suffix value

void store64(uint8_t *where, uint64_t what) {
	int idx = sizeof(int64_t);

	while (idx--) {
		where[idx] = what & 0xff;
		what >>= 8;
	}
}

//  Write Value into collection
//  return new DocId

Status storeVal(DbMap *map, DbAddr docAddr, DocId *docId, uint32_t set) {
	uint64_t txnId = atomicAdd64(&docStoreAddr(map)->txnId, 1ULL);
	uint8_t keyBuff[MAX_key], suffix[sizeof(KeySuffix)];
	DbDoc *doc = getObj(map, docAddr);
    DbAddr *slot, *free, *tail;
	DbMap *index;
    Status error;

    free = docStoreAddr(map)->waitLists[set][DocIdType].free;
    tail = docStoreAddr(map)->waitLists[set][DocIdType].tail;

    if ((docId->bits = allocDocId(map, free, tail)) )
        slot = fetchSlot(map, *docId);
    else
        return ERROR_outofmemory;

	doc->docTxn.bits = startTxn(map, *docId, DocStore);

	//  index the keys

	store64(suffix, docId->bits);
	store64(suffix + sizeof(uint64_t), -txnId);
	store64(suffix + 2 * sizeof(uint64_t), doc->docTxn.bits);

	if (index = map->child) do {
		int keyLen = makeKey(keyBuff, getObj(map, docAddr), index);

		if (!insertKey (index, keyBuff, keyLen, suffix, set))
			return rollbackTxn(map, doc);

		addTxnStep(map, &doc->docTxn, keyBuff, keyLen, KeyInsert, set);
	} while (index = index->next);

    //  store the address of the new document
    //  and bring the document to life.

    slot->bits = docAddr.bits;
	return commitTxn(map, doc);
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

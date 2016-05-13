#include "jsdb.h"
#include "jsdb_db.h"

value_t createDocStore(value_t name, uint64_t size, bool onDisk) {
	value_t v, val = newObject(), n;
	DbMap *docStore, *index;
	NameList *entry;
	DbAddr child;
	int idx;

	docStore = createMap(name, NULL, sizeof(DbStore), size, onDisk);
	docStore->arena->type = hndl_docStore;

	v.bits = vt_handle;
	v.aux = hndl_docStore;
	v.hndl = docStore;
	v.refcount = 1;

	n.bits = vt_string;
	n.string = "_docStore";
	n.aux = 9;

	incrRefCnt(v);
	*lookup(val.oval, n, true) = v;

	//  enumerate and open the document indexes

	readLock(docStore->arena->childLock);

	if ((child.bits = docStore->arena->childList.bits)) do {
		entry = getObj(docStore, child);
		name.bits = vt_string;
		name.string = entry->name;
		name.aux = strlen(entry->name);
		DbMap *index = openMap(name, docStore);

		if (!index)
			continue;

		waitNonZero(&index->arena->type);
		v.bits = vt_handle;
		v.aux = index->arena->type;
		v.hndl = index;
		v.refcount = 1;
		incrRefCnt(v);

		name.string = entry->name;
		name.aux = strlen(entry->name);
		*lookup(val.oval, name, true) = v;
	} while ((child.bits = entry->next.bits));

	rwUnlock(docStore->arena->childLock);
	return val;
}

DbAddr *fetchSlot (DbMap *map, DocId docId)
{
	if (!docId.index) {
		fprintf (stderr, "Invalid zero document index: %s\n", map->name.str);
		exit(1);
	}

	return (DbAddr *)(map->base[docId.segment] + map->arena->segs[docId.segment].size) - docId.index;
}

void *findDoc(DbMap *map, DocId docId) {
	DbAddr *addr = fetchSlot(map, docId);
	DbDoc *doc = getObj(map, *addr);
	return doc + 1;
}

//	get 64 bit suffix value

uint64_t get64(uint8_t *from) {
	uint64_t result = 0;

	for (int idx = 0; idx < sizeof(uint64_t); idx++) {
		result <<= 8;
		result |= from[idx];
	}
	return result;
}

//  fill in 64 bit suffix value

void store64(uint8_t *where, uint64_t what) {
	int idx = sizeof(uint64_t);

	while (idx--) {
		where[idx] = what & 0xff;
		what >>= 8;
	}
}

//  Put initial Document version into collection
//	create document keys
//  return new DocId

Status storeVal(object_t *docStore, DbAddr docAddr, DocId *docId, uint32_t set) {
	DbAddr *slot, *free, *tail;
	Status error;
	DbMap *map;
	DbDoc *doc;

	//	grab the index array read lock

	readLock(docStore->lock);
	map = docStore->pairs[0].value.hndl;

	free = docStoreAddr(map)->waitLists[set][DocIdType].free;
	tail = docStoreAddr(map)->waitLists[set][DocIdType].tail;

	if ((docId->bits = allocDocId(map, free, tail)) )
		slot = fetchSlot(map, *docId);
	else
		return ERROR_outofmemory;

	//	index the keys

	for (uint32_t idx = 1; idx < vec_count(docStore->pairs); idx++) {
		DbMap *index = docStore->pairs[idx].value.hndl;
		Status stat;

		if (index->arena->drop)
			continue;

		switch(index->arena->type) {
			case hndl_artIndex:
				stat = artIndexKey (map, index, docAddr, *docId, set, 0);
				break;
			case hndl_btreeIndex:
				stat = btreeIndexKey (map, index, docAddr, *docId, 0);
				break;
			case hndl_colIndex:
				stat = OK;
				break;
			default:
				fprintf(stderr, "unknown index type %d\n", index->arena->type);
				exit(1);
		}

		if(stat == OK)
			continue;

//		rollbackTxn(docStore, docAddr);
		rwUnlock(docStore->lock);
		return stat;
	}

	//  store the address of the new document
	//  and bring the document slot to life.

	slot->bits = docAddr.bits;

	//	release the docStore/index handle array lock

	rwUnlock(docStore->lock);
	return OK;
}

//  update document
//  return OK if no error

Status updateDoc(object_t *docStore, DbAddr docAddr, DocId docId, uint32_t set) {
	DbAddr *slot, *prev;
	Status error;
	DbMap *map;

	map = docStore->pairs[0].value.hndl;

	slot = fetchSlot(map, docId);
	lockLatch(slot->latch);

	// TODO: put the old document on waitlist

	prev = fetchSlot(map, docId);

	//  install new document in array and unlock

	slot->bits = docAddr.bits;
	return OK;
}

Status deleteDoc(object_t *docStore, DocId docId, uint32_t set) {
	Status error;
	DbAddr *slot;
	DbMap *map;

	map = docStore->pairs[0].value.hndl;

	slot = fetchSlot(map, docId);
	lockLatch(slot->latch);

	// TODO: put the old document on waitlist
	//   and delete the keys

	slot->bits = 0;
	return OK;
}

void *allocateDoc(DbMap *map, uint32_t size, DbAddr *addr, uint32_t set) {
	uint32_t amt = size + sizeof(DbDoc), bits = MinDocType;
	DbAddr *free, *tail;
	Status error;
	DbDoc *doc;

	while ((1UL << bits) < amt)
		bits++;

	free = docStoreAddr(map)->waitLists[set][bits].free;
	tail = docStoreAddr(map)->waitLists[set][bits].tail;

	if ((addr->bits = allocObj(map, free, tail, bits, 1UL << bits, false)))
		doc = getObj(map, *addr);
	else
		return NULL;

	memset(doc, 0, sizeof(DbDoc));
	return doc + 1;
}

//  TODO:  find appropriate prior version

DbDoc *findDocVersion(DbMap *map, DocId docId) {
	DbAddr *addr = fetchSlot(map, docId);

	if (addr->bits)
		return getObj(map, *addr);

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
	val.hndl = jsdb_alloc(sizeof(Iterator), true);
	val.aux = hndl_iterator;
	val.refcount = 1;

	iteratorStart(map, val.hndl, fromMin);
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

	docId->bits = 0;
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

	docId->bits = 0;
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

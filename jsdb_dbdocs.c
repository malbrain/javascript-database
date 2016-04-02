#include "jsdb.h"
#include "jsdb_db.h"
#include "jsdb_dbtxn.h"

value_t createDocStore(value_t name, DbMap *database, uint64_t size, bool onDisk) {
	value_t v, val = newArray();
	NameList *entry;
	DbMap *docStore;
	DbAddr child;
	int idx;

	docStore = createMap(name, database, sizeof(DbStore), 0, size, onDisk);

	if (docStore->created)
		docStore->arena->type = hndl_docStore;

	v.bits = vt_handle;
	v.aux = hndl_docStore;
	v.hndl = docStore;
	v.refcount = 1;
	incrRefCnt(v);
	vec_push(val.aval->array, v);

	//  open the document indexes

	readLock(docStore->arena->childLock);

	if ((idx = docStore->arena->childCnt))
		vec_add(val.aval->array, idx);

	if (child.bits = docStore->arena->childList.bits) do {
		entry = getObj(docStore, child);
		value_t name;

		name.str = entry->name;
		name.aux = entry->len;
		DbMap *index = openMap (name, docStore);

		//  return the docStore handle

		v.bits = vt_handle;
		v.aux = index->arena->type;
		v.hndl = index;
		v.refcount = 1;
		incrRefCnt(v);

		// index zero is the docStore

		val.aval->array[idx--] = v;
	} while ((child.bits = entry->next.bits));

	rwUnlock(docStore->arena->childLock);
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

//  Write Value into collection
//	create document keys
//  return new DocId

Status storeVal(array_t *docStore, DbAddr docAddr, DocId *docId, uint32_t set) {
	uint8_t keyBuff[MAX_key], suffix[sizeof(SuffixBytes)];
	DbAddr *slot, *free, *tail;
	uint64_t txnId;
	Status error;
	DbMap *map;
	DbDoc *doc;
	int keyLen;

	readLock(docStore->lock);
	map = docStore->array[0].hndl;

	doc = getObj(map, docAddr);
	txnId = atomicAdd64(&docStoreAddr(map)->txnId, 1ULL);
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

	for (uint32_t idx = 1; idx < vec_count(docStore); idx++) {
		DbMap *index = docStore->array[idx].hndl;

		if (!index->arena->drop)
			keyLen = makeKey(keyBuff, getObj(map, docAddr), index);
		else
			continue;

		if (!indexKey (index, keyBuff, keyLen, suffix, set)) {
			rollbackTxn(docStore, doc);
			rwUnlock(docStore->lock);
			return ERROR_duplicatekey;
		}

		addTxnStep(docStore, idx, &doc->docTxn, keyBuff, keyLen, KeyInsert, set);
	}

	//  store the address of the new document
	//  and bring it to life.

	slot->bits = docAddr.bits;
	commitTxn(map, doc);
	rwUnlock(docStore->lock);
	return OK;
}

//  update document
//  return OK if no error

Status updateDoc(array_t *docStore, DbAddr docAddr, DocId docId, uint32_t set) {
	DbAddr *slot, *prev;
	Status error;
	DbMap *map;

	map = docStore->array[0].hndl;

	slot = fetchSlot(map, docId);
	lockLatch(slot->latch);

	// TODO: put the old document on waitlist

	prev = fetchSlot(map, docId);

	//  install new document in array and unlock

	slot->bits = docAddr.bits;
	return OK;
}

Status deleteDoc(array_t *docStore, DocId docId, uint32_t set) {
	Status error;
	DbAddr *slot;
	DbMap *map;

	map = docStore->array[0].hndl;

	slot = fetchSlot(map, docId);
	lockLatch(slot->latch);

	// TODO: put the old document on waitlist
	//   and delete the keys

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

	if ((addr->bits = allocObj(map, free, tail, bits, 1UL << bits, false)))
		doc = getObj(map, *addr);
	else
		return NULL;

	memset(doc, 0, sizeof(DbDoc));
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

#include "jsdb.h"
#include "jsdb_db.h"

value_t createDocStore(DbMap *map, value_t name, uint64_t size, bool onDisk, bool *created) {
	DbMap *docStore;
	Handle *hndl;
	value_t val;

	writeLock(map->arena->childLock);
	val = openMap(name, map, sizeof(DbStore), size, onDisk);
	rwUnlock(map->arena->childLock);

	val.subType = Hndl_docStore;
	hndl = val.handle;

	docStore = hndl->object;
	docStore->arena->idSize = sizeof(DbAddr);
	docStore->arena->type = Hndl_docStore;

	*created = docStore->created;
	return val;
}

//	return pointer to Id slot

void *fetchIdSlot (DbMap *map, DocId docId) {
	if (!docId.index) {
		RedBlack *entry = map->entry;
		fprintf (stderr, "Invalid zero document index: %*s\n", entry->keyLen, entry->key);
		exit(1);
	}

	return map->base[docId.segment] + map->arena->segs[docId.segment].size - docId.index * map->arena->idSize;
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

//	callback to construct and add key to an index

typedef struct {
	DocId docId;
	DbDoc *doc;
	Txn *txn;
} Params;

Status indexDoc(DbMap *docStore, RedBlack *entry, void *params) {
	Params *p = params;
	value_t val, name;
	Handle *handle;
	DbMap *index;

	name.bits = vt_string;
	name.aux = entry->keyLen;
	name.str = (uint8_t *)entry->key;

	val = openMap(name, docStore, 0, 0, false);

	if ((handle = val.handle))
		index = handle->object;
	else
		return OK;

	if (index->arena->drop)
		return OK;

	switch(index->arena->type) {
	case Hndl_artIndex:
		return artIndexKey (index, p->doc, p->docId, p->txn->set);
	case Hndl_btreeIndex:
		return btreeIndexKey (index, p->doc, p->docId);
	case Hndl_colIndex:
		return OK;
	}

	fprintf(stderr, "unknown index type %d\n", index->arena->type);
	exit(1);
}

//  Put initial Document version into collection
//	create document keys
//  return new DocId

Status storeVal(DbMap *docStore, DbAddr docAddr, DocId *docId, DocId txnId) {
	DbDoc *doc = getObj(docStore, docAddr);
	DbAddr *slot, *free, *tail;
	DbMap *db = docStore->parent;
	Params params[1];
	Status stat;
	Txn *txn;

	//	grab the child index list read lock

	txn = fetchIdSlot(db, txnId);

	free = docStoreAddr(docStore)->docWait[txn->set][DocIdType].free;
	tail = docStoreAddr(docStore)->docWait[txn->set][DocIdType].tail;

	if ((docId->bits = allocDocId(docStore, free, tail)) )
		slot = fetchIdSlot(docStore, *docId);
	else
		return ERROR_outofmemory;

	//	enumerate and add the keys

	params->docId.bits = docId->bits;
	params->doc = doc;
	params->txn = txn;

	readLock(docStore->arena->childLock);
	rbList(docStore, docStore->arena->childRoot, indexDoc, NULL, 0, params);
	rwUnlock(docStore->arena->childLock);

	//  store the address of the new document
	//  and bring the document slot to life.

	slot->bits = docAddr.bits;

	stat = txnStep(docStore, txnId, *docId, Txn_add);
	return OK;
}

//  update document
//  return OK if no error

Status updateDoc(DbMap *docStore, DbAddr docAddr, DocId docId, DocId txnId) {
	DbDoc *doc = getObj(docStore, docAddr);
	DbMap *db = docStore->parent;
	DbAddr *slot, *prev;
	DbDoc *prevDoc;
	Status stat;
	Txn *txn;

	txn = fetchIdSlot(db, txnId);

	slot = fetchIdSlot(docStore, docId);
	lockLatch(slot->latch);

	// TODO: put the old document on waitlist

	prev = fetchIdSlot(docStore, docId);
	prevDoc = getObj(docStore, *prev);

	//  install new document in array and unlock

	slot->bits = docAddr.bits;
	stat = txnStep(docStore, txnId, docId, Txn_upd);
	return OK;
}

Status deleteDoc(DbMap *docStore, uint64_t docBits, uint64_t txnBits) {
	DocId docId, txnId;
	DbAddr *slot;
	DbDoc *doc;

	docId.bits = docBits;
	txnId.bits = txnBits;

	slot = fetchIdSlot(docStore, docId);
	lockLatch(slot->latch);

	doc = getObj(docStore, *slot);

	// TODO: put the old document on waitlist
	//   and delete the keys

	slot->bits = 0;
	return OK;
}

void *allocateDoc(DbMap *docStore, uint32_t size, DbAddr *addr, uint32_t set) {
	uint32_t amt = size + sizeof(DbDoc), bits = MinDocType;
	DbAddr *free, *tail, slot;
	DbDoc *doc;

	while ((1UL << bits) < amt)
		bits++;

	free = docStoreAddr(docStore)->docWait[set][bits].free;
	tail = docStoreAddr(docStore)->docWait[set][bits].tail;

	lockLatch(free->latch);
	slot.bits = bits;

	while (!(slot.addr = getNodeFromFrame(docStore, free))) {
	  if (!getNodeWait(docStore, free, tail))
		if (!initObjFrame(docStore, free, bits, 1UL << bits)) {
			unlockLatch(free->latch);
			return 0;
		}
	}

	addr->bits = slot.bits;
	unlockLatch(free->latch);
	doc = getObj(docStore, slot);

	memset(doc, 0, sizeof(DbDoc));
	doc->docSize = size;
	return doc + 1;
}

//  find appropriate document version per txn & timestamp

uint64_t findDocVer(DbMap *docStore, DocId docId, DocId txnId, uint64_t ts) {
	DbAddr *addr = fetchIdSlot(docStore, docId);
	DbMap *db = docStore->parent;
	uint64_t txnTs;

	//	examine all prior versions

	while (addr->bits) {
		DbDoc *doc = getObj(docStore, *addr);

		// is version in same txn?

		if (doc->txnId.bits == txnId.bits)
			return addr->bits;

		// is version committed?

		if (doc->docTs)
		  if (doc->docTs < ts)
			return addr->bits;

		// is txn committed?

		if (txnId.bits) {
			Txn *txn = fetchIdSlot(db, txnId);

			if (isCommitted(txn->ts) && ts >= txn->ts)
				return addr->bits;

            while (isReader((txnTs = txn->ts)) && txnTs < ts)
                compareAndSwap(&txn->ts, txnTs, ts);
		}

		addr = doc->oldDoc;
	}

	return 0;
}

//
// create and position the start of an iterator
//

value_t createIterator(value_t docStore, DbMap *map, bool fromMin) {
	Iterator *it = jsdb_alloc(sizeof(Iterator), true);
	value_t val;

	val.bits = vt_handle;
	val.subType = Hndl_iterator;
	val.refcount = 1;
	val.handle = it;
	incrRefCnt(val);

	it->pqAddr.bits = addPQEntry(map, getSet(map), en_reader);
	it->timestamp = getTimestamp(map, it->pqAddr);
	it->docStore = docStore;

	if (fromMin) {
		it->docId.bits = 0;
	} else {
		it->docId = map->arena->segs[map->arena->currSeg].nextDoc;
		it->docId.index++;
	}

	return val;
}

void destroyIterator(Iterator *it, DbMap *docStore) {
	removePQEntry (docStore, it->pqAddr);

	jsdb_free(it);
}

//
// increment a segmented DocId
//

bool incrDocId(Iterator *it, DbMap *docStore) {
DocId start = it->docId;

	while (it->docId.segment <= docStore->arena->currSeg) {
		if (++it->docId.index <= docStore->arena->segs[it->docId.segment].nextDoc.index)
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

bool decrDocId(Iterator *it, DbMap *docStore) {
DocId start = it->docId;

	while (it->docId.index) {
		if (--it->docId.index)
			return true;
		if (!it->docId.segment)
			break;

		it->docId.segment--;
		it->docId.index = docStore->arena->segs[it->docId.segment].nextDoc.index + 1;
	}

	it->docId = start;
	return false;
}

//
//  advance iterator forward
//

//  TODO:  lock the record

value_t iteratorNext(Iterator *it, DbMap *docStore) {
	DbAddr slot;
	DbDoc *doc;
	value_t v;

	while (incrDocId(it, docStore))
		if ((slot.bits = findDocVer(docStore, it->docId, it->txnId, it->timestamp)) ) {
			v.bits = vt_document;
			doc = getObj(docStore, slot);
			v.document = (document_t *)(doc + 1);
			return v;
		}

	v.bits = vt_null;
	return v;
}

//
//  advance iterator backward
//

//  TODO:  lock the record

value_t iteratorPrev(Iterator *it, DbMap *docStore) {
	DbAddr slot;
	DbDoc *doc;
	value_t v;

	while (decrDocId(it, docStore))
		if ((slot.bits = findDocVer(docStore, it->docId, it->txnId, it->timestamp)) ) {
			v.bits = vt_document;
			doc = getObj(docStore, slot);
			v.document = (document_t *)(doc + 1);
			return v;
		}

	v.bits = vt_null;
	return v;
}

//
//  set iterator to specific recordId
//

//  TODO:  lock the record

value_t iteratorSeek(Iterator *it, DbMap *docStore, uint64_t docBits) {
	DocId docId;
	DbAddr slot;
	DbDoc *doc;
	value_t v;

	docId.bits = docBits;

	if ((slot.bits = findDocVer(docStore, docId, it->txnId, it->timestamp))) {
		v.bits = vt_document;
		doc = getObj(docStore, slot);
		v.document = (document_t *)(doc + 1);
		it->docId.bits = docId.bits;
		return v;
	}

	v.bits = vt_null;
	return v;
}

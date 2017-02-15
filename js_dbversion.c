#include "js.h"
#include "js_db.h"
#include "js_dbindex.h"

#ifdef __linux__
#define offsetof(type,member) __builtin_offsetof(type,member)
#endif

//	insert a document into a docStore

uint64_t insertDoc(Handle **idxHndls, value_t val, uint64_t prevAddr, ObjId docId, ObjId txnId, Ver *prevVer) {
	uint32_t docSize = calcSize(val, true), rawSize;
	DocArena *docArena = docarena(idxHndls[0]->map);
	DbAddr addr, *slot, keys[1];
	dbaddr_t dbAddr;
	Ver *ver;
	Doc *doc;

	// build object of index keys for this document

	keys->bits = 0;

	if (val.type == vt_object)
	  for (int idx = 1; idx < vec_cnt(idxHndls); idx++)
		buildKeys(idxHndls[0], idxHndls[idx], val, keys, docId, prevVer, vec_cnt(idxHndls));

    if ((addr.bits = dbAllocDocStore(idxHndls[0], docSize + sizeof(Doc) + sizeof(Ver), false)))
        doc = getObj(idxHndls[0]->map, addr);
    else
        return 0;

	rawSize = db_rawSize(addr.bits);

    memset (doc, 0, sizeof(Doc));
    doc->lastVer = rawSize - sizeof(Ver) - offsetof(Ver, txnId) - docSize;
    doc->prevAddr.bits = prevAddr;
    doc->docAddr.bits = addr.bits;
	doc->docId.bits = docId.bits;

	//	fill-in stopper for version array

	ver = (Ver *)((uint8_t *)doc + rawSize - offsetof(Ver, txnId));
    ver->offset = rawSize - offsetof(Ver, txnId);
    ver->verSize = 0;

	//	fill-in new version

	ver = (Ver *)((uint8_t *)doc + doc->lastVer);
    ver->verSize = sizeof(Ver) + docSize;
	ver->keys->bits = keys->bits;
    ver->offset = doc->lastVer;

	if (txnId.bits)
		ver->txnId.bits = txnId.bits;
	else
		ver->timestamp = allocateTimestamp(idxHndls[0]->map->db, en_writer);

	dbAddr.addr = addr.addr;
	dbAddr.storeId = docArena->storeId;

	marshalDoc(val, (uint8_t*)doc, doc->lastVer + sizeof(Ver), dbAddr, docSize, ver->rec, true);

	//	install the document
	//	and return docId

	//  TODO: resolve timestamps re: new reader > ourTs

	slot = fetchIdSlot(idxHndls[0]->map, docId);
	slot->bits = addr.bits;
	return docId.bits;
}

uint64_t updateDoc(Handle **idxHndls, document_t *document, ObjId txnId) {
	uint32_t docSize, totSize, offset;
	Ver *prevVer, *newVer;
	DocArena *docArena;
	dbaddr_t dbAddr;
	DbAddr keys[1];
	Doc *doc;

	keys->bits = 0;

	docArena = docarena(idxHndls[0]->map);

    doc = (Doc *)((uint8_t *)document->ver - document->ver->offset);
	prevVer = document->ver;

	docSize = calcSize(*document->update, false);
	totSize = docSize + sizeof(Ver);

	//	start over if not enough room for the version in the set

	if (totSize > doc->lastVer - sizeof(Doc))
		return insertDoc(idxHndls, *document->update, doc->docAddr.bits, doc->docId, txnId, prevVer);

	if (document->update->type == vt_object)
	  for (int idx = 1; idx < vec_cnt(idxHndls); idx++)
		buildKeys(idxHndls[0], idxHndls[idx], *document->update, keys, doc->docId, prevVer, vec_cnt(idxHndls));

	docSize = calcSize(*document->update, true);
	totSize = docSize + sizeof(Ver);
	offset = doc->lastVer - totSize;

	newVer = (Ver *)((uint8_t *)doc + offset);
	newVer->txnId.bits = txnId.bits;
	newVer->keys->bits = keys->bits;
    newVer->verSize = totSize;
    newVer->offset = offset;

	//	build and install the update

	dbAddr.addr = doc->docAddr.addr;
	dbAddr.storeId = docArena->storeId;

	marshalDoc(*document->update, (uint8_t*)doc, offset + sizeof(Ver), dbAddr, docSize, newVer->rec, false);

    doc->lastVer = offset;
	return doc->docId.bits;
}

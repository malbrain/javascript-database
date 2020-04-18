#include "js.h"
#include "js_db.h"
#include "js_dbindex.h"

#include <stddef.h>

//	insert a document or an updated version into a docStore
//	returns prev document addr from the slot

JsStatus writeRawDoc(Handle *docHndl, value_t val, ObjId *docId) {
    DbMap *map = MapAddr(docHndl);
    uint32_t docSize, rawSize, baseOff;
    DbAddr newAddr, prevAddr;
    document_t *rawDoc;
    JsDoc *jsDoc;             // follows document
    DbAddr *docSlot;
    value_t s;

    s.bits = vt_status;

    docSize = calcSize(val, true);
    docSlot = fetchIdSlot(map, *docId);
    prevAddr = *docSlot;

    DocIdXtra(docId)->txnAccess = TxnRaw;

    rawSize = docSize + sizeof(JsDoc) + sizeof(struct Document);

    //	allocate space in docStore for the document

    if ((newAddr.bits = allocDocStore(docHndl, rawSize, false)))
        rawSize = db_rawSize(newAddr);
    else
        return (JsStatus)ERROR_outofmemory;

    //	set up the document header

    rawDoc = getObj(map, newAddr);
    memset(rawDoc, 0, sizeof(struct Document));

    rawDoc->ourAddr.bits = newAddr.bits;
    rawDoc->docId.bits = docId->bits;
    rawDoc->docType = VerRaw;

    jsDoc = (JsDoc *)(rawDoc + 1);
    jsDoc->maxOffset = rawSize;

    baseOff = sizeof(struct Document) + sizeof(JsDoc);

    // marshal directly into the mmap file

    marshalDoc(val, rawDoc->base, baseOff, docSize, jsDoc->value, true);

	//	install the document in the slot
	//	and return old addr

	docSlot->bits = newAddr.bits;
	return (JsStatus)prevAddr.bits;
}

//  write/update mvcc doc

JsStatus writeMVCCDoc(Handle *docHndl, value_t val, ObjId *docId) {
  DbMap *map = MapAddr(docHndl);
  uint32_t docSize, baseOff;
  MVCCResult result;
  DbAddr *docSlot;
  JsDoc *jsDoc;
  value_t s;
  Doc *doc;         // follows Document
  Ver *ver;

  s.bits = vt_status;

  docSize = calcSize(val, true);
  docSlot = fetchIdSlot(map, *docId);

  result = mvcc_installNewDocVer(docHndl, sizeof(JsDoc) + docSize, docId);
  doc = result.object;
  ver = (Ver *)(doc->doc->base + doc->newestVer);
  jsDoc = (JsDoc *)(ver + 1);
  baseOff = doc->newestVer - sizeof(Ver) - sizeof(JsDoc) - docSize;
  
  // marshal directly into the mmap file

  marshalDoc(val, doc->doc->base, baseOff, docSize, jsDoc->value, true);
  return (JsStatus)DB_OK;
}

JsStatus writeDoc(Handle *docHndl, value_t val, ObjId *docId) {
  struct Document *prevDoc;
  DocStore *docStore;
  JsStatus stat = (JsStatus)DB_OK;
  DbMap *docMap;

  docMap = MapAddr(docHndl);
  docStore = (DocStore *)(docMap->arena + 1);

  if(docId->bits == 0)
      docId->bits = allocObjId(docMap, listFree(docHndl, 0), listWait(docHndl, 0));

  switch (docStore->docType) {
    case VerRaw:
        DocIdXtra(docId)->txnAccess = TxnRaw;
        prevDoc = writeRawDoc(docHndl, val, docId);

      if (jsError(prevDoc))
        stat = (JsStatus)prevDoc;

      break;

    case VerMvcc:
      DocIdXtra(docId)->txnAccess = TxnWrt;
      prevDoc = writeMVCCDoc(docHndl, val, docId);

      if (jsError(prevDoc))
        stat = (JsStatus)prevDoc;

        break;
    }

  return stat;
}

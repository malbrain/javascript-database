#include "js.h"
#include "js_db.h"
#include "js_dbindex.h"

#include <stddef.h>

//	insert a document or an updated version into a docStore
//	if update, call with docId slot locked.
//	returns prev document addr from the slot

JsStatus writeRawDoc(Handle *docHndl, value_t val, ObjId *docId) {
  DbMap *map = MapAddr(docHndl);
  uint32_t docSize, rawSize;
  DbAddr newAddr, prevAddr;
  document_t *document;
  JsDoc *jsDoc;             // follows document
  DbAddr *docSlot;
  value_t s;

  s.bits = vt_status;

	//	assign a new docId slot?

	if (docId->bits) {
		prevAddr = *(DbAddr *)fetchIdSlot(map, *docId);

    } else {
        prevAddr.bits = 0;
        docId->bits =
              allocObjId(map, listFree(docHndl, 0), listWait(docHndl, 0));
        }

    DocIdXtra(docId)->txnAccess = TxnRaw;
	docSize = calcSize(val, true);
	docSlot = fetchIdSlot(map, *docId);

    rawSize = docSize + sizeof(JsDoc) + sizeof(struct Document);

	//	allocate space in docStore for the document

    if ((newAddr.bits = allocDocStore(docHndl, rawSize, false)))
		rawSize = db_rawSize(newAddr);
    else
        return (JsStatus)ERROR_outofmemory;

	//	set up the document header

    document = getObj(map, newAddr);
    memset (document, 0, sizeof(struct Document));

	document->ourAddr.bits = newAddr.bits;
	document->docId.bits = docId->bits;
    document->docMin = sizeof(document_t);
    document->docType = VerRaw;

    jsDoc = docAddr(document);
    jsDoc->maxOffset = document->docMin + sizeof(JsDoc) + docSize;

    // marshal directly into the mmap file

    marshalDoc(val, document->base, document->docMin + sizeof(JsDoc), docSize, jsDoc->value, true);

	//	install the document in the slot
	//	and return old addr

	docSlot->bits = newAddr.bits;
	return (JsStatus)prevAddr.bits;
}

//  write/update mvcc doc

JsStatus writeMVCCDoc(Handle *docHndl, value_t val, ObjId *docId) {
  DbMap *map = MapAddr(docHndl);
  uint32_t docSize, base;
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
  ver = (Ver *)(doc->doc->base + doc->pendingVer);
  jsDoc = (JsDoc *)(ver + 1);
  base = doc->pendingVer + sizeof(Ver) + sizeof(JsDoc);
  
  // marshal directly into the mmap file

  marshalDoc(val, doc->doc->base, base, docSize, jsDoc->value, true);
  return (JsStatus)DB_OK;
}

JsStatus writeDoc(Handle *docHndl, value_t val, ObjId *docId) {
  DocStore *docStore;
  JsStatus stat = (JsStatus)DB_OK;
  DbMap *docMap;

  docMap = MapAddr(docHndl);
  docStore = (DocStore *)(docMap->arena + 1);

  switch (docStore->docType) {
    case VerRaw: {
      struct Document *prevDoc = writeRawDoc(docHndl, val, docId);

      if (jsError(prevDoc))
        stat = (JsStatus)prevDoc;

      break;
    }

    case VerMvcc: {
      struct Document *prevDoc = writeMVCCDoc(docHndl, val, docId);

      if (jsError(prevDoc))
        stat = (JsStatus)prevDoc;

        break;
      }
    }

  return stat;
}

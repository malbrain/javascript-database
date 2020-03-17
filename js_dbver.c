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
    document->docType = DocRaw;

    JsDoc *doc = docAddr(document);
    doc->maxOffset = document->docMin + sizeof(JsDoc) + docSize;

    // marshal directly into the mmap file

    marshalDoc(val, document->base, document->docMin + sizeof(JsDoc), docSize, doc->value, true);

	//	install the document in the slot
	//	and return old addr

	docSlot->bits = newAddr.bits;
	return (JsStatus)prevAddr.bits;
}

//  write/update mvcc doc

JsStatus writeMVCCDoc(DbHandle hndl[1], value_t val, ObjId *docId,
                      ObjId txnId) {
  Handle *docHndl = bindHandle(hndl, Hndl_docStore);
  DbMap *map = MapAddr(docHndl);
  uint32_t docSize;
  MVCCResult result;
  DbAddr *docSlot;
  value_t s;
  JsDoc *jsDoc;
  Doc *doc;

  s.bits = vt_status;

  docSize = calcSize(val, true);
  docSlot = fetchIdSlot(map, *docId);

  result = mvcc_installNewDocVer(hndl, sizeof(JsDoc) + docSize, docId, txnId);
  doc = result.object;
  jsDoc = docAddr(doc->doc);

  // marshal directly into the mmap file

  marshalDoc(val, doc->doc->base, doc->doc->docMin + sizeof(JsDoc),
             docSize, jsDoc->value, true);

  return (JsStatus)DB_OK;
}

JsStatus writeDoc(value_t hndl, value_t val, ObjId *docId,
                      ObjId txnId) {
  DocStore *docStore;
  JsStatus stat = (JsStatus)DB_OK;
  Handle *docHndl;
  DbMap *docMap;

  hndl = js_handle(hndl, Hndl_docStore);

  if (hndl.type == vt_hndl)
    if (!(docHndl = bindHandle(hndl.hndl, Hndl_docStore)))
      return (JsStatus)DB_ERROR_handleclosed;
    else
      docMap = MapAddr(docHndl);
  else
    return badHandle(hndl);

  docStore = (DocStore *)(docMap->arena + 1);

  switch (docStore->docType) {
    case DocRaw: {
      struct Document *prevDoc = writeRawDoc(docHndl, val, docId);

      if (jsError(prevDoc))
        stat = (JsStatus)prevDoc;

      break;
    }

    case DocMvcc: {
      struct Document *prevDoc = writeMVCCDoc(hndl.hndl, val, docId, txnId);

      if (jsError(prevDoc))
        stat = (JsStatus)prevDoc;

        break;
      }
    }
  releaseHandle(docHndl);
  return stat;
}

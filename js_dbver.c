#include "js.h"
#include "js_db.h"
#include "js_dbindex.h"

#include <stddef.h>

//	insert a document or an updated version into a docStore
//	if update, call with docId slot locked.
//	returns prev document addr

JsStatus appendDoc(Handle *docHndl, value_t val, ObjId *docId) {
  DbMap *map = MapAddr(docHndl);
  uint32_t docSize, rawSize;
  DbAddr docAddr, *prevAddr;
  document_t *prevDoc = NULL;
  document_t *document;
  DbAddr *docSlot;
  JsStatus stat;
  uint32_t idx;

	//	assign a new docId slot

	if (docId->bits) {
		prevAddr = fetchIdSlot(map, *docId);
        prevDoc = getObj(map, *prevAddr);
	} else
        docId->bits = allocObjId(map, listFree(docHndl, 0), listWait(docHndl, 0));

	docSize = calcSize(val, true, prevDoc ? prevDoc->base : NULL);
	docSlot = fetchIdSlot(map, *docId);

    rawSize = docSize + sizeof(document_t);

	if (rawSize < 12 * 1024 * 1024)
		rawSize += rawSize / 2;

	//	allocate space in docStore for the document

    if ((docAddr.bits = allocDocStore(docHndl, rawSize, false)))
		rawSize = db_rawSize(docAddr);
    else
        return (JsStatus)ERROR_outofmemory;

	//	set up the document header

    document = getObj(map, docAddr);
    memset (document, 0, sizeof(document_t));

	document->ourAddr.bits = docAddr.bits;
	document->docId.bits = docId->bits;
    document->docLen = docSize;

	marshalDoc(val, document->base, 0, docSize, document->value, true, prevDoc ? prevDoc->base : NULL);

	//	install the document
	//	and return old addr

	docSlot->bits = docAddr.bits;
	return prevDoc;
}


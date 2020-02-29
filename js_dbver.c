#include "js.h"
#include "js_db.h"
#include "js_dbindex.h"

#include <stddef.h>

//	insert a document or an updated version into a docStore
//	if update, call with docId slot locked.
//	returns prev document addr

//  note: not for mvcc documents

JsStatus appendDoc(Handle *docHndl, value_t val, ObjId *docId) {
  DbMap *map = MapAddr(docHndl);
  uint32_t docSize, rawSize;
  DbAddr newAddr, prevAddr;
  document_t *document;
  DbAddr *docSlot;
  JsStatus stat;
  value_t *rec;
  uint32_t idx;
  JsDoc *doc;

	//	assign a new docId slot

	if (docId->bits) {
		prevAddr = *(DbAddr *)fetchIdSlot(map, *docId);
// delete prevDoc
        } else {
          prevAddr.bits = 0;
          docId->bits =
              allocObjId(map, listFree(docHndl, 0), listWait(docHndl, 0));
        }

	docSize = calcSize(val, true);
	docSlot = fetchIdSlot(map, *docId);

    rawSize = docSize + sizeof(JsDoc) + sizeof(document_t);

	if (rawSize < 12 * 1024 * 1024)
		rawSize += rawSize / 2;

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

    docAddr(document)->maxOffset = document->docMin + sizeof(JsDoc) + docSize;
    rec = docAddr(document)->value;

    marshalDoc(val, document->base, document->docMin + sizeof(JsDoc), docSize, rec, true);

	//	install the document
	//	and return old addr

	docSlot->bits = newAddr.bits;
	return (JsStatus)prevAddr.bits;
}


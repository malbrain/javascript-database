#include "js.h"
#include "js_props.h"

#include "js_db.h"
#include "js_dbindex.h"

value_t fcnIterNext(value_t *args, value_t *thisVal, environment_t *env) {
	Ver *ver = NULL;
	Doc *doc = NULL;
	Handle *docHndl;
	DbHandle *hndl;
	JsMvcc *jsMvcc;
	Iterator *it;
	DbAddr *slot;
	value_t s;

	s.bits = vt_status;

	hndl = (DbHandle *)baseObject(*thisVal)->hndl;

	if (!(docHndl = bindHandle(hndl)))
		return s.status = DB_ERROR_handleclosed, s;

	it = (Iterator *)(docHndl + 1);
	jsMvcc = (JsMvcc *)(it + 1);

	while ((slot = iteratorNext(docHndl))) {
	  doc = getObj(docHndl->map, *slot);
	  ver = findDocVer(docHndl->map, doc, jsMvcc);

	  if (ver && !jsError(ver))
		break;
	}

	releaseHandle(docHndl, hndl);

	if (!slot || !doc || !ver)
		return s.status = DB_ITERATOR_eof, s;

	return makeDocument(ver, hndl);
}

value_t fcnIterPrev(value_t *args, value_t *thisVal, environment_t *env) {
	Ver *ver = NULL;
	Doc *doc = NULL;
	Handle *docHndl;
	DbHandle *hndl;
	JsMvcc *jsMvcc;
	DbAddr *slot;
	Iterator *it;
	value_t s;

	s.bits = vt_status;

	hndl = (DbHandle *)baseObject(*thisVal)->hndl;

	if (!(docHndl = bindHandle(hndl)))
		return s.status = DB_ERROR_handleclosed, s;

	it = (Iterator *)(docHndl + 1);
	jsMvcc = (JsMvcc *)(it + 1);

	while ((slot = iteratorPrev(docHndl))) {
	  doc = getObj(docHndl->map, *slot);
	  ver = findDocVer(docHndl->map, doc, jsMvcc);

	  if (ver && !jsError(ver))
		break;
	}

	releaseHandle(docHndl, hndl);

	if (!slot || !doc || !ver)
		return s.status = DB_ITERATOR_eof, s;

	return makeDocument(ver, hndl);
}

//  iterator.seek(ver)

value_t fcnIterSeek(value_t *args, value_t *thisVal, environment_t *env) {
	IteratorOp op = IterSeek;
	document_t *document;
	Ver *ver = NULL;
	Doc *doc = NULL;
	Handle *docHndl;
	DbHandle *hndl;
	JsMvcc *jsMvcc;
	Iterator *it;
	DbAddr *slot;
	ObjId docId;
	value_t s;

	s.bits = vt_status;

	if (args->type == vt_document)
		document = args->addr;
	else
		return s.status = ERROR_not_document, s;

	hndl = (DbHandle *)baseObject(*thisVal)->hndl;

	if (!(docHndl = bindHandle(hndl)))
		return s.status = DB_ERROR_handleclosed, s;

	it = (Iterator *)(docHndl + 1);
	jsMvcc = (JsMvcc *)(it + 1);

	doc = (Doc *)((uint8_t *)document->ver - document->ver->offset);
	docId.bits = doc->docId.bits;

	while ((slot = iteratorSeek(docHndl, op, docId))) {
	  doc = getObj(docHndl->map, *slot);
	  ver = findDocVer(docHndl->map, doc, jsMvcc);
	  op = IterNext;

	  if (ver && !jsError(ver))
		break;
	}

	if (!slot || !doc || !ver)
		return s.status = DB_ITERATOR_eof, s;

	releaseHandle(docHndl, hndl);
	return s.status = OK, s;
}

PropFcn builtinIterFcns[] = {
	{ fcnIterNext, "next" },
	{ fcnIterPrev, "prev" },
	{ fcnIterSeek, "seek" },
	{ NULL, NULL}
};

PropVal builtinIterProp[] = {
//	{ propIterOnDisk, "onDisk" },
	{ NULL, NULL}
};


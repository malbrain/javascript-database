#include "js.h"
#include "js_props.h"

#include "js_db.h"
#include "js_dbindex.h"

value_t fcnIterNext(value_t *args, value_t *thisVal) {
	object_t *oval = js_addr(*thisVal);
	document_t *document;
	value_t next, s;
	Handle *docHndl;
	DbHandle *hndl;
	JsMvcc *jsMvcc;
	Iterator *it;
	Ver *ver;
	Doc *doc;

	s.bits = vt_status;

	hndl = (DbHandle *)oval->base->hndl;

	if (!(docHndl = bindHandle(hndl)))
		return s.status = DB_ERROR_handleclosed, s;

	it = (Iterator *)(docHndl + 1);
	jsMvcc = (JsMvcc *)(it + 1);

	while ((doc = iteratorNext(docHndl)))
	  if ((ver = findDocVer(docHndl->map, doc, jsMvcc)))
		break;

	if (!doc || !ver) {
		releaseHandle(docHndl, hndl);
		return s.status = DB_ITERATOR_eof, s;
	}

	next.bits = vt_document;
	next.addr = js_alloc(sizeof(document_t), true);
	next.refcount = true;

	document = next.addr;
	document->docHndl = docHndl;
	document->ver = ver;
	return next;
}

value_t fcnIterPrev(value_t *args, value_t *thisVal) {
	object_t *oval = js_addr(*thisVal);
	document_t *document;
	value_t next, s;
	Handle *docHndl;
	DbHandle *hndl;
	JsMvcc *jsMvcc;
	Iterator *it;
	Ver *ver;
	Doc *doc;

	s.bits = vt_status;

	hndl = (DbHandle *)oval->base->hndl;

	if (!(docHndl = bindHandle(hndl)))
		return s.status = DB_ERROR_handleclosed, s;

	it = (Iterator *)(docHndl + 1);
	jsMvcc = (JsMvcc *)(it + 1);

	while ((doc = iteratorPrev(docHndl)))
	  if ((ver = findDocVer(docHndl->map, doc, jsMvcc)))
		break;

	if (!doc || !ver) {
		releaseHandle(docHndl, hndl);
		return s.status = DB_ITERATOR_eof, s;
	}

	next.bits = vt_document;
	next.addr = js_alloc(sizeof(document_t), true);
	next.refcount = true;

	document = next.addr;
	document->docHndl = docHndl;
	document->ver = ver;
	return next;
}

value_t fcnIterSeek(value_t *args, value_t *thisVal) {
	object_t *oval = js_addr(*thisVal);
	IteratorOp op = IterSeek;
	document_t *document;
	value_t next, s;
	Handle *docHndl;
	DbHandle *hndl;
	JsMvcc *jsMvcc;
	Iterator *it;
	ObjId docId;
	Ver *ver;
	Doc *doc;

	s.bits = vt_status;

	hndl = (DbHandle *)oval->base->hndl;

	if (!(docHndl = bindHandle(hndl)))
		return s.status = DB_ERROR_handleclosed, s;

	it = (Iterator *)(docHndl + 1);
	jsMvcc = (JsMvcc *)(it + 1);

	if (args->type == vt_docId)
		docId.bits = args->docBits;
	else if (args->type == vt_int) {
		docId.bits = 0;
		op = args->nval;
		iteratorSeek(docHndl, op, docId);
		releaseHandle(docHndl, hndl);
		return s.status = DB_OK, s;
	} else {
		releaseHandle(docHndl, hndl);
		return s.status = ERROR_not_docid, s;
	}

	if (!(doc = iteratorSeek(docHndl, op, docId))) {
		releaseHandle(docHndl, hndl);
		return s.status = ERROR_not_found, s;
	}
		
	if (!(ver = findDocVer(docHndl->map, doc, jsMvcc))) {
		releaseHandle(docHndl, hndl);
		return s.status = ERROR_not_found, s;
	}

	next.bits = vt_document;
	next.addr = js_alloc(sizeof(document_t), true);
	next.refcount = true;

	document = next.addr;
	document->docHndl = docHndl;
	document->ver = ver;
	return next;
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


#include "js.h"
#include "js_props.h"

#include "js_db.h"
#include "js_dbindex.h"

value_t fcnIterNext(value_t *args, value_t *thisVal) {
	object_t *oval = js_addr(*thisVal);
	document_t *document;
	value_t next, s;
	Handle *handle;
	DbHandle *hndl;
	JsMvcc *jsMvcc;
	Iterator *it;
	Ver *ver;
	Doc *doc;

	s.bits = vt_status;

	hndl = (DbHandle *)oval->base->hndl;

	if (!(handle = bindHandle(hndl)))
		return s.status = DB_ERROR_handleclosed, s;

	it = (Iterator *)(handle + 1);
	jsMvcc = (JsMvcc *)(it + 1);

	while ((doc = iteratorNext(handle)))
	  if ((ver = findDocVer(handle->map, doc, jsMvcc)))
		break;

	if (!doc || !ver) {
		releaseHandle(handle, hndl);
		return s.status = DB_ITERATOR_eof, s;
	}

	next.bits = vt_document;
	next.addr = js_alloc(sizeof(document_t), true);
	next.refcount = true;

	document = next.addr;
	*document->hndl = hndl->hndlBits;
	document->ver = ver;
	return next;
}

value_t fcnIterPrev(value_t *args, value_t *thisVal) {
	object_t *oval = js_addr(*thisVal);
	document_t *document;
	value_t next, s;
	Handle *handle;
	DbHandle *hndl;
	JsMvcc *jsMvcc;
	Iterator *it;
	Ver *ver;
	Doc *doc;

	s.bits = vt_status;

	hndl = (DbHandle *)oval->base->hndl;

	if (!(handle = bindHandle(hndl)))
		return s.status = DB_ERROR_handleclosed, s;

	it = (Iterator *)(handle + 1);
	jsMvcc = (JsMvcc *)(it + 1);

	while ((doc = iteratorPrev(handle)))
	  if ((ver = findDocVer(handle->map, doc, jsMvcc)))
		break;

	if (!doc || !ver) {
		releaseHandle(handle, hndl);
		return s.status = DB_ITERATOR_eof, s;
	}

	next.bits = vt_document;
	next.addr = js_alloc(sizeof(document_t), true);
	next.refcount = true;

	document = next.addr;
	*document->hndl = hndl->hndlBits;
	document->ver = ver;
	return next;
}

value_t fcnIterSeek(value_t *args, value_t *thisVal) {
	object_t *oval = js_addr(*thisVal);
	IteratorOp op = IterSeek;
	document_t *document;
	value_t next, s;
	Handle *handle;
	DbHandle *hndl;
	JsMvcc *jsMvcc;
	Iterator *it;
	ObjId docId;
	Ver *ver;
	Doc *doc;

	s.bits = vt_status;

	hndl = (DbHandle *)oval->base->hndl;

	if (!(handle = bindHandle(hndl)))
		return s.status = DB_ERROR_handleclosed, s;

	it = (Iterator *)(handle + 1);
	jsMvcc = (JsMvcc *)(it + 1);

	if (args->type == vt_docId)
		docId.bits = args->docBits;
	else if (args->type == vt_int) {
		docId.bits = 0;
		op = args->nval;
		iteratorSeek(handle, op, docId);
		releaseHandle(handle, hndl);
		return s.status = DB_OK, s;
	} else {
		releaseHandle(handle, hndl);
		return s.status = ERROR_not_docid, s;
	}

	if (!(doc = iteratorSeek(handle, op, docId))) {
		releaseHandle(handle, hndl);
		return s.status = ERROR_not_found, s;
	}
		
	if (!(ver = findDocVer(handle->map, doc, jsMvcc))) {
		releaseHandle(handle, hndl);
		return s.status = ERROR_not_found, s;
	}

	next.bits = vt_document;
	next.addr = js_alloc(sizeof(document_t), true);
	next.refcount = true;

	document = next.addr;
	*document->hndl = hndl->hndlBits;
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


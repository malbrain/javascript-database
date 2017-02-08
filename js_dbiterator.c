#include "js.h"
#include "js_props.h"
#include "js_dbindex.h"
#include "database/db.h"
#include "database/db_object.h"
#include "database/db_handle.h"
#include "database/db_arena.h"

value_t fcnIterNext(value_t *args, value_t *thisVal) {
	object_t *oval = js_addr(*thisVal);
	document_t *document;
	value_t next, s;
	DbHandle *hndl;
	Doc *doc;
	Ver *ver;

	s.bits = vt_status;

	hndl = (DbHandle *)oval->base->handle;

	if (!(ver = iteratorNext(hndl)))
		return s.status = ERROR_endoffile, s;
		
	doc = (Doc *)((uint8_t *)ver - ver->offset);

	next.bits = vt_document;
	next.addr = js_alloc(sizeof(document_t), true);
	next.refcount = true;

	document = next.addr;
	*document->handle = hndl->hndlBits;
	*document->addr = doc->addr.bits;
	document->ver = ver;
	return next;
}

value_t fcnIterPrev(value_t *args, value_t *thisVal) {
	object_t *oval = js_addr(*thisVal);
	document_t *document;
	value_t next, s;
	DbHandle *hndl;
	Doc *doc;
	Ver *ver;

	s.bits = vt_status;

	hndl = (DbHandle *)oval->base->handle;

	if (!(ver = iteratorPrev(hndl)))
		return s.status = ERROR_endoffile, s;
		
	doc = (Doc *)((uint8_t *)ver - ver->offset);

	next.bits = vt_document;
	next.addr = js_alloc(sizeof(document_t), true);
	next.refcount = true;

	document = next.addr;
	*document->handle = hndl->hndlBits;
	*document->addr = doc->addr.bits;
	document->ver = ver;
	return next;
}

value_t fcnIterSeek(value_t *args, value_t *thisVal) {
	object_t *oval = js_addr(*thisVal);
	IteratorPos pos = PosAt;
	document_t *document;
	value_t next, s;
	DbHandle *hndl;
	ObjId docId;
	Doc *doc;
	Ver *ver;

	s.bits = vt_status;

	hndl = (DbHandle *)oval->base->handle;

	if (args->type == vt_docId)
		docId.bits = args->docBits;
	else if (args->type == vt_int) {
		docId.bits = 0;
		pos = args->nval;
		iteratorSeek(hndl, pos, docId);
		return s.status = DB_OK, s;
	} else
		return s.status = ERROR_not_docid, s;

	 if (!(ver = iteratorSeek(hndl, pos, docId)))
		return s.status = ERROR_not_found, s;
		
	doc = (Doc *)((uint8_t *)ver - ver->offset);

	next.bits = vt_document;
	next.addr = js_alloc(sizeof(document_t), true);
	next.refcount = true;

	document = next.addr;
	*document->handle = hndl->hndlBits;
	*document->addr = doc->addr.bits;
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


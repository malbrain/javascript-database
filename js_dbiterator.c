#include "js.h"
#include "js_props.h"

#include "js_db.h"
#include "js_dbindex.h"

value_t fcnIterNext(value_t *args, value_t thisVal, environment_t *env) {
	Handle *docHndl;
	Iterator *it;
    DbMap *map;
	value_t s;

	s.bits = vt_status;
        thisVal = js_handle(thisVal, Hndl_iterator);

	if( thisVal.ishandle )
	  if ((docHndl = bindHandle(thisVal.hndl, Hndl_iterator)))
		it = ClntAddr(docHndl);
	  else
		return s.status = DB_ERROR_handleclosed, s;
	else
		return s.status = DB_ERROR_notbasever, s;

	map = MapAddr(docHndl);

	if ((iteratorNext(docHndl)))
          s = makeDocument(it->docId, map);
        else
          s.status = DB_ITERATOR_eof;

	releaseHandle(docHndl, thisVal.hndl);
	return s;
}

value_t fcnIterPrev(value_t *args, value_t thisVal, environment_t *env) {
	Handle *docHndl;
	Iterator *it;
	value_t s;
    DbMap *map;

	s.bits = vt_status;

	thisVal = js_handle(thisVal, Hndl_iterator);

	if (thisVal.ishandle)
       if ((docHndl = bindHandle(thisVal.hndl, Hndl_iterator)))
           it = ClntAddr(docHndl);
       else
		   return s.status = DB_ERROR_handleclosed, s;
	else
          return s.status = DB_ITERATOR_nothandle, s;

	map = MapAddr(docHndl);

	if ((iteratorPrev(docHndl)))
        s = makeDocument(it->docId, map);
    else 
		s.status = DB_ITERATOR_eof;

	releaseHandle(docHndl, thisVal.hndl);
	return s;
}

//  iterator.seek(ver)

value_t fcnIterSeek(value_t *args, value_t thisVal, environment_t *env) {
	IteratorOp op = IterSeek;
	Handle *docHndl;
    Iterator *it;
	ObjId docId;
	value_t s;
    DbMap *map;

	s.bits = vt_status;
	s.status = OK;

	if (args->type == vt_int) {
		docId.bits = 0;
		op = (IteratorOp)args->nval;
	} else if (args->type == vt_docId) {
		docId.bits = args->idBits;
		op = IterSeek;
	} else
		return s.status = ERROR_not_operator_int, s;

	thisVal = js_handle(thisVal, Hndl_iterator);
	
	if (thisVal.ishandle)
      if ((docHndl = bindHandle(thisVal.hndl, Hndl_iterator)))
        it = ClntAddr(docHndl);
      else
		return s.status = DB_ERROR_handleclosed, s;
	else
		return s.status = DB_ERROR_notbasever, s;

	map = MapAddr(docHndl);

	if ((iteratorSeek(docHndl, op, docId)))
        s = makeDocument(it->docId, map);
    else
		s.status = DB_ITERATOR_eof;

	releaseHandle(docHndl, thisVal.hndl);
	return s;
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


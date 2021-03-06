#include "js.h"
#include "js_props.h"
#include "js_vector.h"

#include "js_db.h"
#include "js_dbindex.h"

value_t fcnIterNext(value_t *args, value_t thisVal, environment_t *env) {
	Handle *docHndl;
	DbMap *docMap;
	uint32_t count, idx = 0;
	Iterator *it;
	value_t s, v;

	s.bits = vt_status;

	if((docHndl = js_handle(thisVal, Hndl_iterator)))
		it = ClntAddr(docHndl);
	  else
		return s.status = DB_ERROR_handleclosed, s;

	docMap = MapAddr(docHndl);

	if (vec_cnt(args))
      count = (uint32_t)args[0].nval;
    else
      count = 0;

    while (iteratorNext(docHndl)) {
		  value_t d;

		  d.bits = vt_docId;
		  d.idBits = it->docId.bits;
		  d.hndlIdx = docHndl->hndlIdx;

          if (count == 0) {
            v = d;
            idx++;
            break;
          }
          
		  if (idx == 0) 
			  v = newArray(array_value, count);

          vec_push(v.aval->valuePtr, d);
          if (++idx > count) break;
    }

	releaseHandle(docHndl);
    
	if (idx) return v;

	return s.status = DB_ITERATOR_eof, s;
}

value_t fcnIterPrev(value_t *args, value_t thisVal, environment_t *env) {
  int count, idx = 0;
	Handle *docHndl;
	Iterator *it;
	value_t s, v;
	DbMap *docMap;
	s.bits = vt_status;

	if((docHndl = js_handle(thisVal, Hndl_iterator)))
      it = ClntAddr(docHndl);
    else
	   return s.status = DB_ERROR_handleclosed, s;

	docMap = MapAddr(docHndl);

	if (vec_cnt(args))
          count = (uint32_t)args[0].nval;
        else
          count = 0;

    while (iteratorPrev(docHndl)) {
		value_t d;

		d.bits = vt_docId;
		d.idBits = it->docId.bits;
		d.hndlIdx = docHndl->hndlIdx;
		
		if (count == 0) {
            v = d;
            idx++;
            break;
          }

          if (idx == 0) v = newArray(array_value, count);

          vec_push(v.aval->valuePtr, d);
          if (++idx > count) break;
        }

        releaseHandle(docHndl);

        if (idx) return v;

        return s.status = DB_ITERATOR_eof, s;
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

	if((docHndl = js_handle(thisVal, Hndl_iterator)))
        it = ClntAddr(docHndl);
      else
		return s.status = DB_ERROR_handleclosed, s;

	map = MapAddr(docHndl);

	if ((iteratorSeek(docHndl, op, docId))) {
		value_t d;
		d.bits = vt_docId;
		d.idBits = it->docId.bits;
		d.hndlIdx = docHndl->hndlIdx;
	} else
		s.status = DB_ITERATOR_eof;

	releaseHandle(docHndl);
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


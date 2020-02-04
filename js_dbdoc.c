#include "js.h"
#include "js_db.h"
#include "js_props.h"
#include "js_string.h"

#include "js_dbindex.h"
#include <stddef.h>

void *js_dbaddr(value_t val, document_t * doc) {
  if (val.marshaled && val.document) 
	  doc = val.document;

  if( val.marshaled )
	  return doc->base + val.offset;

  if ((vt_document == val.type))
    return val.document->base + val.offset;
  else
    return val.addr;

  fprintf(stderr, "Not document item: %s\n", strtype(val.type));
	exit(1);
}

value_t makeDocument(ObjId docId, DbMap *map) {
  DbAddr *addr = fetchIdSlot(map, docId);
  document_t *document = getObj(map, *addr);
  value_t val;

	val.bits = vt_document;
	val.document = document;

	incrRefCnt(val);
	return val;
}

//	delete a document

void deleteDocument(value_t val) {
	if (decrRefCnt(val))
		deleteValue(val);
}

//  convert document object to modifiable object

value_t convDocObject(value_t obj) {
	if (obj.type == vt_document) {
	  if (obj.document->value->marshaled)
		obj = cloneValue(*obj.document->value);
          else
	    obj = *obj.document->value;
	} else {
	  if (obj.marshaled)
		obj = cloneValue(obj);
	}

	incrRefCnt(obj);
	return obj;
}

// retrieve document object

value_t getDocObject(value_t doc) {
  value_t ans;
  incrRefCnt(doc);
  ans.bits = doc.document->value->bits;
  ans.document = doc.document;
  return ans;
}

//	clone marshaled array

value_t cloneArray(value_t obj) {
	dbarray_t *dbaval = (dbarray_t *)(obj.document->base + obj.offset);
	uint32_t cnt = dbaval->cnt, idx;
	value_t val = newArray(array_value, cnt + cnt / 4);

	for (idx = 0; idx < cnt; idx++) {
	  value_t element = dbaval->valueArray[idx];

	  if (element.marshaled) {
		element.document = obj.document;
		incrRefCnt(element);
	  }

	  val.aval->valuePtr[idx] = element;
	}

	return val;
}

value_t cloneObject(value_t obj) {
	dbobject_t *dboval = (dbobject_t *)(obj.document->base + obj.offset);
	value_t val = newObject(vt_object);
	pair_t *pairs = dboval->pairs;
	uint32_t cnt = dboval->cnt;
	uint32_t cap, hashEnt;
	value_t left, right;
	uint32_t h, idx;
	void *hashTbl;

	pair_t *newPair = newVector(cnt + cnt / 4, sizeof(pair_t), true);
	val.oval->pairsPtr = newPair;

	cap = vec_max(newPair);
	hashTbl = newPair + cap;
	hashEnt = hashBytes(cap);

	for (idx = 0; idx < cnt; idx++) {
	  left = pairs[idx].name;

	  if (left.marshaled) {
		left.document = obj.document;
		incrRefCnt(left);
	  }

	  h = -lookupValue(val, left, 0, false);
	  right = pairs[idx].value;

	  if (right.marshaled) {
		right.document = obj.document;
		incrRefCnt(right);
	  }

	  replaceSlot (&newPair[idx].name, left);
	  replaceSlot (&newPair[idx].value, right);
	  hashStore(hashTbl, hashEnt, h, idx + 1);
	}

	vec_size(newPair) = cnt;
	return val;
}

//	document store Append method
//	return docId, or array of docId

value_t fcnStoreAppend(value_t *args, value_t thisVal, environment_t *env) {
	document_t *prevDoc;
	Handle *docHndl;
    value_t resp, s, v;
    DbAddr *addr;
	value_t hndl;
	uint32_t idx;
    ObjId docId[1];
    DbMap *map;

	s.bits = vt_status;
	s.status = 0;

	hndl = js_handle(thisVal, Hndl_docStore);

	if (hndl.ishandle)
          if (!(docHndl = bindHandle(hndl.hndl, Hndl_docStore)))
            return s.status = DB_ERROR_handleclosed, s;
          else
            map = MapAddr(docHndl);
    else
        return hndl;

	// multiple document/value case

	if (args[0].type == vt_array) {
	  dbarray_t *dbaval = js_dbaddr(args[0], NULL);
	  value_t *values = args[0].marshaled ? dbaval->valueArray : args[0].aval->valuePtr;
	  uint32_t cnt = args[0].marshaled ? dbaval->cnt : vec_cnt(values);
	  resp = newArray(array_value, cnt);
	  array_t *respval = resp.addr;

	  for (idx = 0; idx < cnt; idx++) {
        docId->bits = 0;
		prevDoc = appendDoc(docHndl, values[idx], docId);

		if (jsError(prevDoc)) {
                  s.status = (Status)prevDoc;
                  break;
        }

		v.bits = vt_docId;
        v.idBits = docId->bits;
        respval->valuePtr[idx] = v;
	  }
	} else {
       docId->bits = 0;
       prevDoc = appendDoc(docHndl, args[0], docId);

	   v.bits = vt_docId;
       v.idBits = docId->bits;

	  if (jsError(prevDoc))
		s.status = (Status)prevDoc;
	  else
	  	resp = v;
	}

	if (!s.status)
		return resp;

	deleteValue(resp);
	return s;
}

//	convert DocId to string

value_t fcnDocIdToString(value_t *args, value_t thisVal, environment_t *env) {
	char buff[64];
	ObjId docId;
	int len;

	docId.bits = thisVal.idBits;

#ifndef _WIN32
	len = snprintf(buff, sizeof(buff), "%X:%X", docId.seg, docId.idx);
#else
	len = _snprintf_s(buff, sizeof(buff), _TRUNCATE, "%X:%X", docId.seg, docId.idx);
#endif
	return newString(buff, len);
}

//	display a document

value_t fcnDocToString(value_t *args, value_t thisVal, environment_t *env) {
	return conv2Str(*thisVal.document->value, true, false);
}

//	return base value for a document version (usually a vt_document object)

value_t fcnDocValueOf(value_t *args, value_t thisVal, environment_t *env) {
	return *thisVal.document->value;
}

//	return size of a document version

value_t fcnDocSize(value_t *args, value_t thisVal, environment_t *env) {
	value_t v;

	v.bits = vt_int;
	v.nval = thisVal.document->docLen;
	return v;
}

//	update a document

value_t fcnDocUpdate(value_t *args, value_t thisVal, environment_t *env) {
  Handle *docHndl;
  document_t *prevDoc;
  uint32_t idx;
  value_t s, hndl;
  ObjId docId[1];
  DbMap *map;

	s.bits = vt_status;
	s.status = 0;

	docId->bits = thisVal.idBits;

	docHndl = bindHandle(args[0].hndl, Hndl_docStore);

	prevDoc = appendDoc(docHndl, args[1], docId);
    map = MapAddr(docHndl);

	if (jsError(prevDoc))
	  s.status = (Status)prevDoc;
	else
	  s = makeDocument(*docId, map);

	releaseHandle(docHndl, args[0].hndl);
	return s;
}

//	return the docId of a version

value_t propDocDocId(value_t val, bool lval) {
	value_t v;

	v.bits = vt_docId;
	v.idBits = val.document->docId.bits;
	return v;
}

PropFcn builtinDocIdFcns[] = {
	{ fcnDocIdToString, "toString" },
	{ NULL, NULL}
};

PropVal builtinDocIdProp[] = {
	{ NULL, NULL}
};

PropFcn builtinDocFcns[] = {
	{ fcnDocToString, "toString" },
	{ fcnDocValueOf, "valueOf" },
	{ fcnDocUpdate, "update" },
	{ fcnDocSize, "size" },
	{ NULL, NULL}
};

PropVal builtinDocProp[] = {
	{ propDocDocId, "docId" },
	{ NULL, NULL}
};

PropFcn builtinStoreFcns[] = {
	{ fcnStoreAppend, "append" },
	{ NULL, NULL}
};

PropVal builtinStoreProp[] = {
	{ NULL, NULL}
};


#include "js.h"
#include "js_db.h"
#include "js_props.h"
#include "js_string.h"

#include "js_dbindex.h"
#include <stddef.h>

void *js_dbaddr(value_t val, document_t * document) {
  if (val.marshaled && val.document) 
	  document = val.document;

  if( val.marshaled && document )
	  return document->base + val.offset;

  if ((vt_document == val.type))
    return val.document->base + val.offset;
  else
    return val.addr;

//  fprintf(stderr, "Not document item: %s\n", strtype(val));
//	exit(1);
}

value_t makeDocument(ObjId docId, DbMap *map) {
  DbAddr *addr = fetchIdSlot(map, docId);
  document_t *document = getObj(map, *addr);
  value_t val;

  val.bits = vt_document;
  val.document = document;

  switch (document->docType) {
    case DocRaw:
      break;
    case DocMvcc:
      val.offset = mvccAddr(document)->newestVer;
  };

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
	  if (docAddr(obj.document)->value->marshaled)
		obj = cloneValue(*docAddr(obj.document)->value);
          else
	    obj = *docAddr(obj.document)->value;
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
  ans.bits = docAddr(doc.document)->value->bits;
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

//	retreive  document value

value_t fcnDocIdRetreive(value_t *args, value_t thisVal, environment_t *env) {
  DocStore *docStore;
  Handle *docHndl;
  DbMap *docMap = NULL;
  ObjId docId;
  ObjId txnId;
  value_t v, s, base;
  int arg, cnt = vec_cnt(args);

  s.bits = vt_status;
  txnId.bits = 0;

  if (thisVal.type != vt_docId) {
    fprintf(stderr, "Error: docIdRetrieve => expecting docId => %s\n",
            strtype(thisVal));
  
    return s.status = ERROR_script_internal, s;
  }

  for (arg = 0; arg < cnt; arg++) 
    switch ((base = args[arg]).type) {
      case vt_txnId:
        txnId.bits = base.idBits;
        break;
      case vt_object:
        base = base.oval->baseVal[0];

      case vt_hndl:
        if ((docHndl = js_handle(base, Hndl_docStore)))
          docMap = MapAddr(docHndl);
        else
          return s.status = DB_ERROR_badhandle, s;

        docStore = (DocStore *)(docMap->arena + 1);
        break;
    }

  docId.bits = thisVal.idBits;
  v = makeDocument(docId, docMap);

  releaseHandle(docHndl);
  return v;
}

//	convert DocId to string

value_t fcnDocIdToString(value_t *args, value_t thisVal, environment_t *env) {
	char buff[64];
	ObjId docId;
	int len;

	docId.bits = thisVal.idBits;

#ifndef _WIN32
	len = snprintf(buff, sizeof(buff), "%X:%.5X", docId.seg, docId.idx);
#else
	len = _snprintf_s(buff, sizeof(buff), _TRUNCATE, "%X:%.5X", docId.seg, docId.idx);
#endif
	return newString(buff, len);
}

//	display a document

value_t fcnDocToString(value_t *args, value_t thisVal, environment_t *env) {
	return conv2Str(docAddr(thisVal.document)->value[0], true, false);
}

//	return base value for a document version (usually a vt_document object)

value_t fcnDocValueOf(value_t *args, value_t thisVal, environment_t *env) {
	return docAddr(thisVal.document)->value[0];
}

//	return size of a document version

value_t fcnDocSize(value_t *args, value_t thisVal, environment_t *env) {
	value_t v;

	v.bits = vt_int;
	v.nval = docAddr(thisVal.document)->maxOffset - thisVal.document->docMin ;
	return v;
}

//  docStore.createIterator(options)

value_t fcnStoreCreateIterator(value_t *args, value_t thisVal,
                               environment_t *env) {
  Params params[MaxParam + 1];
  Iterator *iterator;
  value_t iter;
  Handle *iterHndl;
  value_t s;

  s.bits = vt_status;

  if (debug) fprintf(stderr, "funcall : CreateIterator\n");

  // process options array

  if (vec_cnt(args))
    processOptions(params, args[0]);
  else 
    memset(params, 0, sizeof(params));

  while (thisVal.type == vt_object)
      thisVal = thisVal.oval->baseVal[0];

  if ((s.status = (int)createIterator(iter.hndl, thisVal.hndl, params))) return s;

  iterHndl = bindHandle(iter.hndl, Hndl_iterator);
  iterator = ClntAddr(iterHndl);

  iter.bits = vt_hndl;
  iter.subType = Hndl_iterator;

  releaseHandle(iterHndl);
  return iter;
}

value_t fcnStoreUpdate(value_t *args, value_t thisVal, environment_t *env) {
  value_t s;

  s.bits = vt_status;

  return s;
}

value_t fcnStoreRead(value_t *args, value_t thisVal, environment_t *env) {
  value_t s;

  s.bits = vt_status;

  return s;
}

  //	document store write method
  //	return docId, or array of docId

value_t fcnStoreWrite(value_t *args, value_t thisVal, environment_t *env) {
  struct Document *prevDoc;
  DocStore *docStore;
  Handle *docHndl;
  value_t resp, s, v;
  uint32_t idx;
  ObjId docId[1], txnId;
  DbMap *map;

  s.bits = vt_status;
  s.status = 0;

  if((docHndl = js_handle(thisVal, Hndl_docStore))) 
      map = MapAddr(docHndl);
  else
      return s.status = DB_ERROR_handleclosed, s;

  docStore = (DocStore *)(map->arena + 1);

  if (vec_cnt(args) > 1 && args[1].type == vt_txnId)
    txnId.bits = args[1].idBits;
  else
    txnId.bits = 0;

  // multiple document/value case

  if (args[0].type == vt_array) {
    dbarray_t *dbaval = js_dbaddr(args[0], NULL);
    value_t *values =
        args[0].marshaled ? dbaval->valueArray : args[0].aval->valuePtr;
    uint32_t cnt = args[0].marshaled ? dbaval->cnt : vec_cnt(values);
    resp = newArray(array_value, cnt);
    array_t *respval = resp.addr;

    for (idx = 0; idx < cnt; idx++) {
      docId->bits = 0;
      prevDoc = writeDoc(docHndl, values[idx], docId, txnId);

      if (jsError(prevDoc)) {
        s.status = (Status)prevDoc;
        break;
      }

      v.bits = vt_docId;
      v.idBits = docId->bits;
      vec_push(respval->valuePtr, v);
    }
  } else {
    docId->bits = 0;
    prevDoc = writeDoc(docHndl, args[0], docId, txnId);

    v.bits = vt_docId;
    v.idBits = docId->bits;

    if (jsError(prevDoc))
      s.status = (Status)prevDoc;
    else
      resp = v;
  }

  if (!s.status) return resp;

  abandonValue(resp);
  return s;
}

//	update a document
//  doc

value_t fcnDocUpdate(value_t *args, value_t thisVal, environment_t *env) {
  document_t *prevDoc;
  Handle *docHndl;
  value_t s;
  ObjId docId[1], txnId;

	s.bits = vt_status;
	s.status = 0;

	docId->bits = thisVal.idBits;

	if (vec_cnt(args) > 1 && args[1].type == vt_txnId)
          txnId.bits = args[1].idBits;
        else
          txnId.bits = 0;

    docHndl = js_handle(args[0], Hndl_docStore);
    prevDoc = writeDoc(docHndl, args[1], docId, txnId);

        if (jsError(prevDoc))
          s.status = (Status)prevDoc;
        else {
          s.bits = vt_docId;
          s.idBits = docId->bits;
        }
        releaseHandle(docHndl);
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
    { fcnDocIdRetreive, "retrieve"},
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

PropFcn builtinStoreFcns[] = {{fcnStoreWrite,  "writeDocs"},
                              {fcnStoreRead,   "readDocs"},
                              {fcnStoreUpdate, "updateDocs"},
                              {fcnStoreCreateIterator, "createIterator"},
    {NULL, NULL}};

PropVal builtinStoreProp[] = {
	{ NULL, NULL}
};


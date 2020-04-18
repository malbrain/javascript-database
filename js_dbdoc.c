#include "js.h"
#include "js_db.h"
#include "js_props.h"
#include "js_string.h"

#include "js_dbindex.h"
#include <stddef.h>

void *js_dbaddr(value_t val, document_t *rawDoc) {
  if (val.marshaled && val.rawDoc) 
	  rawDoc = val.rawDoc;

  if( val.marshaled && rawDoc )
	  return rawDoc->base + val.offset;

  if ((vt_document == val.type))
    return val.rawDoc->base + val.offset;
  else
    return val.addr;

//  fprintf(stderr, "Not document item: %s\n", strtype(val));
//	exit(1);
  }

//	delete a document

void deleteDocument(value_t val) {
	if (decrRefCnt(val))
		deleteValue(val);
}

//  convert document object to modifiable object

value_t convDocObject(value_t obj) {
	if (obj.type == vt_document) {
	  if (jsDocAddr(obj)->value->marshaled)
		obj = cloneValue(*jsDocAddr(obj)->value);
          else
	    obj = *jsDocAddr(obj)->value;
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
  ans.bits = jsDocAddr(doc)->value->bits;
  ans.rawDoc = doc.rawDoc;
  return ans;
}

//	clone marshaled array

value_t cloneArray(value_t obj) {
	dbarray_t *dbaval = (dbarray_t *)(obj.rawDoc->base + obj.offset);
	uint32_t cnt = dbaval->cnt, idx;
	value_t val = newArray(array_value, cnt + cnt / 4);

	for (idx = 0; idx < cnt; idx++) {
	  value_t element = dbaval->valueArray[idx];

	  if (element.marshaled) {
		element.rawDoc = obj.rawDoc;
		incrRefCnt(element);
	  }

	  val.aval->valuePtr[idx] = element;
	}

	return val;
}

value_t cloneObject(value_t obj) {
	dbobject_t *dboval = (dbobject_t *)(obj.rawDoc->base + obj.offset);
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
		left.rawDoc = obj.rawDoc;
		incrRefCnt(left);
	  }

	  h = -lookupValue(val, left, 0, false);
	  right = pairs[idx].value;

	  if (right.marshaled) {
		right.rawDoc = obj.rawDoc;
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
  MVCCResult result;
  Handle *docHndl;
  DbAddr *docSlot;
  DbMap *docMap;
  ObjId docId;
  Txn *txn = NULL;
  Doc *doc;
  Ver *ver;
  value_t v, s, txnId, base;
  int arg, cnt = vec_cnt(args);

  s.bits = vt_status;
  txnId.bits = 0;

  docHndl = getDocIdHndl(thisVal.hndlIdx);
  docMap = MapAddr(docHndl);

  docId.bits = thisVal.idBits;
  docSlot = fetchIdSlot(docMap, docId);
  doc = getObj(docMap, *docSlot);

  for (arg = 0; arg < cnt; arg++)
    switch ((base = args[arg]).type) {
      case vt_txnId:
          txn = js_fetchTxn(base);
          break;
      case vt_object:
        base = base.oval->baseVal[0];
        break;
    }

  if (doc->doc->docType == VerRaw) {
    v.bits = vt_document;
    v.rawDoc = doc->doc;
    return v;
  }

  if(doc->doc->docType == VerMvcc) 
    result = mvcc_findDocVer(txn, doc, docHndl);

  mvcc_releaseTxn(txn);

  if ((s.status = result.status))
      return s;

  if (result.objType == objVer)
      ver = result.object;
  else
      return s.status = DB_ERROR_no_visible_version, s;

  v.bits = vt_document;
  v.rawDoc = (document_t *)(ver->verBase - ver->stop->offset);
  v.offset = ver->stop->offset;
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
	return conv2Str(jsDocAddr(thisVal)->value[0], true, false);
}

//	return base value for a document version (usually a vt_document object)

value_t fcnDocValueOf(value_t *args, value_t thisVal, environment_t *env) {
	return jsDocAddr(thisVal)->value[0];
}

//	return size of a document version

value_t fcnDocSize(value_t *args, value_t thisVal, environment_t *env) {
	value_t v;
    uint32_t docMin;
    value_t doc = thisVal;

    if (doc.marshaled)
        if (doc.rawDoc->docType == VerMvcc)
            docMin = sizeof(Ver) + sizeof(JsDoc);
        else
            docMin = sizeof(struct Document) + sizeof(JsDoc);

	v.bits = vt_int;
	v.nval = jsDocAddr(doc)->maxOffset - docMin ;
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
  //    add index keys

value_t fcnStoreWrite(value_t *args, value_t thisVal, environment_t *env) {
  struct Document *prevDoc;
  DocStore *docStore;
  Handle *docHndl;
  value_t keys[1], resp, s, v, *idxHndlVector, *next;
  uint32_t idx = 0, reps = 0, cnt, arg = 0;
  dbarray_t *dbaval;
  ObjId docId[1];
  DbMap *map;

  s.bits = vt_status;
  s.status = 0;

  if((docHndl = js_handle(thisVal, Hndl_docStore))) 
      map = MapAddr(docHndl);
  else
      return s.status = DB_ERROR_handleclosed, s;

  docStore = (DocStore *)(map->arena + 1);
  resp = newArray(array_value, 0);
  *keys = newArray(array_value, 0);
  arg = 0;
  reps = 0;

  //  gather index array
  /*
  if (args[arg].type == vt_array)
      if ((cnt = vec_cnt(args[arg].aval->valuePtr)))
          idxHndlVector = args[arg++].aval->valuePtr;
*/

  while (arg < vec_cnt(args)) {
    switch (args[arg].type) {
      case vt_array:
        if (idx == 0) {
          dbaval = js_dbaddr(args[arg], NULL);
          next = args[arg].marshaled ? dbaval->valueArray : args[arg].aval->valuePtr;
          reps = args[arg].marshaled ? dbaval->cnt : vec_cnt(next);
        } else
          next++;

        if (idx++ < reps)
          break;

        reps = 0;
        idx = 0;
        arg++;
        continue;

      case vt_document:
        next = jsDocAddr(args[arg++])->value;
        break;

      default:
        next = args + arg++;
        break;
    }

    docId->bits = 0;
    prevDoc = writeDoc(docHndl, *next, docId);

    if (jsError(prevDoc)) {
      s.status = (Status)prevDoc;
      break;
    }

    v.bits = vt_docId;
    v.idBits = docId->bits;
    v.hndlIdx = docHndl->hndlIdx;

    vec_push(resp.aval->valuePtr, v);

//    if ((s.status = idxBldKeyHelper(*next, docId, docHndl, idxHndlVector, keys)))
//      return s;
  }
//  if (!s.status) 
      return resp;

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

	if (vec_cnt(args) && args[0].type == vt_txnId)
          txnId.bits = args[0].idBits;
        else
          txnId.bits = 0;

    docHndl = js_handle(args[0], Hndl_docStore);
    prevDoc = writeDoc(docHndl, args[1], docId);

        if (jsError(prevDoc))
          s.status = (Status)prevDoc;
        else {
          s.bits = vt_docId;
          s.idBits = docId->bits;
          s.hndlIdx = docHndl->hndlIdx;
        }
        releaseHandle(docHndl);
  return s;
}

//	return the docId of a version

value_t propDocDocId(value_t val, bool lval) {
	value_t v;

	v.bits = vt_docId;
	v.idBits = val.rawDoc->docId.bits;
    v.hndlIdx = val.rawDoc->hndlIdx;
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


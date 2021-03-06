#include "js.h"
#include "js_db.h"
#include "js_props.h"

#include "database/mvcc_dbapi.h"

extern DbMap *txnMap;

//	fetch and lock txn

Txn* js_fetchTxn(value_t hndl) {
    ObjId txnId;
    Txn *txn;

    while (hndl.type == vt_object)
        hndl = *hndl.oval->baseVal;

    if (hndl.type == vt_txnId)
        txnId.bits = hndl.idBits;

    txn = fetchIdSlot(txnMap, txnId);

    lockLatch(txn->latch);
    return txn;
}

//	beginTxn(options)

value_t js_beginTxn(uint32_t args, environment_t *env) {
  Params params[MaxParam + 1];
  value_t v, opts, nest;
  MVCCResult result;
  ObjId nestId;

  if (debug) fprintf(stderr, "funcall : beginTxn\n");

  // process options array

  opts = eval_arg(&args, env);
  processOptions(params, opts);
  abandonValue(opts);

  nest = eval_arg(&args, env);

  if (nest.type == vt_txnId)
    nestId.bits = nest.idBits;
  else
    nestId.bits = 0;

  result = mvcc_BeginTxn(params, nestId);

  if ((v.status = result.status)) return v.bits = vt_status, v;

  v.bits = vt_txnId;
  v.idBits = result.bits;
  return v;
}

  //	rollbackTxn()

  value_t fcnRollbackTxn(value_t *args, value_t thisVal, environment_t *env) {
  Params params[MaxParam + 1];
  MVCCResult result;
  value_t v;

  v.bits = vt_status;

  if (debug) fprintf(stderr, "funcall : rollbackTxn\n");

  // process options array

  if (vec_cnt(args))
    processOptions(params, args[0]);
  else
    memset(params, 0, sizeof(params));

  result = mvcc_RollbackTxn(params, thisVal.idBits);
  v.status = result.status;
  return v;
}

//	commitTxn(options)

value_t fcnCommitTxn(value_t *args, value_t thisVal, environment_t *env) {
  Params params[MaxParam + 1];
  MVCCResult result;
  value_t v;
  Txn *txn;

  v.bits = vt_status;

  if (debug) fprintf(stderr, "funcall : commitTxn\n");

  if (!(txn = js_fetchTxn(thisVal)))
      return v.status = DB_ERROR_badhandle, v;

  // process options array

  if (vec_cnt(args))
    processOptions(params, args[0]);
  else
    memset(params, 0, sizeof(params));

  result = mvcc_CommitTxn(txn, params);
  v.status = result.status;
  return v;
}

//  add new docs array to txn write set

DbStatus txnHelper(Txn *txn, value_t next, enum TxnStep step) {
    MVCCResult result = {
        .value = 0, .count = 0, .objType = objTxn, .status = DB_OK };
    ObjId docId;
    DbAddr *addr;
  Handle *docHndl;
  DbMap *docMap;
  Doc *doc;
  Ver *ver;

  docHndl = getDocIdHndl(next.hndlIdx);
  docMap = MapAddr(docHndl);

  switch (next.type) {
    case vt_docId:
      docId.bits = next.idBits;
      addr = fetchIdSlot(docMap, docId);
      doc = getObj(docMap, *addr);

      if (doc->doc->docType == VerRaw)
          return DB_OK;

      result = mvcc_findDocVer(txn, doc, docHndl);
  
      if (result.status)
          return result.status;

      ver = result.object;
      break;

    case vt_document:
      doc = mvccDoc(next.rawDoc);
      ver = mvccVer(next);
      break;

    default:
      return DB_OK;
  }

  switch (step) {
    case TxnRdr:
      result = mvcc_addDocRdToTxn(txn, ver);
      break;

    case TxnWrt:
      if(doc->op != OpRaw)
        result = mvcc_addDocWrToTxn(txn, doc);

      break;
  }

  return result.status;
}

//  fill new Txn from write docIds

value_t fcnWrtTxn(value_t *args, value_t thisVal, environment_t *env) {
  unsigned int max, idx = 0, cnt = 0;
  value_t v, *next, resp;
  Txn *txn;

  v.bits = vt_status;
  v.status = DB_OK;

  max = vec_cnt(args);
  resp = newArray(array_value, max);

  txn = js_fetchTxn(thisVal);

  for (idx = 0; idx < max; idx++) { 
    if (args[idx].type == vt_array) {
      for (cnt = 0; cnt < vec_cnt(args[idx].aval->valuePtr); cnt++) {
        next = args[idx].aval->valuePtr + cnt;
        if ((v.status = txnHelper(txn, *next, TxnWrt)))
            break;
        vec_push(resp.aval->valuePtr, *next);
        continue;
      }
      break;
    }

    if (args[idx].type == vt_docId) {
        if ((v.status = txnHelper(txn, args[idx], TxnWrt)))
            break;
        else {
            vec_push(resp.aval->valuePtr, args[idx]);
            continue;
        }
    }

    if(args[idx].type == vt_document) {
      value_t val;
      next = args + idx;
        
      val.bits = vt_docId;
      val.idBits = next->rawDoc->docId.bits;
      val.hndlIdx = next->rawDoc->hndlIdx;

      if ((v.status = txnHelper(txn, val, TxnWrt)))
          break;
      else {
          vec_push(resp.aval->valuePtr, val);
          continue;
      }
    }
  }
   mvcc_releaseTxn(txn);

   if (v.status)
       return v;
   return resp;
}

// add document read to txn

value_t fcnRdrTxn(value_t *args, value_t thisVal, environment_t *env) {
  MVCCResult result;
  uint32_t idx, argIdx = 0;
  value_t v;
  Txn *txn;
  Ver *ver;

  v.bits = vt_status;

  txn = js_fetchTxn(thisVal);

  for (argIdx = 0; argIdx < vec_cnt(args); argIdx++) {
    switch (args[argIdx].type) {
      case vt_document:
        ver = mvccVer(args[argIdx]);
        result = mvcc_addDocRdToTxn(txn, ver);
        break;

      case vt_array:
        for (idx = 0; idx < vec_cnt(args[argIdx].aval->valuePtr); idx++) {
          value_t *next = args[argIdx].aval->valuePtr + idx;
          switch (next->type) {
            case vt_document:
              ver = mvccVer(next[0]);
              result = mvcc_addDocRdToTxn(txn, ver);
              if ((v.status = result.status)) 
                  break;
          }
        }
    }
    if ((v.status = result.status)) break;
  }
  return v;
}

//	display a txn

value_t fcnTxnToString(value_t *args, value_t thisVal, environment_t *env) {
  char buff[64];
  ObjId txnId;
  int len;

  txnId.bits = thisVal.idBits;

#ifndef _WIN32
  len = snprintf(buff, sizeof(buff), "%X:%X", txnId.seg, txnId.idx);
#else
  len =
      _snprintf_s(buff, sizeof(buff), _TRUNCATE, "%X:%X", txnId.seg, txnId.idx);
#endif
  return newString(buff, len);
}

value_t propTxnCount(value_t val, bool lVal) {
  value_t count;
  ObjId txnId;

  count.bits = vt_int;
  count.nval = 0;

  if (val.type == vt_txnId)
    if ((txnId.bits = val.idBits)) {
      Txn *txn = fetchIdSlot(txnMap, txnId);
      count.nval = txn->wrtCount;
    }

  return count;
}

PropFcn builtinTxnFcns[] = {
    {fcnTxnToString, "toString" },
    {fcnRollbackTxn, "rollback"},
    {fcnCommitTxn, "commit"},
    {fcnWrtTxn, "writeTxn"},
    {fcnRdrTxn, "readTxn"},
    {NULL, NULL}};

PropVal builtinTxnProp[] = {
    {propTxnCount, "count"},
    {NULL, NULL}};

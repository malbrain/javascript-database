#include "js.h"
#include "js_db.h"
#include "js_props.h"

#include "database/mvcc_dbapi.h"

extern DbMap *txnMap;

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

  v.bits = vt_status;

  if (debug) fprintf(stderr, "funcall : commitTxn\n");

  // process options array

  if (vec_cnt(args))
    processOptions(params, args[0]);
  else
    memset(params, 0, sizeof(params));

  result = mvcc_CommitTxn(params, thisVal.idBits);
  v.status = result.status;
  return v;
}

//  add new docs array to txn write set

DbStatus txnHelper(Txn *txn, value_t next, TxnStep step) {
  MVCCResult result;
  ObjId docId;
  Handle *docHndl;
  DbMap *docMap;
  Doc *doc;
  Ver *ver;

  switch (next.type) {
    case vt_docId:
      docId.bits = next.idBits;
      docHndl = getDocIdHndl(next.hndlIdx);
      docMap = MapAddr(docHndl);
      doc = getObj(docMap, *(DbAddr *)fetchIdSlot(docMap, docId));
      ver = (Ver *)(doc->doc->base + doc->newestVer);
      break;

    case vt_document:
      doc = mvccDoc(next.document);
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
      result = mvcc_addDocWrToTxn(txn, doc);
      break;
  }

  return result.status;
}

//  fill new Txn from write docIds

value_t fcnWrtTxn(value_t *args, value_t thisVal, environment_t *env) {
  unsigned int max, idx = 0, cnt = 0;
  ObjId txnId;
  value_t v, *next;
  Txn *txn;
  Doc *doc;

  v.bits = vt_status;
  max = vec_cnt(args);

  if (thisVal.type == vt_txnId)
    txnId.bits = thisVal.idBits;
  else
    return v.status = DB_ERROR_badhandle, v;

  txn = mvcc_fetchTxn(txnId);

  for (idx = 0; idx < max; idx++) { 
    if (args[idx].type == vt_docId)
      if ((v.status = txnHelper(txn, args[idx], TxnWrt)))
        return v;
      else
        continue;
    if (args[idx].type == vt_array) {
      for (cnt = 0; cnt < vec_cnt(args[idx].aval->valuePtr); idx++) {
        next = args[idx].aval->valuePtr + cnt;
        if ((v.status = txnHelper(txn, *next, TxnWrt))) 
            return v;
      }
      break;
    }

    if(args[idx].type == vt_document) {
      value_t val;
      next = args + idx;
      doc = mvccDoc(next->document);
        
      val.bits = vt_docId;
      val.idBits = doc->doc->docId.bits;
      val.hndlIdx = doc->doc->hndlIdx;

      if ((v.status = txnHelper(txn, val, TxnWrt))) 
          return v;

    }
  }
   mvcc_releaseTxn(txn);
   return v;
}

// add document read to txn

value_t fcnRdrTxn(value_t *args, value_t thisVal, environment_t *env) {
  MVCCResult result;
  uint32_t idx, argIdx = 0;
  value_t v;
  Txn *txn;
  Ver *ver;
  ObjId txnId;

  v.bits = vt_status;

  if (thisVal.type == vt_txnId)
    txnId.bits = thisVal.idBits;
  else
    return v.status = DB_ERROR_badhandle, v;

  txn = mvcc_fetchTxn(txnId);

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
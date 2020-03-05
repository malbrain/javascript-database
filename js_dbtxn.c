#include "database/mvcc_dbapi.h"
#include "database/mvcc_dbtxn.h"
#include "js.h"
#include "js_db.h"
#include "js_props.h"

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

  if (nest.type == vt_hndl && nest.subType == Hndl_txns)
    nestId.bits = nest.idBits;
  else
    nestId.bits = 0;

  result = mvcc_BeginTxn(params, nestId);

  if ((v.status = result.status))
      return v.bits = vt_status, v;

  v.bits = vt_txn;
  v.idBits = result.bits;
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

value_t fcnDocTxn(value_t *args, value_t thisVal, environment_t *env) {
  int idx, max, off = 0, cnt = 0;
  ObjId docId[1024], txnId;
  Handle *docHndl;
  MVCCResult result;
  DbMap *map;
  value_t v;

  v.bits = vt_status;
  max = vec_cnt(args);

  txnId.bits = thisVal.idBits;

  if (max && args->type == vt_hndl && args->subType == Hndl_docStore) {
    if ((docHndl = bindHandle(args->hndl, Hndl_docStore)))
      map = MapAddr(docHndl);
    else
      return v.status = DB_ERROR_badhandle, v;
  } else
    return v.status = DB_ERROR_badhandle, v;

  do {
    // assemble next batch
    while (cnt < 1024 && ++off < max)
        docId[cnt++].bits = args[off].idBits;
    
    result = mvcc_addDocWrToTxn(txnId, map, docId, cnt, args->hndl);

    if ((v.status = result.status)) 
        goto docXit;
    
    cnt = 0;
   } while (off < max); 

docXit:
   releaseHandle(docHndl, NULL);
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

  if (val.type == vt_txn)
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
    {fcnDocTxn, "write"},
    {NULL, NULL}};

PropVal builtinTxnProp[] = {
    {propTxnCount, "count"},
    {NULL, NULL}
};

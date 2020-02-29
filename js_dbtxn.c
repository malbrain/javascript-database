#include "database/mvcc_dbapi.h"
#include "database/mvcc_dbtxn.h"
#include "js.h"
#include "js_db.h"
#include "js_props.h"

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

value_t fcnDocTxn(value_t *args, value_t thisVal, environment_t *env) {
  int idx, max, off = 0, cnt = 0;
  ObjId docId[1024], txnId;
  MVCCResult result;
  value_t v;

  v.bits = vt_status;
  max = vec_cnt(args);

  txnId.bits = thisVal.idBits;

  if (max && args->type == vt_hndl && args->subType == Hndl_docStore)
   do {
    while (cnt < 1024 && ++off < max)
        docId[cnt++].bits = args[off].idBits;
    
    result = addDocWrToTxn(txnId, args[0].hndl, docId, cnt);
    
    if ((v.status = result.status))
        return v;
    
    cnt = 0;
   } while (off < max); 

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

PropFcn builtinTxnFcns[] = {
    //	{ fcnTxnToString, "toString" },
    {fcnRollbackTxn, "rollback"},
    {fcnCommitTxn, "commit"},
    {fcnDocTxn, "write"},
    {NULL, NULL}};

PropVal builtinTxnProp[] = {
    //	{ propTxnCount, "count" },
    {NULL, NULL}};

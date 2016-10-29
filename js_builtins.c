#include "js.h"

value_t js_beginTxn (uint32_t args, environment_t *env);
value_t js_commitTxn (uint32_t args, environment_t *env);
value_t js_commitTxn (uint32_t args, environment_t *env);
value_t js_rollbackTxn (uint32_t args, environment_t *env);
value_t js_openDatabase (uint32_t args, environment_t *env);
value_t js_listFiles (uint32_t args, environment_t *env);
value_t js_createIndex (uint32_t args, environment_t *env);
value_t js_drop (uint32_t args, environment_t *env);
value_t js_dropIndex (uint32_t args, environment_t *env);
value_t js_createCursor (uint32_t args, environment_t *env);
value_t js_closeCursor (uint32_t args, environment_t *env);
value_t js_nextKey (uint32_t args, environment_t *env);
value_t js_prevKey (uint32_t args, environment_t *env);
value_t js_setSnapshot (uint32_t args, environment_t *env);
value_t js_detachSnapshot (uint32_t args, environment_t *env);
value_t js_getKey (uint32_t args, environment_t *env);
value_t js_getPayload (uint32_t args, environment_t *env);
value_t js_openDocStore (uint32_t args, environment_t *env);
value_t js_insertDocs (uint32_t args, environment_t *env);
value_t js_deleteDoc (uint32_t args, environment_t *env);
value_t js_updateDoc (uint32_t args, environment_t *env);
value_t js_createIterator (uint32_t args, environment_t *env);
value_t js_closeIterator (uint32_t args, environment_t *env);
value_t js_seekDoc (uint32_t args, environment_t *env);
value_t js_nextDoc (uint32_t args, environment_t *env);
value_t js_prevDoc (uint32_t args, environment_t *env);
value_t js_print (uint32_t args, environment_t *env);
value_t js_open (uint32_t args, environment_t *env);
value_t js_close (uint32_t args, environment_t *env);
value_t js_readInt32 (uint32_t args, environment_t *env);
value_t js_readInt64 (uint32_t args, environment_t *env);
value_t js_readString (uint32_t args, environment_t *env);
value_t js_readBSON (uint32_t args, environment_t *env);
value_t js_findDocs (uint32_t args, environment_t *env);
value_t js_tcpListen (uint32_t args, environment_t *env);
value_t js_response (uint32_t args, environment_t *env);
value_t js_makeWeakRef (uint32_t args, environment_t *env);
value_t js_quit (uint32_t args, environment_t *env);
value_t js_mathop (uint32_t args, environment_t *env);
value_t js_miscop (uint32_t args, environment_t *env);
value_t js_eval (uint32_t args, environment_t *env);
value_t js_installProps (uint32_t args, environment_t *env);

value_t js_isNaN (uint32_t args, environment_t *env);
value_t js_parseInt (uint32_t args, environment_t *env);
value_t js_parseFlt (uint32_t args, environment_t *env);
value_t js_objectOp (uint32_t args, environment_t *env);

typedef value_t (*Valuefcnp)(uint32_t args, environment_t *env);

struct {
	Valuefcnp fcn;
	char *name;
} builtIns[] = {
{ js_openDatabase, "jsdb_openDatabase" },
{ js_beginTxn, "jsdb_beginTxn" },
{ js_commitTxn, "jsdb_commitTxn" },
{ js_rollbackTxn, "jsdb_rollbackTxn" },
{ js_installProps, "jsdb_installProps" },
{ js_listFiles, "jsdb_listFiles" },
{ js_createIndex, "jsdb_createIndex" },
{ js_drop, "jsdb_drop" },
{ js_createCursor, "jsdb_createCursor" },
{ js_nextKey, "jsdb_nextKey" },
{ js_prevKey, "jsdb_prevKey" },
{ js_getKey, "jsdb_getKey" },
{ js_openDocStore, "jsdb_openDocStore" },
{ js_insertDocs, "jsdb_insertDocs" },
{ js_deleteDoc, "jsdb_deleteDoc" },
{ js_updateDoc, "jsdb_updateDoc" },
{ js_createIterator, "jsdb_createIterator" },
{ js_seekDoc, "jsdb_seekDoc" },
{ js_nextDoc, "jsdb_nextDoc" },
{ js_prevDoc, "jsdb_prevDoc" },
{ js_findDocs, "jsdb_findDocs" },
{ js_readInt32, "jsdb_readInt32" },
{ js_readInt64, "jsdb_readInt64" },
{ js_readString, "jsdb_readString" },
{ js_readBSON, "jsdb_readBSON" },
{ js_tcpListen, "jsdb_tcpListen" },
{ js_response, "jsdb_response" },
{ js_mathop, "jsdb_mathop" },
{ js_miscop, "jsdb_miscop" },
{ js_objectOp, "jsdb_objectOp" },
{ js_makeWeakRef, "makeWeakRef" },

{ js_isNaN, "isNaN" },
{ js_parseInt, "parseInt" },
{ js_parseFlt, "parseFloat" },
{ js_eval, "eval" },
{ js_print, "print" },
{ js_open, "open" },
{ js_close, "close" },
{ js_quit, "quit" }
};


int builtin (stringNode *name) {
	for (int idx = 0; idx < sizeof(builtIns) / sizeof(*builtIns); idx++)
		if (!strncmp (builtIns[idx].name, name->string, name->hdr->aux))
			  if( !builtIns[idx].name[name->hdr->aux])
				return idx;

	return -1;
}

value_t eval_builtin(Node *a, environment_t *env) {
	fcnCallNode *fc = (fcnCallNode *)a;
	value_t v = (*builtIns[fc->hdr->aux].fcn)(fc->args, env);
	firstNode *fn;
	char *name;
	int idx;

	if (v.type != vt_status || v.status == OK)
		return v;

	idx = fc->hdr->aux;
	name = builtIns[idx].name;
	fn = (firstNode *)env->table;

	fprintf (stderr, "File: %s, Line: %lld, Function: %s Status: %s\n", fn->string, a->lineno, name, strstatus(v.status));
	exit(1); 
}

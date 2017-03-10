#include "js.h"

value_t js_deleteFile(uint32_t args, environment_t *env);
value_t js_openCatalog (uint32_t args, environment_t *env);
value_t js_setOption (uint32_t args, environment_t *env);
value_t js_closeHandle (uint32_t args, environment_t *env);
value_t js_beginTxn (uint32_t args, environment_t *env);
value_t js_commitTxn (uint32_t args, environment_t *env);
value_t js_rollbackTxn (uint32_t args, environment_t *env);
value_t js_openDatabase (uint32_t args, environment_t *env);
value_t js_listFiles (uint32_t args, environment_t *env);
value_t js_createIndex (uint32_t args, environment_t *env);
value_t js_createCursor (uint32_t args, environment_t *env);
value_t js_openDocStore (uint32_t args, environment_t *env);
value_t js_createIterator (uint32_t args, environment_t *env);
value_t js_print (uint32_t args, environment_t *env);
value_t js_open (uint32_t args, environment_t *env);
value_t js_close (uint32_t args, environment_t *env);
value_t js_readInt32 (uint32_t args, environment_t *env);
value_t js_readInt64 (uint32_t args, environment_t *env);
value_t js_readString (uint32_t args, environment_t *env);
value_t js_readBSON (uint32_t args, environment_t *env);
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

value_t js_parseEval (uint32_t args, environment_t *env);
value_t js_json (uint32_t args, environment_t *env);

typedef value_t (*Valuefcnp)(uint32_t args, environment_t *env);

struct {
	Valuefcnp fcn;
	char *name;
} builtIns[] = {
{ js_deleteFile, "jsdb_deleteFile" },
{ js_openCatalog, "jsdb_openCatalog" },
{ js_closeHandle, "jsdb_closeHandle" },
{ js_parseEval, "jsdb_parseEval" },
{ js_beginTxn, "jsdb_beginTxn" },
{ js_commitTxn, "jsdb_commitTxn" },
{ js_rollbackTxn, "jsdb_rollbackTxn" },
{ js_openDatabase, "jsdb_openDatabase" },
{ js_setOption, "jsdb_setOption" },
{ js_installProps, "jsdb_installProps" },
{ js_listFiles, "jsdb_listFiles" },
{ js_createIndex, "jsdb_createIndex" },
{ js_createCursor, "jsdb_createCursor" },
{ js_openDocStore, "jsdb_openDocStore" },
{ js_createIterator, "jsdb_createIterator" },
{ js_readInt32, "jsdb_readInt32" },
{ js_readInt64, "jsdb_readInt64" },
{ js_readString, "jsdb_readString" },
{ js_readBSON, "jsdb_readBSON" },
{ js_tcpListen, "jsdb_tcpListen" },
{ js_response, "jsdb_response" },
{ js_mathop, "jsdb_mathop" },
{ js_miscop, "jsdb_miscop" },
{ js_makeWeakRef, "makeWeakRef" },

{ js_isNaN, "isNaN" },
{ js_parseInt, "parseInt" },
{ js_parseFlt, "parseFloat" },
{ js_eval, "eval" },
{ js_print, "print" },
{ js_open, "open" },
{ js_close, "close" },
{ js_quit, "quit" },
{ js_json, "jsdb_json" }
};


int builtin (string_t *name) {
	for (int idx = 0; idx < sizeof(builtIns) / sizeof(*builtIns); idx++)
		if (!memcmp (builtIns[idx].name, name->val, name->len))
			  if( !builtIns[idx].name[name->len])
				return idx;

	return -1;
}

value_t eval_builtin(Node *a, environment_t *env) {
	fcnCallNode *fc = (fcnCallNode *)a;
	firstNode *fn;
	char *name;
	value_t v;
	int idx;

	v = (*builtIns[fc->hdr->aux].fcn)(fc->args, env);

	if (v.type != vt_status || v.status == OK)
		return v;

	idx = fc->hdr->aux;
	name = builtIns[idx].name;
	fn = findFirstNode(env->table, a - env->table);

	fprintf (stderr, "File: %s, Line: %d, Function: %s Status: %s\n", fn->script, (int)a->lineNo, name, strstatus(v.status));
	exit(1); 
}

#include "jsdb.h"

static bool debug = false;

value_t jsdb_initDatabase (uint32_t args, environment_t *env);
value_t jsdb_createIndex (uint32_t args, environment_t *env);
value_t jsdb_drop (uint32_t args, environment_t *env);
value_t jsdb_dropIndex (uint32_t args, environment_t *env);
value_t jsdb_createCursor (uint32_t args, environment_t *env);
value_t jsdb_closeCursor (uint32_t args, environment_t *env);
value_t jsdb_seekKey (uint32_t args, environment_t *env);
value_t jsdb_nextKey (uint32_t args, environment_t *env);
value_t jsdb_prevKey (uint32_t args, environment_t *env);
value_t jsdb_setSnapshot (uint32_t args, environment_t *env);
value_t jsdb_detachSnapshot (uint32_t args, environment_t *env);
value_t jsdb_getKey (uint32_t args, environment_t *env);
value_t jsdb_getPayload (uint32_t args, environment_t *env);
value_t jsdb_createDocStore (uint32_t args, environment_t *env);
value_t jsdb_findDoc (uint32_t args, environment_t *env);
value_t jsdb_insertDocs (uint32_t args, environment_t *env);
value_t jsdb_deleteDoc (uint32_t args, environment_t *env);
value_t jsdb_updateDoc (uint32_t args, environment_t *env);
value_t jsdb_createIterator (uint32_t args, environment_t *env);
value_t jsdb_closeIterator (uint32_t args, environment_t *env);
value_t jsdb_seekDoc (uint32_t args, environment_t *env);
value_t jsdb_nextDoc (uint32_t args, environment_t *env);
value_t jsdb_prevDoc (uint32_t args, environment_t *env);
value_t jsdb_print (uint32_t args, environment_t *env);
value_t jsdb_open (uint32_t args, environment_t *env);
value_t jsdb_close (uint32_t args, environment_t *env);
value_t jsdb_readInt32 (uint32_t args, environment_t *env);
value_t jsdb_readInt64 (uint32_t args, environment_t *env);
value_t jsdb_readString (uint32_t args, environment_t *env);
value_t jsdb_readBSON (uint32_t args, environment_t *env);
value_t jsdb_findDocs (uint32_t args, environment_t *env);
value_t jsdb_tcpListen (uint32_t args, environment_t *env);
value_t jsdb_response (uint32_t args, environment_t *env);
value_t jsdb_getObject (uint32_t args, environment_t *env);
value_t jsdb_makeWeakRef (uint32_t args, environment_t *env);
value_t jsdb_quit (uint32_t args, environment_t *env);
value_t jsdb_mathop (uint32_t args, environment_t *env);
value_t jsdb_eval (uint32_t args, environment_t *env);
value_t jsdb_installProps (uint32_t args, environment_t *env);

typedef value_t (*Valuefcnp)(uint32_t args, environment_t *env);

struct {
	Valuefcnp fcn;
	char *name;
} builtIns[] = {
{ jsdb_installProps, "jsdb_installProps" },
{ jsdb_initDatabase, "jsdb_initDatabase" },
{ jsdb_createIndex, "jsdb_createIndex" },
{ jsdb_drop, "jsdb_drop" },
{ jsdb_createCursor, "jsdb_createCursor" },
{ jsdb_seekKey, "jsdb_seekKey" },
{ jsdb_nextKey, "jsdb_nextKey" },
{ jsdb_prevKey, "jsdb_prevKey" },
{ jsdb_getKey, "jsdb_getKey" },
{ jsdb_createDocStore, "jsdb_createDocStore" },
{ jsdb_findDoc, "jsdb_findDoc" },
{ jsdb_insertDocs, "jsdb_insertDocs" },
{ jsdb_deleteDoc, "jsdb_deleteDoc" },
{ jsdb_updateDoc, "jsdb_updateDoc" },
{ jsdb_createIterator, "jsdb_createIterator" },
{ jsdb_seekDoc, "jsdb_seekDoc" },
{ jsdb_nextDoc, "jsdb_nextDoc" },
{ jsdb_prevDoc, "jsdb_prevDoc" },
{ jsdb_findDocs, "jsdb_findDocs" },
{ jsdb_readInt32, "jsdb_readInt32" },
{ jsdb_readInt64, "jsdb_readInt64" },
{ jsdb_readString, "jsdb_readString" },
{ jsdb_readBSON, "jsdb_readBSON" },
{ jsdb_tcpListen, "jsdb_tcpListen" },
{ jsdb_response, "jsdb_response" },
{ jsdb_mathop, "jsdb_mathop" },

{ jsdb_getObject, "getObject" },
{ jsdb_makeWeakRef, "makeWeakRef" },
{ jsdb_eval, "eval" },
{ jsdb_print, "print" },
{ jsdb_open, "open" },
{ jsdb_close, "close" },
{ jsdb_quit, "quit" }
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
	return (*builtIns[fc->hdr->aux].fcn)(fc->args, env);
}

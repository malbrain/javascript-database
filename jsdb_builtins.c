#include "jsdb.h"

static bool debug = false;

Status jsdb_initDatabase (uint32_t args, environment_t *env);
Status jsdb_createIndex (uint32_t args, environment_t *env);
Status jsdb_drop (uint32_t args, environment_t *env);
Status jsdb_dropIndex (uint32_t args, environment_t *env);
Status jsdb_createCursor (uint32_t args, environment_t *env);
Status jsdb_closeCursor (uint32_t args, environment_t *env);
Status jsdb_seekKey (uint32_t args, environment_t *env);
Status jsdb_nextKey (uint32_t args, environment_t *env);
Status jsdb_prevKey (uint32_t args, environment_t *env);
Status jsdb_setSnapshot (uint32_t args, environment_t *env);
Status jsdb_detachSnapshot (uint32_t args, environment_t *env);
Status jsdb_getKey (uint32_t args, environment_t *env);
Status jsdb_getPayload (uint32_t args, environment_t *env);
Status jsdb_createDocStore (uint32_t args, environment_t *env);
Status jsdb_findDoc (uint32_t args, environment_t *env);
Status jsdb_insertDocs (uint32_t args, environment_t *env);
Status jsdb_deleteDoc (uint32_t args, environment_t *env);
Status jsdb_updateDoc (uint32_t args, environment_t *env);
Status jsdb_createIterator (uint32_t args, environment_t *env);
Status jsdb_closeIterator (uint32_t args, environment_t *env);
Status jsdb_seekDoc (uint32_t args, environment_t *env);
Status jsdb_nextDoc (uint32_t args, environment_t *env);
Status jsdb_prevDoc (uint32_t args, environment_t *env);
Status jsdb_print (uint32_t args, environment_t *env);
Status jsdb_open (uint32_t args, environment_t *env);
Status jsdb_close (uint32_t args, environment_t *env);
Status jsdb_readInt32 (uint32_t args, environment_t *env);
Status jsdb_readInt64 (uint32_t args, environment_t *env);
Status jsdb_readString (uint32_t args, environment_t *env);
Status jsdb_readBSON (uint32_t args, environment_t *env);
Status jsdb_findDocs (uint32_t args, environment_t *env);
Status jsdb_tcpListen (uint32_t args, environment_t *env);
Status jsdb_response (uint32_t args, environment_t *env);
Status jsdb_getObject (uint32_t args, environment_t *env);
Status jsdb_getTokens (uint32_t args, environment_t *env);
Status jsdb_makeWeakRef (uint32_t args, environment_t *env);
Status jsdb_exit (uint32_t args, environment_t *env);

typedef Status (*Statfcnp)(uint32_t args, environment_t *env);

struct {
	Statfcnp fcn;
	char *name;
} builtIns[] = {
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

{ jsdb_getObject, "getObject" },
{ jsdb_getTokens, "getTokens" },
{ jsdb_makeWeakRef, "makeWeakRef" },
{ jsdb_print, "print" },
{ jsdb_open, "open" },
{ jsdb_close, "close" },
{ jsdb_exit, "exit" }
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
	value_t v;

	v.bits = vt_status;
	v.status = (*builtIns[fc->hdr->aux].fcn)(fc->args, env);
	return v;
}

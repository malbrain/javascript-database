#include "js.h"

value_t js_deleteFile(uint32_t args, environment_t *env);
value_t js_openCatalog (uint32_t args, environment_t *env);
value_t js_setOption (uint32_t args, environment_t *env);
value_t js_closeHandle (uint32_t args, environment_t *env);
value_t js_beginTxn (uint32_t args, environment_t *env);
value_t js_openDatabase (uint32_t args, environment_t *env);
value_t js_listFiles (uint32_t args, environment_t *env);
value_t js_createIndex (uint32_t args, environment_t *env);
value_t js_createCursor (uint32_t args, environment_t *env);
value_t js_openDocStore (uint32_t args, environment_t *env);
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
value_t js_eval (uint32_t args, environment_t *env);
value_t js_installProps (uint32_t args, environment_t *env);

value_t js_isNaN (uint32_t args, environment_t *env);
value_t js_parseInt (uint32_t args, environment_t *env);
value_t js_parseFlt (uint32_t args, environment_t *env);

value_t js_parseEval (uint32_t args, environment_t *env);
value_t js_json (uint32_t args, environment_t *env);
value_t js_fromCharCode (uint32_t args, environment_t *env);
value_t js_newDate (uint32_t args, environment_t *env);

typedef value_t (*Valuefcnp)(uint32_t args, environment_t *env);

struct {
	Valuefcnp fcn;
	char *name;
	uint32_t str[1];
	char string[25];
	uint32_t frameidx;
} builtIns[] = {
    {js_deleteFile, "jsdb_deleteFile"},
    {js_openCatalog, "jsdb_openCatalog"},
    {js_closeHandle, "jsdb_closeHandle"},
    {js_parseEval, "jsdb_parseEval"},
    {js_beginTxn, "jsdb_beginTxn"},
    {js_openDatabase, "jsdb_openDatabase"},
    {js_setOption, "jsdb_setOption"},
    {js_installProps, "jsdb_installProps"},
    {js_listFiles, "jsdb_listFiles"},
    {js_createIndex, "jsdb_createIndex"},
    {js_createCursor, "jsdb_createCursor"},
    {js_openDocStore, "jsdb_openDocStore"},
    {js_readInt32, "jsdb_readInt32"},
    {js_readInt64, "jsdb_readInt64"},
    {js_readString, "jsdb_readString"},
    {js_readBSON, "jsdb_readBSON"},
    {js_tcpListen, "jsdb_tcpListen"},
    {js_response, "jsdb_response"},
    {js_mathop, "jsdb_mathop"},
    {js_makeWeakRef, "makeWeakRef"},

    {js_isNaN, "isNaN"},
    {js_parseInt, "parseInt"},
    {js_parseFlt, "parseFloat"},
    {js_eval, "eval"},
    {js_print, "print"},
    {js_open, "open"},
    {js_close, "close"},
    {js_quit, "quit"},
    {js_json, "jsdb_json"},
    {js_fromCharCode, "jsdb_fromCharCode"},
    {js_newDate, "jsdb_newDate"},
};

char *strncpy_x(char *dest, const char *src, size_t n) {
  size_t i;

  for (i = 0; i < n && src[i] != '\0'; i++) 
      dest[i] = src[i];

  while (i < n) 
      dest[i++] = '\0';

  return dest;
}

int builtinFcns(symtab_t *symbols) {
	int idx;

	for (idx = 0; idx < sizeof(builtIns) / sizeof(*builtIns); idx++) {
		builtIns[idx].str[0] = (uint32_t)strnlen(builtIns[idx].name, sizeof(builtIns[idx].string));
		strncpy_x(builtIns[idx].string, builtIns[idx].name, sizeof(builtIns[idx].string));
		builtIns[idx].frameidx = insertSymbol((string_t *)builtIns[idx].str, symbols, false);
	}
	return idx;
}

void installBuiltIns(frame_t *frame, environment_t *env) {
	int idx;
	value_t val;

	val.bits = vt_builtin;

	for (idx = 0; idx < sizeof(builtIns) / sizeof(*builtIns); idx++) {
		val.builtIn = idx;
		frame->values[builtIns[idx].frameidx] = val;
	}
}

value_t execbuiltin(fcnCallNode *fc, value_t fcn, environment_t *env) {
	firstNode *fn;
	char *name;
	value_t v;
	int idx;

	v = (*builtIns[fcn.builtIn].fcn)(fc->args, env);

	if (v.type != vt_status || v.status == OK)
		return v;

	idx = fc->hdr->aux;
	name = builtIns[idx].name;
	fn = findFirstNode(env->table, (uint32_t)(fc->hdr - env->table));

	fprintf (stderr, "File: %s, Line: %d, Function: %s Status: %s\n", fn->script, (int)fc->hdr->lineNo, name, strstatus(v.status));
	exit(1); 
}

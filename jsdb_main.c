#include "jsdb.h"
#include "jsdb_db.h"
#include "jsdb_eval.h"

#include "jsdb.tab.h"
#include "jsdb.lex.h"

void memInit();

dispatchFcn dispatchTable[node_MAX];

uint32_t insertSymbol(char *name, uint32_t len, symtab_t *symtab);
symbol_t *lookupSymbol(char *name, uint32_t len, symtab_t *symtab);

double getCpuTime(int);

void loadNGo(char *name, symtab_t *systemSymbols, frame_t *system, value_t args, FILE *strm) {
	double start, elapsed;
	fcnDeclNode *topLevel;
	environment_t env[1];
	uint32_t firstNode;
	closure_t *closure;
	parseData pd[1];
	frame_t *frame;
	stringNode *sn;
	int k;

	// initialize

	memset(pd, 0, sizeof(parseData));

	yylex_init(&pd->scaninfo);
	yyset_debug(1, pd->scaninfo);
	pd->script = name;
	pd->lineno = 1;

	// occupy table slot zero with endlist and script name

	firstNode = newNode(pd, node_none, sizeof(stringNode) + strlen(name), false);
	sn = (stringNode  *)(pd->table + firstNode);
	sn->hdr->aux = strlen(name);

	memcpy (sn->string, name, sn->hdr->aux);

	if ( (k = yyparse(pd->scaninfo, pd)) ) {
		if (k==1)
			printf("Parse error\n");
		else if (k==2)
			printf("Parser exhausted memory\n");
		exit(1);
	}

	// cleanup

	yylex_destroy(pd->scaninfo);

	// initialize node zero

	topLevel = (fcnDeclNode *)(pd->table + newNode(pd, node_fcndef, sizeof(fcnDeclNode), true));
	topLevel->body = pd->beginning;

	// hoist and assign var decls

	compileSymbols(topLevel, pd->table, systemSymbols, 1);

	frame = jsdb_alloc(sizeof(value_t) * topLevel->nsymbols + sizeof(frame_t), true);
	frame->count = topLevel->nsymbols;
	frame->arguments = args;

	if (strm) {
		fwrite (&pd->tablenext, sizeof(pd->tablenext), 1, strm);
		fwrite (pd->table, sizeof(Node), pd->tablenext, strm);
	}

	start = getCpuTime(0);

	closure = jsdb_alloc(sizeof(closure_t) + sizeof(valueframe_t), true);
	closure->frames[0] = system;
	closure->table = pd->table;
	closure->count = 1;

	env->table = pd->table;
	env->closure = closure;
	env->topFrame = frame;

	installFcns(topLevel->fcn, env);
	dispatch(pd->beginning, env);

	elapsed = getCpuTime(0) - start;
	fprintf (stderr, "%s real %dm%.6fs\n", name, (int)(elapsed/60), elapsed - (int)(elapsed/60)*60);
}
 
value_t *builtinObj[vt_MAX];

void installValue(char *name, symtab_t *symtab) {
	uint32_t idx = insertSymbol(name, strlen(name), symtab);
}

void installProps(char *obj, symtab_t *symtab, frame_t *frame, valuetype_t type) {
	symbol_t *sym = lookupSymbol(obj, strlen(obj), symtab);

	if (sym)
		builtinObj[type] = frame->values + sym->frameidx;
	else
		fprintf(stderr, "Unable to find builtin Object %s\n", obj);
}

void usage(char* cmd) {
	printf("%s scr1.js scr2.js ... -- arg1 arg2 ...\n", cmd);
}

int main(int argc, char* argv[])
{
	symtab_t systemSymbols[1];
	char **scripts = NULL;
	bool argmode = false;
	value_t val, args;
	char *name = NULL;
	FILE *strm = NULL;
	frame_t *system;
	array_t aval[1];

	memset(aval, 0, sizeof(aval));
	memInit();

	printf("sizeof value_t = %ld\n",  sizeof(value_t));
	printf("sizeof Node = %ld\n",  sizeof(Node));

	memset (systemSymbols, 0, sizeof(symtab_t));

	installValue("Object",		systemSymbols);
	installValue("Function",	systemSymbols);
	installValue("Boolean",		systemSymbols);
	installValue("Symbol",		systemSymbols);
	installValue("Error",		systemSymbols);
	installValue("EvalError",	systemSymbols);
	installValue("InternalError",	systemSymbols);
	installValue("RangeError",	systemSymbols);
	installValue("ReferenceError",	systemSymbols);
	installValue("SyntaxError",	systemSymbols);
	installValue("TypeError",	systemSymbols);
	installValue("URIError",	systemSymbols);
	installValue("Number",		systemSymbols);
	installValue("Math",		systemSymbols);
	installValue("Db",			systemSymbols);
	installValue("Date",		systemSymbols);
	installValue("String",		systemSymbols);
	installValue("RegExp",		systemSymbols);
	installValue("Array",		systemSymbols);
	installValue("Int8Array",	systemSymbols);
	installValue("Uint8Array",	systemSymbols);
	installValue("Int16Array",	systemSymbols);
	installValue("Uint16Array",	systemSymbols);
	installValue("Int32Array",	systemSymbols);
	installValue("Uint32Array",	systemSymbols);
	installValue("Float32Array",	systemSymbols);
	installValue("Float64Array",	systemSymbols);
	installValue("Map",			systemSymbols);
	installValue("Set",			systemSymbols);
	installValue("WeakMap",		systemSymbols);
	installValue("WeakSet",		systemSymbols);
	installValue("ArrayBuffer",	systemSymbols);
	installValue("SharedArrayBuffer",	systemSymbols);
	installValue("Atomics",		systemSymbols);
	installValue("DataView",	systemSymbols);
	installValue("JSON",		systemSymbols);

	for (int i = 0; i < node_MAX; i++)
		dispatchTable[i] = eval_badop;

	dispatchTable[node_endlist] = eval_list;
	dispatchTable[node_fcndef] = eval_noop;

	dispatchTable[node_dowhile] = eval_dowhile;
	dispatchTable[node_builtin] = eval_builtin;
	dispatchTable[node_fcncall] = eval_fcncall;
	dispatchTable[node_fcnexpr] = eval_fcnexpr;
	dispatchTable[node_lookup] = eval_lookup;
	dispatchTable[node_access] = eval_access;
	dispatchTable[node_return] = eval_return;
	dispatchTable[node_assign] = eval_assign;
	dispatchTable[node_string] = eval_string;
	dispatchTable[node_ifthen] = eval_ifthen;
	dispatchTable[node_typeof] = eval_typeof;
	dispatchTable[node_ternary] = eval_tern;
	dispatchTable[node_array] = eval_array;
	dispatchTable[node_while] = eval_while;
	dispatchTable[node_forin] = eval_forin;
	dispatchTable[node_incr] = eval_incr;
	dispatchTable[node_list] = eval_list;
	dispatchTable[node_math] = eval_math;
	dispatchTable[node_enum] = eval_enum;
	dispatchTable[node_neg] = eval_neg;
	dispatchTable[node_var] = eval_var;
	dispatchTable[node_ref] = eval_ref;
	dispatchTable[node_for] = eval_for;
	dispatchTable[node_obj] = eval_obj;
	dispatchTable[node_num] = eval_num;
	dispatchTable[node_lor] = eval_lor;
	dispatchTable[node_land] = eval_land;

	//  install system frame with vt_undef variable values

	system = jsdb_alloc(sizeof(value_t) * vec_count(systemSymbols->entries) + sizeof(frame_t), true);
	system->count = vec_count(systemSymbols->entries);
	incrFrameCnt(system);

	installProps ("Object",	systemSymbols, system, vt_object);
	installProps ("String",	systemSymbols, system, vt_string);
	installProps ("Array",	systemSymbols, system, vt_array);
	installProps ("Number",	systemSymbols, system, vt_int);
	installProps ("Number",	systemSymbols, system, vt_dbl);
	installProps ("Date",	systemSymbols, system, vt_date);
	installProps ("Boolean",	systemSymbols, system, vt_bool);
	installProps ("Function",	systemSymbols, system, vt_closure);

	name = argv[0];
	args.bits = vt_array;
	args.aval = aval;
 	argc--;
	argv++;

	while (argc > 0 ) {
	  if (!argmode && argv[0][0] == '-') {
		switch (argv[0][1]) {
		case '-':
			argmode = true;
			break;
		case 'w':
			strm = fopen(argv[1], "wb");
	 		argc--;
	 		argv++;
			break;
		default:
			usage(name);
			exit(-1);
		}
	  } else if (argmode) {
		val.bits = vt_string;
		val.str = argv[0];
		val.aux = strlen(argv[0]);
		vec_push(aval->values, val);
	  } else
		vec_push(scripts, argv[0]);

	 argc--;
	 argv++;
	}

	for (int idx = 0; idx < vec_count(scripts); idx++) {
		if (freopen(scripts[idx],"r",stdin))
			loadNGo(scripts[idx], systemSymbols, system, args, strm);
		else
			fprintf(stderr, "unable to open %s, errno = %d\n", scripts[idx], errno);
	}

	return 0;
}


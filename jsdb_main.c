#include "jsdb.h"
#include "jsdb_eval.h"

dispatchFcn dispatchTable[node_MAX];

void loadNGo(char *name, symtab_t *systemSymbols, frame_t *system, value_t *args, FILE *strm) {
	valueframe_t *framev = NULL;
	fcnDeclNode *topLevel;
	environment_t env[1];
	uint32_t firstNode;
	parseData pd[1];
	frame_t *frame;
	stringNode *sn;
	int k;

	vec_push(framev, system);
	incrFrameCnt(system);

	// initialize

	memset(pd, 0, sizeof(parseData));
	yylex_init(&pd->scaninfo);
	yyset_debug(1, pd->scaninfo);
	pd->script = name;
	pd->lineno = 1;

	// occupy table slot zero with endlist and script name

	firstNode = newNode(pd, node_endlist, sizeof(stringNode) + strlen(name), false);
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
	frame->args->array = args;
	frame->name = 0;

	vec_push(framev, frame);

	if (strm) {
		fwrite (&pd->tablenext, sizeof(pd->tablenext), 1, strm);
		fwrite (pd->table, sizeof(Node), pd->tablenext, strm);
	}

	installFcns(topLevel->fcn, pd->table, frame);

	env->table = pd->table;
	env->framev = framev;

	dispatch(pd->beginning, env);
}
 
void installValue(char *name, value_t val, symtab_t *symtab) {
	uint32_t idx = insertSymbol(name, strlen(name), symtab, 0);
	symtab->entries[idx].value = val;
}

void usage(char* cmd) {
	printf("%s [-f fname]\n", cmd);
}

double getCpuTime(int);

int main(int argc, char* argv[])
{
	symtab_t systemSymbols[1];
	value_t val, *args, undef;
	char **scripts = NULL;
	double start, elapsed;
	bool argmode = false;
	char *name = NULL;
	FILE *strm = NULL;
	frame_t *system;

	printf("sizeof value_t = %ld\n",  sizeof(value_t));
	printf("sizeof Node = %ld\n",  sizeof(Node));

	memset (systemSymbols, 0, sizeof(symtab_t));

	undef.bits = vt_undef;

	installValue("Map", undef, systemSymbols);
	installValue("Function", undef, systemSymbols);
	installValue("Boolean", undef, systemSymbols);
	installValue("Symbol", newObject(), systemSymbols);
	installValue("Error", newObject(), systemSymbols);
	installValue("EvalError", newObject(), systemSymbols);
	installValue("InternalError", newObject(), systemSymbols);
	installValue("RangeError", newObject(), systemSymbols);
	installValue("ReferenceError", newObject(), systemSymbols);
	installValue("SyntaxError", newObject(), systemSymbols);
	installValue("TypeError", newObject(), systemSymbols);
	installValue("URIError", newObject(), systemSymbols);
	installValue("Number", newObject(), systemSymbols);
	installValue("Math", newObject(), systemSymbols);
	installValue("Date", newObject(), systemSymbols);
	installValue("String", newObject(), systemSymbols);
	installValue("RegExp", newObject(), systemSymbols);
	installValue("Array", newObject(), systemSymbols);
	installValue("Int8Array", newObject(), systemSymbols);
	installValue("Uint8Array", newObject(), systemSymbols);
	installValue("Int16Array", newObject(), systemSymbols);
	installValue("Uint16Array", newObject(), systemSymbols);
	installValue("Int32Array", newObject(), systemSymbols);
	installValue("Uint32Array", newObject(), systemSymbols);
	installValue("Float32Array", newObject(), systemSymbols);
	installValue("Float64Array", newObject(), systemSymbols);
	installValue("Map", newObject(), systemSymbols);
	installValue("Set", newObject(), systemSymbols);
	installValue("WeakMap", newObject(), systemSymbols);
	installValue("WeakSet", newObject(), systemSymbols);
	installValue("ArrayBuffer", newObject(), systemSymbols);
	installValue("SharedArrayBuffer", newObject(), systemSymbols);
	installValue("Atomics", newObject(), systemSymbols);
	installValue("DataView", newObject(), systemSymbols);
	installValue("JSON", newObject(), systemSymbols);

	for (int i = 0; i < node_MAX; i++)
		dispatchTable[i] = eval_badop;

	dispatchTable[node_endlist] = eval_noop;
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
	dispatchTable[node_array] = eval_array;
	dispatchTable[node_while] = eval_while;
	dispatchTable[node_list] = eval_list;
	dispatchTable[node_math] = eval_math;
	dispatchTable[node_enum] = eval_enum;
	dispatchTable[node_neg] = eval_neg;
	dispatchTable[node_var] = eval_var;
	dispatchTable[node_ref] = eval_ref;
	dispatchTable[node_for] = eval_for;
	dispatchTable[node_obj] = eval_obj;
	dispatchTable[node_num] = eval_num;

	//  install system frame variables

	system = jsdb_alloc(sizeof(value_t) * vec_count(systemSymbols->entries) + sizeof(frame_t), true);
	system->count = vec_count(systemSymbols->entries);
	system->name = 0;

	for (int idx = 0; idx < system->count; idx++) {
		system->values[idx] = systemSymbols->entries[idx].value;
		incrRefCnt(system->values[idx]);
	}

	start = getCpuTime(0);
	installProps ();

	name = argv[0];
	args = NULL;
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
		vec_push(args, val);
	  } else
		vec_push(scripts, argv[0]);

	  argc--;
	  argv++;
	}

	for (int idx = 0; idx < vec_count(scripts); idx++) {
		freopen(scripts[idx],"r",stdin);
		loadNGo(scripts[idx], systemSymbols, system, args, strm);
	}

	elapsed = getCpuTime(0) - start;
	fprintf (stderr, "real %dm%.6fs\n", (int)(elapsed/60), elapsed - (int)(elapsed/60)*60);
	return 0;
}


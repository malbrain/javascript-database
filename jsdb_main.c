#include "jsdb.h"
#include "jsdb_db.h"
#include "jsdb_eval.h"

#include "jsdb.tab.h"
#include "jsdb.lex.h"

void memInit();

dispatchFcn dispatchTable[node_MAX];
value_t builtinObj[vt_MAX];

uint32_t insertSymbol(char *name, uint32_t len, symtab_t *symtab);
symbol_t *lookupSymbol(char *name, uint32_t len, symtab_t *symtab);

double getCpuTime(int);

Node *loadScript(char *name, symtab_t *globalSymbols, FILE *strm) {
	fcnDeclNode topLevel[1];
	uint32_t prevSymbols;
	parseData pd[1];
	firstNode *fn;
	int k;

	// initialize

	memset(pd, 0, sizeof(parseData));

	yylex_init(&pd->scaninfo);
	yyset_debug(1, pd->scaninfo);
	pd->script = name;
	pd->lineno = 1;

	// occupy table slot zero with endlist and script name

	newNode(pd, node_first, sizeof(firstNode) + strlen(name) + 1, false);

	fn = (firstNode *)pd->table;
	fn->hdr->aux = strlen(name);
	strcpy (fn->string, name);

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

	fn = (firstNode *)pd->table; // table might have been realloced
	fn->begin = pd->beginning;

	memset (topLevel, 0, sizeof(fcnDeclNode));
	topLevel->body = pd->beginning;

	// hoist and assign var decls

	compileSymbols(topLevel, pd->table, globalSymbols, 0);

	if (strm) {
		fwrite (&pd->tablenext, sizeof(pd->tablenext), 1, strm);
		fwrite (pd->table, sizeof(Node), pd->tablenext, strm);
	}

	return pd->table;
}

void execScript(Node *table, frame_t *frame, symtab_t *globalSymbols) {
	firstNode *fn = (firstNode *)table;
	double start, elapsed;
	environment_t env[1];
	closure_t closure;

	memset (&closure, 0, sizeof(closure_t));
	closure.table = table;
	start = getCpuTime(0);

	memset (env, 0, sizeof(environment_t));
	env->closure = &closure;
	env->topFrame = frame;
	env->table = table;

	dispatch(fn->begin, env);

	elapsed = getCpuTime(0) - start;
	fprintf (stderr, "%s real %dm%.6fs\n", fn->string, (int)(elapsed/60), elapsed - (int)(elapsed/60)*60);
}
 
void usage(char* cmd) {
	printf("%s scr1.js scr2.js ... -- arg1 arg2 ...\n", cmd);
}


int main(int argc, char* argv[])
{
	symtab_t globalSymbols[1];
	Node **scrTables = NULL;
	char **scripts = NULL;
	bool argmode = false;
	value_t val, args;
	char *name = NULL;
	FILE *strm = NULL;
	array_t aval[1];
	frame_t *frame;

	memset(aval, 0, sizeof(aval));
	memInit();

	printf("sizeof value_t = %lld\n",  sizeof(value_t));
	printf("sizeof Node = %lld\n",  sizeof(Node));

	memset (globalSymbols, 0, sizeof(symtab_t));

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
		if (freopen(scripts[idx],"r",stdin)) {
			fprintf(stderr, "Compiling: %s\n", scripts[idx]);
			Node *tbl = loadScript(scripts[idx], globalSymbols, strm);
			vec_push(scrTables, tbl);
		} else
			fprintf(stderr, "unable to open %s, errno = %d\n", scripts[idx], errno);
	}

	frame = jsdb_alloc(sizeof(value_t) * vec_count(globalSymbols->entries) + sizeof(frame_t), true);

	frame->count = vec_count(globalSymbols->entries);
	frame->arguments = args;

	for (int idx = 0; idx < vec_count(scrTables); idx++)
		execScript(scrTables[idx], frame, globalSymbols);

	return 0;
}


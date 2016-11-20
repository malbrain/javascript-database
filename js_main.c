#include "js.h"
#include "js_eval.h"

#include "js.tab.h"
#include "js.lex.h"

#ifndef _WIN32
#define fopen_s(file, path, mode) ((*file = fopen(path, mode)) ? 0 : errno)
#define freopen_s(dummy, path, mode, file) (((*dummy) = freopen(path, mode,file)) ? 0 : errno)
#define strerror_s(buf,siz,err) (strerror_r(err,buf,siz))
#endif

bool MathNums;	//	interpret numbers as doubles
bool noQuotes;	//	print strings without quotes

void memInit(void);

dispatchFcn dispatchTable[node_MAX];
value_t builtinObj[vt_MAX];

uint32_t insertSymbol(char *name, uint32_t len, symtab_t *symtab);
symbol_t *lookupSymbol(char *name, uint32_t len, symtab_t *symtab);

double getCpuTime(int);

Node *loadScript(char *name, symtab_t *globalSymbols, FILE *strm) {
	fcnDeclNode topLevel[1];
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
	memcpy (fn->string, name, fn->hdr->aux);
	fn->string[fn->hdr->aux] = 0;

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

	memset (topLevel, 0, sizeof(fcnDeclNode));
	topLevel->body = pd->beginning;

	// hoist and assign var decls

	compileSymbols(topLevel, pd->table, globalSymbols, 0);

	fn = (firstNode *)pd->table; // table might have been realloced
	fn->fcnChain = topLevel->fcn;
	fn->begin = pd->beginning;

	if (strm) {
		fwrite (&pd->tablenext, sizeof(pd->tablenext), 1, strm);
		fwrite (pd->table, sizeof(Node), pd->tablenext, strm);
	}

	return pd->table;
}

void usage(char* cmd) {
	printf("%s scr1.js scr2.js ... -- arg1 arg2 ...\n", cmd);
}


int main(int argc, char* argv[]) {
	symtab_t globalSymbols[1];
	environment_t env[1];
	closure_t *closure;
	value_t val, args;
	char *name = NULL;
	FILE *strm = NULL;
	char errmsg[1024];
	Node **scrTables;
	array_t aval[1];
	frame_t *frame;
	int err, idx;
	int nScripts;

	memset(aval, 0, sizeof(aval));

	memInit();

	printf("sizeof value_t = %d\n",  (int)sizeof(value_t));
	printf("sizeof Node = %d\n",  (int)sizeof(Node));

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

	while (--argc > 0 && (++argv)[0][0] == '-') {
		if (!strcmp(argv[0], "-Math"))
			MathNums = true;
		else if (!strcmp(argv[0], "-NoQuotes"))
			noQuotes = true;
		else if(!strcmp(argv[0], "-Write") && argc > 1)	{
			if((err = fopen_s(&strm, argv[1], "wb"))) {
			  strerror_s(errmsg, sizeof(errmsg), err);
			  fprintf(stderr, "Error: fopen failed on '%s' err:%d %s\n", argv[1], err, errmsg);
			  strm = NULL;
			}

			// sluff the file name
	 		argc--;
	 		argv++;

		} else {
			usage(name);
			exit(-1);
		}
	}

	//	compile the scripts on the command line
	//	into the scrTables array

	scrTables = js_alloc(argc * sizeof(Node *), true);

	for (idx = 0; idx < argc; idx++) {
	  FILE *dummy;

	  if(argv[idx][0] == '-' && argv[idx][1] == '-' && ++idx)
		break;

	  if((err = freopen_s(&dummy, argv[idx],"r",stdin))) {
		strerror_s(errmsg, sizeof(errmsg), err);
		fprintf(stderr, "Error: unable to open '%s' error: %d: %s\n", argv[idx], err, errmsg);
	  } else {
		fprintf(stderr, "Compiling: %s\n", argv[idx]);
		Node *tbl = loadScript(argv[idx], globalSymbols, strm);
		scrTables[idx] = tbl;
	  }
	}

	nScripts = idx;

	//	assemble user arguments into
	//	the argument's array

	while(idx < argc) {
	  val.bits = vt_string;
	  val.string = argv[idx];
	  val.aux = strlen(argv[idx++]);
	  vec_push(aval->values, val);
	}

	//  allocate the global frame

	frame = js_alloc(sizeof(value_t) * vec_count(globalSymbols->entries) + sizeof(frame_t), true);
	closure = js_alloc(sizeof(closure_t) + sizeof(valueframe_t), true);

	memset (env, 0, sizeof(environment_t));

	frame->count = vec_count(globalSymbols->entries);
	frame->arguments = args;

	closure->frames[0] = frame;
	closure->count = 1;

	//	install top level function definitions

	for (idx = 0; idx < nScripts; idx++) {
	  if (( env->table = scrTables[idx])) {
		firstNode *fn = (firstNode *)env->table;
		closure->table = env->table;
		env->table = env->table;
		env->topFrame = frame;
		installFcns(fn->fcnChain, env);
	  }
	}

	//	run the scripts

	for (idx = 0; idx < nScripts; idx++) {
	  if (( env->table = scrTables[idx])) {
		firstNode *fn = (firstNode *)env->table;
		double start, elapsed;

		closure->table = env->table;

		start = getCpuTime(0);
		env->topFrame = frame;
		env->closure = closure;

		dispatch(fn->begin, env);

		elapsed = getCpuTime(0) - start;
		fprintf (stderr, "%s real %dm%.6fs\n", fn->string, (int)(elapsed/60), elapsed - (int)(elapsed/60)*60);
	  }
	}

	//	TODO: delete objects in the global frame

	return 0;
}


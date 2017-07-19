#include "js.h"
#include "js_malloc.h"
#include "js_props.h"
#include "js_eval.h"
#include "js_db.h"

#include "js.tab.h"
#include "js.lex.h"

#ifndef _WIN32
#define fopen_s(file, path, mode) ((*file = fopen(path, mode)) ? 0 : errno)
#define freopen_s(dummy, path, mode, file) (((*dummy) = freopen(path, mode,file)) ? 0 : errno)
#define strerror_s(buf,siz,err) (strerror_r(err,buf,siz))
#endif

int clusterId = 0;		//  used for db clusters

bool mathNums;	//	interpret numbers as doubles
bool parseDebug;
bool mallocDebug;
bool hoistDebug;
bool evalDebug;
bool debug;

void memInit(void);

dispatchFcn dispatchTable[node_MAX];
symtab_t globalSymbols;

void loadScript(parseData *pd) {
	uint32_t first;
	firstNode *fn;
	int k;

	yylex_init(&pd->scanInfo);
	yyset_debug(1, pd->scanInfo);

	// occupy next table slot with endlist and script name

	first = newNode(pd, node_first, (uint32_t)sizeof(firstNode) + (uint32_t)strlen(pd->script) + 1, true);
	pd->lineNo = 1;

	fn = (firstNode *)(pd->table + first);
	fn->hdr->aux = (uint32_t)strlen(pd->script);
	memcpy (fn->script, pd->script, fn->hdr->aux);
	fn->script[fn->hdr->aux] = 0;

	if((k = yyparse(pd->scanInfo, pd))) {
		if (k==1)
			fprintf(stderr, "Parse error\n");
		else if (k==2)
			fprintf(stderr, "Parser exhausted memory\n");
		exit(1);
	}

	// cleanup

	yylex_destroy(pd->scanInfo);

	fn = (firstNode *)(pd->table + first); // might have been realloced
	fn->moduleSize = pd->tableNext - first;
	fn->begin = pd->beginning;
}

void usage(char* cmd) {
	fprintf(stderr, "%s scr1.js scr2.js ... -- arg1 arg2 ...\n", cmd);
}


int main(int argc, char* argv[]) {
	value_t val, args;
	char *name = NULL;
	FILE *strm = NULL;
	char errmsg[1024];
	parseData pd[1];
	array_t aval;
	int err, idx;
	int nScripts;

	memset(pd, 0, sizeof(parseData));
	memset(&aval, 0, sizeof(aval));

	memInit();

	for (int i = 0; i < node_MAX; i++)
		dispatchTable[i] = eval_badop;

	dispatchTable[node_first] = eval_noop;
	dispatchTable[node_fcndef] = eval_noop;
	dispatchTable[node_endlist] = eval_list;

	dispatchTable[node_opassign] = eval_opassign;
	dispatchTable[node_dowhile] = eval_dowhile;
	dispatchTable[node_builtin] = eval_builtin;
	dispatchTable[node_fcncall] = eval_fcncall;
	dispatchTable[node_fcnexpr] = eval_fcnexpr;
	dispatchTable[node_access] = eval_access;
	dispatchTable[node_return] = eval_return;
	dispatchTable[node_assign] = eval_assign;
	dispatchTable[node_string] = eval_string;
	dispatchTable[node_ifthen] = eval_ifthen;
	dispatchTable[node_typeof] = eval_typeof;
	dispatchTable[node_ternary] = eval_tern;
	dispatchTable[node_block] = eval_block;
	dispatchTable[node_array] = eval_array;
	dispatchTable[node_while] = eval_while;
	dispatchTable[node_forin] = eval_forin;
	dispatchTable[node_incr] = eval_incr;
	dispatchTable[node_list] = eval_list;
	dispatchTable[node_math] = eval_math;
	dispatchTable[node_enum] = eval_enum;
	dispatchTable[node_neg] = eval_neg;
	dispatchTable[node_var] = eval_var;
	dispatchTable[node_for] = eval_for;
	dispatchTable[node_obj] = eval_obj;
	dispatchTable[node_num] = eval_num;
	dispatchTable[node_lor] = eval_lor;
	dispatchTable[node_land] = eval_land;

	name = argv[0];
	args.bits = vt_array;
	args.addr = &aval;

	if (argc < 2)
		usage(name);

	while (--argc > 0 && (++argv)[0][0] == '-') {
		if (!strcmp(argv[0], "-Math"))
			mathNums = true;
		else if (!memcmp(argv[0], "-ClusterId=", 11))
			clusterId = atoi(argv[0] + 11);
		else if (!strcmp(argv[0], "-ParseDebug"))
			parseDebug = true;
		else if (!strcmp(argv[0], "-MallocDebug"))
			mallocDebug = true;
		else if (!strcmp(argv[0], "-Debug"))
			debug = true;
		else if (!strcmp(argv[0], "-HoistDebug"))
			hoistDebug = true;
		else if (!strcmp(argv[0], "-EvalDebug"))
			evalDebug = true;
		else if(!strcmp(argv[0], "-Write") && argc > 1)	{
			if((err = fopen_s(&strm, argv[1], "wb"))) {
			  strerror_s(errmsg, sizeof(errmsg), err);
			  fprintf(stderr, "Error: fopen failed on '%s' err:%d %s\n", argv[1], err, errmsg);
			  strm = NULL;
			}

			// sluff the file name
	 		argc--;
	 		argv++;
		} else
			fprintf(stderr, "Unknown option %s ignored\n", argv[0] + 1);
	}

	//	compile the scripts on the command line

	for (nScripts = 0; nScripts < argc; nScripts++) {
	  FILE *dummy;

	  if(argv[nScripts][0] == '-' && argv[nScripts][1] == '-' && ++nScripts)
		break;

	  if((err = freopen_s(&dummy, argv[nScripts],"r",stdin))) {
		strerror_s(errmsg, sizeof(errmsg), err);
		fprintf(stderr, "Error: unable to open '%s' error: %d: %s\n", argv[nScripts], err, errmsg);
	  } else {
		if (debug)
			fprintf(stderr, "Compiling: %s\n", argv[nScripts]);
		pd->script = argv[nScripts];
		loadScript(pd);
		fclose(dummy);
	  }
	}

	if (strm) {
		fwrite (pd->table, sizeof(Node), pd->tableNext, strm);
		fclose(strm);
	}

	if (debug) {
		fprintf(stderr, "sizeof value_t = %d\n",  (int)sizeof(value_t));
		fprintf(stderr, "sizeof Node = %d\n",  (int)sizeof(Node));
		fprintf(stderr, "sizeof Object = %d\n",  (int)sizeof(struct Object));
		fprintf(stderr, "sizeof Array = %d\n",  (int)sizeof(struct Array));
		fprintf(stderr, "sizeof DbObject = %d\n",  (int)sizeof(struct DbObject));
		fprintf(stderr, "sizeof DbArray = %d\n",  (int)sizeof(struct DbArray));
		fprintf(stderr, "sizeof Document = %d\n",  (int)sizeof(struct Document));
		fprintf(stderr, "sizeof rawobj_t = %d\n",  (int)sizeof(rawobj_t));
		fprintf(stderr, "sizeof pair_t = %d\n",  (int)sizeof(pair_t));
	}

	//	assemble user arguments into
	//	an argument array

	for(idx = nScripts; idx < argc; idx++) {
	  val = newString(argv[idx], -1);
	  vec_push(aval.valuePtr, val);
	}

	execScripts(pd->table, pd->tableNext, args, &globalSymbols, NULL);
	return 0;
}


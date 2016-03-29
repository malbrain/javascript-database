#include "jsdb.h"
#include "jsdb_eval.h"

static bool debug = false;

static char vt_handle_str[]  = "handle";
static char vt_docid_str[]   = "docid";
static char vt_string_str[]  = "string";
static char vt_int_str[]	 = "int";
static char vt_dbl_str[]	 = "dbl";
static char vt_file_str[]    = "file";
static char vt_status_str[]  = "status";
static char vt_null_str[]    = "null value";
static char vt_closure_str[] = "function";

static char *ok_str = "OK";
static char *outofmemory_str = "out of memory";
static char *handleclosed_str = "handle closed";
static char *badhandle_str = "bad handle";
static char *badrecid_str = "bad recid";
static char *endoffile_str = "end of file";
static char *notbasever_str = "not base version";
static char *recorddeleted_str = "record deleted";
static char *recordnotvisible_str = "record not visible";
static char *notcurrentversion_str = "not current version";
static char *cursornotpositioned_str = "cursor not positioned";
static char *invaliddeleterecord_str = "invalid delete record";
static char *cursorbasekeyerror_str = "cursor basekey error";
static char *writeconflict_str = "write conflict error";
static char *duplicatekey_str = "duplicate key";
static char *keynotfound_str = "key not found ";
static char *badtxnstep_str = "bad txn step";
static char *arena_already_closed_str = "arena already closed";
static char *unrecognized_str = "unrecognized";
static char *script_internal_str = "script internal error";
static char *script_unrecognized_function_str = "script unrecognized function";

char *strtype(valuetype_t t) {
	switch (t) {
	case vt_handle: return vt_handle_str;
	case vt_docId: return vt_docid_str;
	case vt_string: return vt_string_str;
	case vt_int: return vt_int_str;
	case vt_dbl: return vt_dbl_str;
	case vt_file: return vt_file_str;
	case vt_status: return vt_status_str;
	case vt_null: return vt_null_str;
	case vt_closure: return vt_closure_str;
	default:;
	}
	return unrecognized_str;
}

char *strstatus(Status s) {
	switch (s) {
	case OK: return ok_str;
	case ERROR_outofmemory: return outofmemory_str;
	case ERROR_handleclosed: return handleclosed_str;
	case ERROR_badhandle: return badhandle_str;
	case ERROR_badrecid: return badrecid_str;
	case ERROR_endoffile: return endoffile_str;
	case ERROR_notbasever: return notbasever_str;
	case ERROR_recorddeleted: return recorddeleted_str;
	case ERROR_recordnotvisible: return recordnotvisible_str;
	case ERROR_notcurrentversion: return notcurrentversion_str;
	case ERROR_cursornotpositioned: return cursornotpositioned_str;
	case ERROR_invaliddeleterecord: return invaliddeleterecord_str;
	case ERROR_cursorbasekeyerror: return cursorbasekeyerror_str;
	case ERROR_writeconflict: return writeconflict_str;
	case ERROR_duplicatekey: return duplicatekey_str;
	case ERROR_keynotfound: return keynotfound_str;
	case ERROR_badtxnstep: return badtxnstep_str;
	case ERROR_arena_already_closed: return arena_already_closed_str;
	case ERROR_script_internal: return script_internal_str;
	case ERROR_script_unrecognized_function: return script_unrecognized_function_str;
	default:;
	}
	return unrecognized_str;
}

//  strings

value_t newString(
	uint8_t *value,
	uint32_t len)
{
	value_t v;
	v.bits = vt_string;

	if (len) {
		v.str = jsdb_alloc(len, false);
		v.refcount = 1;
	}

	memcpy(v.str, value, len);
	v.aux  = len;
	return v;
}

dispatchFcn dispatchTable[node_MAX];

value_t eval_arg(uint32_t *args, environment_t *env) {
	listNode *ln = (listNode *)(env->table + *args);
	value_t v;

	if (ln->hdr->type) {
		v = dispatch(ln->elem, env);
		*args -= sizeof(listNode) / sizeof(Node);
	} else
		v.bits = vt_endlist;

	return v;
}

value_t eval_num (Node *a, environment_t *env) {
	numNode *nn = (numNode *)a;
	value_t v;

	switch (nn->hdr->aux) {
	case nn_int:
		v.bits = vt_int;
		v.nval = nn->intval;
		return v;
	case nn_dbl:
		v.bits = vt_dbl;
		v.dbl = nn->dblval;
		return v;
	case nn_bool:
		v.bits = vt_bool;
		v.boolean = nn->boolval;
		return v;
	case nn_null:
		v.bits = vt_null;
		return v;
	case nn_this:
		return env->thisVal;
	}

	fprintf(stderr, "Error in numNode type: %d\n", nn->hdr->aux);
	exit(1);
}

value_t eval_badop (Node *a, environment_t *env) {
	return makeError(a, env, "Node type not implemented");
}

value_t eval_noop (Node *a, environment_t *env) {
	value_t v;

	v.bits = vt_null;
	return v;
}

value_t eval_access (Node *a, environment_t *env) {
	binaryNode *bn = (binaryNode *)a;
	value_t *slot, obj = dispatch(bn->left, env);
	value_t v, field = dispatch(bn->right, env);
	uint32_t idx;

	// document property

	if (obj.type == vt_document) {
		if (field.type == vt_string) {
			v = lookupDoc(obj.document, field);
			abandonValue(field);
			abandonValue(obj);
			return v;
		}
	}

	// object property

	if (obj.type == vt_object) {
	  if (field.type == vt_string)
		if ((slot = lookup(obj, field, a->flag & flag_lval))) {
		  if (a->flag & flag_lval) {
		 	v.bits = vt_lval;
			v.lval = slot;
		  } else
			v = *slot;

		  abandonValue(field);
		  abandonValue(obj);
		  return v;
		}
	}

	if (field.type == vt_string) {
		obj.lvalue = a->flag & flag_lval ? 1 : 0;
		v = builtinProp(obj, field, env);
		abandonValue(field);
		abandonValue(obj);
		return v;
	}

	v.bits = vt_null;
	return v;
}

value_t eval_lookup (Node *a, environment_t *env) {
	binaryNode *bn = (binaryNode *)a;
	value_t *slot, obj = dispatch(bn->left, env);
	value_t v, field = dispatch(bn->right, env);
	uint32_t idx;

	// document property

	if (obj.type == vt_document) {
		if (field.type == vt_string) {
			v = lookupDoc(obj.document, field);
			abandonValue(field);
			abandonValue(obj);
			return v;
		}
	}

	// object property

	if (obj.type == vt_object) {
	  if (field.type == vt_string)
		if ((slot = lookup(obj, field, a->flag & flag_lval))) {
		  if (a->flag & flag_lval) {
		 	v.bits = vt_lval;
			v.lval = slot;
		  } else
			v = *slot;

		  abandonValue(field);
		  abandonValue(obj);
		  return v;
		}
	}

	// array index

	if (obj.type == vt_array) {
		idx = conv2Int(field).nval;

		if (~a->flag & flag_lval) {
			if (idx < vec_count(obj.aval->array))
				v = (obj.aval->array)[idx];
			else
				v.bits = vt_null;
		} else {
			int diff = idx - vec_count(obj.aval->array) + 1;

			if (diff > 0)
				vec_add (obj.aval->array, diff);

			v.bits = vt_lval;
			v.lval = &((obj.aval->array)[idx]);
		}

		abandonValue(field);
		abandonValue(obj);
		return v;
	}

	//  document array index

	if (obj.type == vt_docarray) {
		idx = conv2Int(field).nval;

		if (a->flag & flag_lval)
			return makeError(a, env, "Invalid document mutation");

		if (idx < obj.docarray->count)
			v = (obj.docarray->array)[idx];
		else
			v.bits = vt_null;

		abandonValue(field);
		abandonValue(obj);
		return v;
	}

	if (field.type == vt_string) {
		v = builtinProp(obj, field, env);
		abandonValue(field);
		abandonValue(obj);
		return v;
	}

	v.bits = vt_null;
	return v;
}

value_t eval_array (Node *n, environment_t *env) {
	arrayNode *an = (arrayNode *)n;
	value_t v, a = newArray();
	uint32_t l;

	if ((l = an->exprlist)) do {
		listNode *ln = (listNode *)(env->table + l);
		if (!ln->hdr->type)
			break;
		l -= sizeof(listNode) / sizeof(Node);
		v = dispatch(ln->elem, env);
		incrRefCnt(v);
		vec_push(a.aval->array, v);
	} while ( true );

	return a;
}

value_t eval_obj (Node *n, environment_t *env) {
	objNode *on = (objNode *)n;
	value_t v, o = newObject();
	uint32_t e, l;

	if ((l = on->elemlist)) do {
		listNode *ln = (listNode *)(env->table + l);

		if (!ln->hdr->type)
			break;

		l -= sizeof(listNode) / sizeof(Node);

		binaryNode *bn = (binaryNode *)(env->table + ln->elem);
		v = dispatch(bn->left, env);

		if (v.type == vt_string) {
			value_t *w = lookup(o, v, true);
			replaceSlotValue (w, dispatch(bn->right, env));
		}

		abandonValue(v);
	} while ( true );

	return o;
}

value_t eval_list(Node *n, environment_t *env)
{
	uint32_t list;
	value_t v;

	if (list = n - env->table) do {
		listNode *ln = (listNode *)(env->table + list);

		if (ln->hdr->type)
			v = dispatch (ln->elem, env);
		else
			break;

		if (v.type == vt_control)
			return v;

		list -= sizeof(listNode) / sizeof(Node);
		abandonValue(v);
	} while (true);

	v.bits = vt_null;
	return v;
}

value_t eval_ref(Node *a, environment_t *env)
{
	symNode *sn = (symNode*)a;
	value_t v;

	v.bits = vt_ref;
	v.ref = &env->framev[sn->level]->values[sn->frameidx];
	return v;
}

value_t eval_var(Node *a, environment_t *env)
{
	symNode *sn = (symNode*)a;
	value_t v, *slot = &env->framev[sn->level]->values[sn->frameidx];

	if (a->flag & flag_lval) {
		v.bits = vt_lval;
		v.lval = slot;
		return v;
	}

	v = *slot;

	// delayed fcn closures

	if (v.type == vt_fcndef)
		return replaceSlotValue(slot, newClosure (v.fcn, v.aux, env->table, env->framev));

	return v;
}

value_t eval_string(Node *a, environment_t *env)
{
	stringNode *sn = (stringNode *)a;
	value_t v;
	
	v.bits = vt_string;
	v.aux = sn->hdr->aux;
	v.str = sn->string;
	return v;
}

value_t eval_while(Node *a, environment_t *env)
{
	whileNode *wn = (whileNode*)a;
	if (debug) printf("node_while\n");
	value_t v;

	if (wn->cond)
	  while (true) {
		value_t condVal = dispatch(wn->cond, env);
		bool cond = conv2Bool(condVal).boolean;
		abandonValue(condVal);

		if (!cond)
			break;

		v = dispatch(wn->stmt, env);

		if (v.type == vt_control) {
			if (v.ctl == flag_break)
				break;
			else if (v.ctl == flag_return)
				return v;
		}

		abandonValue(v);
	  }

	v.bits = vt_null;
	return v;
}

value_t eval_dowhile(Node *a, environment_t *env)
{
	whileNode *wn = (whileNode*)a;
	value_t condVal, v;
	bool cond;

	do {
		v = dispatch(wn->stmt, env);

		if (v.type == vt_control) {
			if (v.ctl == flag_break)
				break;
			else if (v.ctl == flag_return)
				return v;
		}

		abandonValue(v);

		if (!wn->cond)
			break;

		condVal = dispatch(wn->cond, env);
		cond = conv2Bool(condVal).boolean;
		abandonValue(condVal);
	} while (cond);

	v.bits = vt_null;
	return v;
}

value_t eval_ifthen(Node *a, environment_t *env)
{
	ifThenNode *iftn = (ifThenNode*)a;
	value_t condVal, v;
	uint32_t stmt;
	bool cond;

	condVal = dispatch(iftn->condexpr, env);
	cond = conv2Bool(condVal).boolean;
	abandonValue(condVal);
	v.bits = vt_null;

	if (cond) {
		if (iftn->thenstmt)
			stmt = iftn->thenstmt;
		else
			return v;
	}
	else {
		if (iftn->elsestmt)
			stmt = iftn->elsestmt;
		else
			return v;
	}

	return dispatch(stmt, env);
}

value_t eval_return(Node *a, environment_t *env)
{
	exprNode *en = (exprNode *)a;
	value_t v;

	if (en->expr)
		v = dispatch(en->expr, env);
	else
		v.bits = vt_null;

	incrRefCnt(v);
	env->framev[vec_count(env->framev) - 1]->rtnVal = v;
	
	v.bits = vt_control;
	v.ctl = a->flag & flag_typemask;
	return v;
}

value_t eval_for(Node *a, environment_t *env)
{
	forNode *fn = (forNode*)a;
	value_t condVal, v;
	bool cond;

	if (fn->init) {
		v = dispatch(fn->init, env);
		if (v.type == vt_control)
			return v;
		abandonValue(v);
	}

	if (fn->cond) while (true) {
		condVal = dispatch(fn->cond, env);
		cond = conv2Bool(condVal).boolean;
		abandonValue(condVal);

		if (!cond)
			break;

		v = dispatch(fn->stmt, env);

		if (v.type == vt_control) {
			if (v.ctl == flag_break)
				break;
			else if (v.ctl == flag_return)
				return v;
		}

		abandonValue(v);

		if (fn->incr) {
			v = dispatch(fn->incr, env);
			if (v.type == vt_control)
				return v;
			abandonValue(v);
		}
	}

	v.bits = vt_null;
	return v;
}

/*
void installSymbol(char *name, symtab_t *symtab) {
	struct SymNode *sn = (struct SymNode *)newSymNode( newStringNode(name, strlen(name)) );
	insertSymbol(sn, symtab, 0);
}
*/

void initStatusSymbols(symtab_t *symtab) {
/*
	installSymbol("status_ok", symtab);
	installSymbol("status_outofmemory", symtab);
	installSymbol("status_handleclosed", symtab);
	installSymbol("status_badhandle", symtab);
	installSymbol("status_badrecid", symtab);
	installSymbol("status_endoffile", symtab);
	installSymbol("status_notbasever", symtab);
	installSymbol("status_recorddeleted", symtab);
	installSymbol("status_recordnotvisible", symtab);
	installSymbol("status_notcurrentversion", symtab);
	installSymbol("status_cursornotpositioned", symtab);
	installSymbol("status_invaliddeleterecord", symtab);
	installSymbol("status_cursorbasekeyerror", symtab);
	installSymbol("status_writeconflict", symtab);
	installSymbol("status_duplicatekey", symtab);
	installSymbol("status_keynotfound", symtab);
	installSymbol("status_badtxnstep", symtab);
	installSymbol("status_arena_already_closed", symtab);
	installSymbol("status_script_internal", symtab);
*/
}

void usage(char* cmd) {
	printf("%s [-f fname]\n", cmd);
}

double getCpuTime(int);

int main(int argc, char* argv[])
{
	valueframe_t *framev = NULL;
	fcnDeclNode *topLevel;
	double start, elapsed;
	environment_t env[1];
	uint32_t firstNode;
	char *out = NULL;
	parseData pd[1];
	frame_t *frame;
	int i, k;

	printf("sizeof value_t = %ld\n",  sizeof(value_t));
	printf("sizeof Node = %ld\n",  sizeof(Node));

	for (i = 0; i < node_MAX; i++)
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
	dispatchTable[node_neg] = eval_neg;
	dispatchTable[node_var] = eval_var;
	dispatchTable[node_ref] = eval_ref;
	dispatchTable[node_for] = eval_for;
	dispatchTable[node_obj] = eval_obj;
	dispatchTable[node_num] = eval_num;

	installProps ();

	while (argc > 1 && argv[1][0] == '-') {
		switch (argv[1][1]) {
		case 'f':
			freopen(argv[2],"r",stdin);
			break;
		case 'w':
			out = argv[2];
			break;
		default:
			usage(argv[0]);
			exit(-1);
		}
		argc--;
		argv++;
	}

	// initialize

	memset(pd, 0, sizeof(parseData));
	yylex_init(&pd->scaninfo);
	yyset_debug(1, pd->scaninfo);
	pd->lineno = 1;

	// occupy table slot zero with zeroes

	firstNode = newNode(pd, node_endlist, sizeof(fcnDeclNode), true);

	if ( (k = yyparse(pd->scaninfo, pd)) ) {
		if (k==1)
			printf("Parse error\n");
		else if (k==2)
			printf("Parser exhausted memory\n");
		return -1;
	}

	// cleanup

	yylex_destroy(pd->scaninfo);

	// initialize node zero

	topLevel = (fcnDeclNode *)(pd->table + firstNode);
	topLevel->body = pd->beginning;

	// hoist and assign var decls

	compile(topLevel, pd->table, NULL, 0);

	if (out) {
		FILE *strm = fopen(out, "wb");
		fwrite (pd->table, sizeof(Node), pd->tablenext, strm);
		fclose (strm);
		exit(0);
	}

	frame = jsdb_alloc(sizeof(value_t) * topLevel->nsymbols + sizeof(frame_t), true);
	frame->count = topLevel->nsymbols;
	frame->name = 0;

	vec_push(framev, frame);
	incrFrameCnt(frame);

	installFcns(topLevel->fcn, pd->table, frame);
	start = getCpuTime(0);

	env->table = pd->table;
	env->framev = framev;

	dispatch(pd->beginning, env);

	elapsed = getCpuTime(0) - start;
	fprintf (stderr, "real %dm%.6fs\n", (int)(elapsed/60), elapsed - (int)(elapsed/60)*60);
	return 0;
}


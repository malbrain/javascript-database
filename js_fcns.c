#include "js.h"
#include "js_malloc.h"

// closures

value_t newClosure( fcnDeclNode *fd, environment_t *env) {
	closure_t *closure;
	uint32_t depth = 1;
	value_t v;

	if (env->closure)
		depth += env->closure->depth;

	closure = js_alloc(sizeof(closure_t) + sizeof(scope_t) * depth, true);
	closure->scope[0] = js_alloc(sizeof(scope_t) + sizeof(value_t) * env->scope->count, true);
	closure->scope[0]->symbols = env->scope->symbols;
	closure->scope[0]->count = env->scope->count;
	closure->scope[0]->frame = env->topFrame;

	for (uint32_t idx = 1; idx < env->scope->count; idx++)
		replaceSlot(closure->scope[0]->values + idx, env->scope->values[idx]);

	incrScopeCnt(closure->scope[0]);

	for (uint32_t idx=1; idx < depth; idx++) {
		closure->scope[idx] = env->closure->scope[idx-1];
		incrScopeCnt(closure->scope[idx]);
	}

	closure->obj = newObject(vt_closure);
	incrRefCnt(closure->obj);

	closure->protoObj = newObject(vt_object);
	incrRefCnt(closure->protoObj);

	closure->symbols = &fd->symbols;
	closure->table = env->table;
	closure->depth = depth;
	closure->fd = fd;

	v.bits = vt_closure;
	v.closure = closure;
	v.refcount = 1;
	v.objvalue = 1;
	return v;
}

// function expressions

value_t eval_fcnexpr (Node *a, environment_t *env) {
	fcnDeclNode *fd = (fcnDeclNode *)a;

	return newClosure(fd, env);
}

// do function call

value_t fcnCall (value_t fcnClosure, value_t args, value_t thisVal, bool rtnVal, environment_t *env) {
	closure_t *closure = fcnClosure.closure;
	fcnDeclNode *fd = closure->fd;
	environment_t newEnv[1];
	frame_t *frame;
	scope_t *scope;
	array_t *aval;
	value_t v;

	memset (newEnv, 0, sizeof(environment_t));

	frame = js_alloc(sizeof(value_t) * fd->symbols.frameIdx + sizeof(frame_t), true);
	frame->count = fd->symbols.frameIdx;
	frame->nextThis.bits = vt_undef;

	replaceSlot(&frame->thisVal, thisVal);
	replaceSlot(&frame->arguments, args);

	scope = js_alloc(sizeof(scope_t) + sizeof(value_t) * fd->symbols.scopeCnt, true);
	scope->count = fd->symbols.scopeCnt;
	scope->frame = frame;

	incrScopeCnt(scope);

	aval = js_addr(args);

	for (uint32_t idx = 0; idx < fd->nparams && idx < (uint32_t)(vec_cnt(aval->valuePtr)); idx++)
        replaceSlot(&frame->values[idx + 1], aval->valuePtr[idx]);

	//  prepare new environment

	newEnv->first = findFirstNode(closure->table, fd->body);
	newEnv->closure = fcnClosure.closure;
	newEnv->table = closure->table;
	newEnv->topFrame = frame;
	newEnv->scope = scope;

	if (env) {
		newEnv->timestamp = env->timestamp;
		*newEnv->txnBits = *env->txnBits;
	}

	installFcns(fd->symbols.childFcns, newEnv);

	//  install function expression closure

	if (fd->hdr->type == node_fcnexpr)
		if (fd->name) {
			value_t slot = dispatch(fd->name, newEnv);
			replaceValue(slot, fcnClosure);
		}

	//	pre-load return value with thisVal

	if (rtnVal) 
        replaceSlot(frame->values, thisVal);

	dispatch(fd->body, newEnv);
	v = frame->values[0];

	//	passback global environment values

	if (env) {
		*env->txnBits = *newEnv->txnBits;
		env->timestamp = newEnv->timestamp;
	}

	abandonScope(scope);
	decrRefCnt(v);
	return v;
}

//	return statement

value_t eval_return(Node *a, environment_t *env)
{
	exprNode *en = (exprNode *)a;
	value_t v;

	if (en->expr)
		v = dispatch(en->expr, env);
	else
		v.bits = vt_undef;

	replaceSlot(env->topFrame->values, v);

	v.bits = vt_control;
	v.ctl = a->aux;
	return v;
}

// function calls

value_t eval_fcncall (Node *a, environment_t *env) {
	value_t args = newArray(array_value, 0);
	fcnCallNode *fc = (fcnCallNode *)a;
	array_t *aval = args.addr;
	value_t fcn, v, thisVal;
	bool returnFlag = false;
	uint32_t argList;
	value_t nextThis;
	listNode *ln;

	// process arg list

	if ((argList = fc->args)) do {
		ln = (listNode *)(env->table + argList);
		v = dispatch(ln->elem, env);
		incrRefCnt(v);
		vec_push(aval->valuePtr, v);
		argList -= sizeof(listNode) / sizeof(Node);
	} while (ln->hdr->type == node_list);

	//	prepare to calc new this value

	nextThis = env->topFrame->nextThis;
	env->topFrame->nextThis.bits = vt_undef;

	//	evaluate a closure or internal property fcn

	fcn = dispatch(fc->name, env);

	if (fcn.type == vt_lval)
		fcn = *fcn.lval;

	if (fcn.type == vt_propfcn) {
		v = callFcnFcn(fcn, aval->valuePtr, env);
		abandonValue(args);
		return v;
	}

	if (fcn.type != vt_closure) {
		firstNode *fn = findFirstNode(env->table, (uint32_t)(a - env->table));
		symNode *sym = (symNode *)(env->table + fc->name);
		stringNode *sn = (stringNode *)(env->table + sym->name);
		fprintf(stderr, "%s not function closure: %s line: %d\n", sn->str.val, fn->script, (int)a->lineNo);

		exit(1);
	}

	if (fc->hdr->aux == aux_newobj) {
		thisVal = newObject(vt_object);
		object_t *oval = thisVal.addr;

		oval->protoChain = fcn.closure->protoObj;
		incrRefCnt(oval->protoChain);
		returnFlag = true;
	} else {
		thisVal = env->topFrame->nextThis;
		returnFlag = false;
	}

	//	args will be pushed into a new frame by callFcn

	v = fcnCall(fcn, args, thisVal, returnFlag, env);

	env->topFrame->nextThis.bits = nextThis.bits;
	abandonValue(fcn);
	return v;
}

//  make function closures for functions defined in function

void installFcns(uint32_t decl, environment_t *env) {

	while (decl) {
		fcnDeclNode *fd = (fcnDeclNode *)(env->table + decl);
		symNode *sym = (symNode *)(env->table + fd->name);
		value_t v = newClosure(fd, env);

		incrRefCnt(v);
		replaceSlot(&env->topFrame->values[sym->frameIdx], v);
		decl = fd->next;
	}
}

//	execute collection of scripts

double getCpuTime(int);

void execScripts(Node *table, uint32_t size, value_t args, symtab_t *symbols, environment_t *oldEnv) {
	environment_t env[1];
	uint32_t start = 0;
	uint32_t depth = 0;
	closure_t *closure;
	symtab_t block;
	frame_t *frame;
	firstNode *fn;
	value_t v;

	if (oldEnv)
		depth = oldEnv->closure->depth + 1;

	// hoist and assign symbols decls

	memset (&block, 0, sizeof(block));
	compileScripts(size, table, symbols, &block);

	//  build new frame

	frame = js_alloc(sizeof(value_t) * symbols->frameIdx + sizeof(frame_t), true);
	replaceSlot(&frame->arguments, args);
	frame->count = symbols->frameIdx;
	frame->nextThis.bits = vt_undef;
	frame->thisVal.bits = vt_undef;

	//  allocate the top level closure

	closure = js_alloc(sizeof(closure_t) + sizeof(scope_t) * depth, true);
	closure->scope[0] = js_alloc(sizeof(scope_t) + sizeof(value_t) * symbols->scopeCnt, true);
	closure->scope[0]->frame = oldEnv ? oldEnv->topFrame : NULL;
	closure->scope[0]->count = symbols->scopeCnt;
	closure->obj = newObject(vt_closure);
	closure->protoObj = newObject(vt_object);
	incrRefCnt(closure->protoObj);

	incrScopeCnt(closure->scope[0]);

	closure->symbols = symbols;
	closure->table = table;
	closure->depth = depth;

	if (oldEnv)
		closure->scope[0]->frame = oldEnv->topFrame;

	for (uint32_t idx=1; idx < depth; idx++) {
		closure->scope[idx] = oldEnv->closure->scope[idx-1];
		incrScopeCnt(closure->scope[idx]);
	}

	memset (env, 0, sizeof(environment_t));
	env->scope = closure->scope[0];
	env->closure = closure;
	env->topFrame = frame;
	env->table = table;

	installFcns(symbols->childFcns, env);

	//	run each script in the table

	while (start < size) {
		fn = (firstNode *)(table + start);
		double strtTime, elapsed;
		env->first = fn;

		start += fn->moduleSize;

		strtTime = getCpuTime(0);

		dispatch(fn->begin, env);

		elapsed = getCpuTime(0) - strtTime;

		if (debug)
			fprintf (stderr, "Execution: %dm%.6fs %s \n", (int)(elapsed/60), elapsed - (int)(elapsed/60)*60, fn->script);
	}

	if (env->timestamp)
		js_free(env->timestamp);

	// abandon global scope

	v.bits = vt_closure;
	v.closure = closure;
	deleteValue(v);
}

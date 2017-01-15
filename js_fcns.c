#include "js.h"
#include "js_malloc.h"

// closures

value_t newClosure( fcnDeclNode *fd, environment_t *env) {
	closure_t *closure;
	uint32_t depth = 1;
	value_t v;

	if (env->closure)
		depth += env->closure->count;

	closure = js_alloc(sizeof(closure_t) + sizeof(valueframe_t) * depth, true);
	closure->frames[0] = env->topFrame;
	incrFrameCnt(closure->frames[0]);

	for (int i=1; i < depth; i++) {
		closure->frames[i] = env->closure->frames[i-1];
		incrFrameCnt(closure->frames[i]);
	}

	closure->obj = newObject(vt_closure);
	closure->protoObj = newObject(vt_object);
	incrRefCnt(closure->protoObj);

	closure->symbols = &fd->symbols;
	closure->table = env->table;
	closure->count = depth;
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

value_t fcnCall (value_t fcnClosure, value_t args, value_t thisVal, bool rtnVal) {
	closure_t *closure = fcnClosure.closure;
	fcnDeclNode *fd = closure->fd;
	environment_t newEnv[1];
	frame_t *frame;
	array_t *aval;
	value_t v;

	memset (newEnv, 0, sizeof(environment_t));

	frame = js_alloc(sizeof(value_t) * fd->nsymbols + sizeof(frame_t), true);
	frame->nextThis.bits = vt_undef;
	frame->count = fd->nsymbols;
	frame->arguments = args;
	replaceSlot(&frame->thisVal, thisVal);

	incrRefCnt(fcnClosure); // ???
	incrFrameCnt(frame);

	// bind arguments to parameters

	if (args.refcount)
		args.raw[-1].refCnt[0] = 1;

	aval = js_addr(args);

	for (int idx = 0; idx < fd->nparams && idx < vec_cnt(aval->valuePtr); idx++)
        replaceSlot(&frame->values[idx + 1], aval->valuePtr[idx]);

	//  prepare new environment

	newEnv->first = findFirstNode(closure->table, fd->body);
	newEnv->closure = fcnClosure.closure;
	newEnv->table = closure->table;
	newEnv->topFrame = frame;

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

	decrRefCnt(thisVal);        // abandon our reference to 'this'
	decrRefCnt(fcnClosure);     // abondon our reference to the closure
	abandonFrame(frame, false);	// don't abandon frame->values
	decrRefCnt(v);              // matches the 'replaceSlot' in 'eval_return'
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

	replaceSlot(&env->topFrame->values[0], v);

	v.bits = vt_control;
	v.ctl = a->flag & flag_typemask;
	return v;
}

// function calls

value_t eval_fcncall (Node *a, environment_t *env) {
	value_t args = newArray(array_value);
	fcnCallNode *fc = (fcnCallNode *)a;
	array_t *aval = args.addr;
	value_t fcn, v, thisVal;
	bool returnFlag = false;
	bool old = env->lVal;
	uint32_t argList;
	listNode *ln;

	// process arg list

	args.raw[-1].refCnt[0] = 1;
	env->lVal = false;

	if ((argList = fc->args)) do {
		ln = (listNode *)(env->table + argList);
		v = dispatch(ln->elem, env);
		vec_push(aval->valuePtr, v);
		incrRefCnt(v);
		argList -= sizeof(listNode) / sizeof(Node);
	} while (ln->hdr->type == node_list);

	//	evaluate a closure or internal property fcn

	env->lVal = true;
	fcn = dispatch(fc->name, env);
	env->lVal = old;

	if (fcn.type == vt_lval)
		fcn = *fcn.lval;

	if (fcn.type == vt_propfcn) {
		v = callFcnFcn(fcn, aval->valuePtr, env);
		abandonValue(args);
		return v;
	}

	if (fcn.type != vt_closure) {
		firstNode *fn = findFirstNode(env->table, a - env->table);
		printf("%s not function closure line: %d\n", fn->script, (int)a->lineNo);
		exit(1);
	}

	if ((fc->hdr->flag & flag_typemask) == flag_newobj) {
		abandonValue(env->topFrame->nextThis);
		thisVal = newObject(vt_object);
		object_t *oval = thisVal.addr;

		oval->protoChain = fcn.closure->protoObj;
		incrRefCnt(oval->protoChain);
		returnFlag = true;
	} else {
		thisVal = env->topFrame->nextThis;
		env->topFrame->nextThis.bits = vt_undef;
		returnFlag = false;
	}

	//	args will be pushed into a new frame by callFcn

	v = fcnCall(fcn, args, thisVal, returnFlag);

	abandonValue(fcn);
	return v;
}

//  make function closures for functions defined in function

void installFcns(uint32_t decl, environment_t *env) {

	while (decl) {
		fcnDeclNode *fd = (fcnDeclNode *)(env->table + decl);
		symNode *sym = (symNode *)(env->table + fd->name);
		value_t v = newClosure(fd, env);

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
	frame_t *frame;
	value_t v;

	if (oldEnv)
		depth = oldEnv->closure->count + 1;

	// hoist and assign symbols decls

	compileScripts(size, table, symbols);

	//  build new frame

	frame = js_alloc(sizeof(value_t) * symbols->frameIdx + sizeof(frame_t), true);
	frame->count = symbols->frameIdx;
	frame->arguments = args;

	//  allocate the closure

	closure = js_alloc(sizeof(closure_t) + sizeof(valueframe_t) * depth, true);
	closure->obj = newObject(vt_closure);
	closure->protoObj = newObject(vt_object);
	incrRefCnt(closure->protoObj);

	closure->symbols = symbols;
	closure->table = table;
	closure->count = depth;

	if (oldEnv)
		closure->frames[0] = oldEnv->topFrame;

	for (int i=1; i < depth; i++) {
		closure->frames[i] = oldEnv->closure->frames[i-1];
		incrFrameCnt(closure->frames[i]);
	}

	memset (env, 0, sizeof(environment_t));
	env->closure = closure;
	env->topFrame = frame;
	env->table = table;

	installFcns(symbols->childFcns, env);

	//	run each script in the table

	while (start < size) {
		firstNode *fn = (firstNode *)(table + start);
		double strtTime, elapsed;
		env->first = fn;

		start += fn->moduleSize;

		strtTime = getCpuTime(0);

		dispatch(fn->begin, env);

		elapsed = getCpuTime(0) - strtTime;
		if (debug)
			fprintf (stderr, "Execution: %dm%.6fs %s \n", (int)(elapsed/60), elapsed - (int)(elapsed/60)*60, fn->script);
	}

	v.bits = vt_closure;
	v.closure = closure;
	deleteValue(v);
}

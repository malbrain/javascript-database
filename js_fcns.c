#include "js.h"

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

	replaceSlot(&closure->protoObj, newObject(vt_object));
	replaceSlot(&closure->obj, newObject(vt_closure));
	replaceSlot(&closure->obj.oval->protoChain, builtinProto[vt_closure]);

	closure->symbols = fd->symbols;
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

value_t fcnCall (value_t fcnClosure, value_t args, value_t thisVal) {
	closure_t *closure = fcnClosure.closure;
	fcnDeclNode *fd = closure->fd;
	environment_t newenv[1];
	frame_t *frame;
	value_t v;

	incrRefCnt(fcnClosure);

	frame = js_alloc(sizeof(value_t) * fd->nsymbols + sizeof(frame_t), true);
	frame->count = fd->nsymbols;
	frame->thisVal = thisVal;
	frame->arguments = args;

	// protect temorary objects from abandonment

	incrRefCnt(args);
	incrRefCnt(thisVal);
	incrFrameCnt(frame);

	// bind arguments to parameters

	for (int idx = 0; idx < fd->nparams && idx < vec_count(args.aval->values); idx++) {
		frame->values[idx + 1] = args.aval->values[idx];
		incrRefCnt(frame->values[idx + 1]);
	}

	//  prepare new environment

	newenv->closure = fcnClosure.closure;
	newenv->table = closure->table;
	newenv->topFrame = frame;

	installFcns(fd->symbols->childFcns, newenv);

	//  install function expression closure

	if (fd->hdr->type == node_fcnexpr)
		if (fd->name) {
			value_t slot = dispatch(fd->name, newenv);
			replaceValue (slot, fcnClosure);
		}

	dispatch(fd->body, newenv);
	v = frame->values[0];

	incrRefCnt(v);
	decrRefCnt(fcnClosure);
	abandonFrame(frame);
	decrRefCnt(thisVal);
	decrRefCnt(v);
	return v;
}

// function calls

value_t eval_fcncall (Node *a, environment_t *env) {
	value_t args = newArray(array_value);
	fcnCallNode *fc = (fcnCallNode *)a;
	value_t fcn, v, thisVal, slot;
	uint32_t argList;
	listNode *ln;

	// process arg list

	if ((argList = fc->args)) do {
		ln = (listNode *)(env->table + argList);
		v = dispatch(ln->elem, env);
		vec_push(args.aval->values, v);
		incrRefCnt(v);
		argList -= sizeof(listNode) / sizeof(Node);
	} while (ln->hdr->type == node_list);

	//  prepare to capture "this" value from the name evaluation

	v.bits = vt_undef;
	slot.bits = vt_lval;
	slot.lval = &env->topFrame->nextThis;
	replaceValue(slot, v);

	fcn = dispatch(fc->name, env);

	if (fcn.type == vt_propfcn)
		return callFcnFcn(fcn, args.aval->values, env->topFrame->nextThis);

	if (fcn.type != vt_closure) {
		firstNode *fn = findFirstNode(env->table, a - env->table);
		printf("%s not function closure line: %d\n", fn->script, (int)a->lineNo);
		exit(1);
	}

	if ((fc->hdr->flag & flag_typemask) == flag_newobj) {
		thisVal = newObject(vt_undef);
		replaceSlot(&thisVal.oval->protoChain, fcn.closure->protoObj);
	} else
		thisVal = env->topFrame->nextThis;

	v = fcnCall(fcn, args, thisVal);

	if ((fc->hdr->flag & flag_typemask) == flag_newobj) {
	  if (v.type == vt_object && v.oval != thisVal.oval)
			abandonValue(thisVal);
	  else if (!v.objvalue) {
		abandonValue(v);
		v = thisVal;
	  }
	}

	// abandon closure

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
	replaceSlot(&closure->protoObj, newObject(vt_object));
	replaceSlot(&closure->obj, newObject(vt_closure));
	replaceSlot(&closure->obj.oval->protoChain, builtinProto[vt_closure]);

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

		start += fn->moduleSize;

		strtTime = getCpuTime(0);

		dispatch(fn->begin, env);

		elapsed = getCpuTime(0) - strtTime;
		fprintf (stderr, "%s real %dm%.6fs\n", fn->script, (int)(elapsed/60), elapsed - (int)(elapsed/60)*60);
	}

	v.bits = vt_closure;
	v.closure = closure;
	deleteValue(v);
}

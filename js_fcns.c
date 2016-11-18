#include "js.h"

// closures

value_t newClosure( fcnDeclNode *fcn, environment_t *env) {
	closure_t *closure;
	uint32_t depth;
	value_t v;

	if (env->closure)
		depth = env->closure->count + 1;
	else
		depth = 0;

	closure = js_alloc(sizeof(closure_t) + sizeof(valueframe_t) * depth, true);
	closure->frames[0] = env->topFrame;
	incrFrameCnt(closure->frames[0]);

	for (int i=1; i < depth; i++) {
		closure->frames[i] = env->closure->frames[i-1];
		incrFrameCnt(closure->frames[i]);
	}

	closure->table = env->table;
	closure->count = depth;
	closure->fcn = fcn;

	v.bits = vt_closure;
	v.closure = closure;
	v.refcount = 1;
	return v;
}

// function expressions

value_t eval_fcnexpr (Node *a, environment_t *env) {
	fcnDeclNode *fcn = (fcnDeclNode *)a;

	return newClosure(fcn, env);
}

// do function call

value_t fcnCall (value_t fcnClosure, value_t args, value_t thisVal) {
	closure_t *closure = fcnClosure.closure;
	fcnDeclNode *fcn = closure->fcn;
	environment_t newenv[1];
	frame_t *frame;
	uint32_t body;
	value_t v;

	incrRefCnt(fcnClosure);

	frame = js_alloc(sizeof(value_t) * fcn->nsymbols + sizeof(frame_t), true);
	frame->count = fcn->nsymbols;
	frame->thisVal = thisVal;
	frame->arguments = args;

	// protect temorary objects from abandonment

	incrRefCnt(args);
	incrRefCnt(thisVal);
	incrFrameCnt(frame);

	body = fcn->body;

	// bind arguments to parameters

	for (int idx = 0; idx < fcn->nparams && idx < vec_count(args.aval->values); idx++) {
		frame->values[idx + 1] = args.aval->values[idx];
		incrRefCnt(frame->values[idx + 1]);
	}

	//  prepare new environment

	newenv->closure = fcnClosure.closure;
	newenv->table = closure->table;
	newenv->topFrame = frame;

	installFcns(fcn->fcn, newenv);

	//  install function expression closure

	if (fcn->hdr->type == node_fcnexpr)
		if (fcn->name) {
			value_t slot = dispatch(fcn->name, newenv);
			replaceValue (slot, fcnClosure);
		}

	dispatch(body, newenv);
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
		return (fcn.propfcn)(args.aval->values, env->topFrame->nextThis);

	if (fcn.type != vt_closure) {
		firstNode *fn = (firstNode *)env->table;	// get script name
		printf("%s not function closure line: %d\n", fn->string, (int)a->lineno);
		exit(1);
	}

	if ((fc->hdr->flag & flag_typemask) == flag_newobj) {
		thisVal = newObject();
		thisVal.oval->proto = fcn.closure->proto;
	} else
		thisVal = env->topFrame->nextThis;

	v = fcnCall(fcn, args, thisVal);

	if ((fc->hdr->flag & flag_typemask) == flag_newobj) {
	  if (v.type == vt_array) {
		v.aval->obj->proto = fcn.closure->proto;
	  } else if (!v.type)
		v = thisVal;
	}

	// abandon closure

	abandonValue(fcn);
	return v;
}

//  make function closures for functions defined in function

void installFcns(uint32_t decl, environment_t *env) {

	while (decl) {
		fcnDeclNode *fcn = (fcnDeclNode *)(env->table + decl);
		symNode *sym = (symNode *)(env->table + fcn->name);
		value_t v, slot;

		slot.bits = vt_lval;
		slot.lval = &env->topFrame->values[sym->frameidx];

		v = newClosure(fcn, env);
		replaceValue(slot, v);
		decl = fcn->next;
	}
}

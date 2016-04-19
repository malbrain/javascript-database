#include "jsdb.h"

static int debug = 0;

// closures

value_t newClosure( fcnDeclNode *fcn, environment_t *env) {
	uint32_t depth = env->closure->count + 1;
	closure_t *result;
	value_t v;

	result = jsdb_alloc(sizeof(closure_t) + sizeof(valueframe_t) * depth, true);
	result->frames[0] = env->topFrame;
	incrFrameCnt(result->frames[0]);

	for (int i=1; i < depth; i++) {
		result->frames[i] = env->closure->frames[i-1];
		incrFrameCnt(result->frames[i]);
	}

	result->table = env->closure->table;
	result->count = depth;
	result->fcn = fcn;

	v.bits = vt_closure;
	v.closure = result;
	v.refcount = 1;
	return v;
}

// function expressions

value_t eval_fcnexpr (Node *a, environment_t *env) {
	fcnDeclNode *fn = (fcnDeclNode *)a;
	return newClosure(fn, env);
}

// do function call

value_t fcnCall (value_t fcnClosure, value_t args, value_t thisVal) {
	closure_t *closure = fcnClosure.closure;
	fcnDeclNode *fcn = closure->fcn;
	environment_t newenv[1];
	uint32_t body, params;
	frame_t *frame;
	listNode *ln;
	value_t v;

	incrRefCnt(fcnClosure);

	frame = jsdb_alloc(sizeof(value_t) * fcn->nsymbols + sizeof(frame_t), true);
	frame->count = fcn->nsymbols;
	frame->thisVal = thisVal;
	frame->arguments = args;

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
	fcnDeclNode *fd;
	listNode *ln;

	// process arg list

	if ((argList = fc->args)) do {
		ln = (listNode *)(env->table + argList);
		v = dispatch(ln->elem, env);
		vec_push(args.aval->values, v);
		incrRefCnt(v);
		argList -= sizeof(listNode) / sizeof(Node);
	} while (ln->hdr->type == node_list);

	//  capture "this" value from the name evaluation

	env->topFrame->nextThis.bits = vt_undef;
	fcn = dispatch(fc->name, env);

	if (fcn.type == vt_propfcn)
		v = (fcn.propfcn)(args.aval->values, env->topFrame->nextThis);

	else {
	  if (fcn.type != vt_closure) {
		stringNode *sn = (stringNode *)(env->table);
		printf("%.*s not function closure line: %d\n", sn->hdr->aux, sn->string, a->lineno);
		exit(1);
	  }

	  if ((fc->hdr->flag & flag_typemask) == flag_newobj) {
		thisVal = newObject();
		thisVal.oval->proto = fcn.closure->proto;
	  } else
		thisVal = env->topFrame->nextThis;

	  v = fcnCall(fcn, args, thisVal);
	}

	if ((fc->hdr->flag & flag_typemask) == flag_newobj)
	  if (v.type == vt_array)
		v.aval->obj->proto = fcn.closure->proto;
	  else if (!v.type)
		v = thisVal;

	// abandon object temorary

	abandonValue(env->topFrame->nextThis);
	abandonValue(fcn);
	return v;
}

//  make function closures for functions defined in function

void installFcns(uint32_t decl, environment_t *env) {

	while (decl) {
		fcnDeclNode *fcn = (fcnDeclNode *)(env->closure->table + decl);
		symNode *sym = (symNode *)(env->closure->table + fcn->name);
		value_t v, slot;

		slot.bits = vt_lval;
		slot.lval = &env->topFrame->values[sym->frameidx];

		v = newClosure(fcn, env);
		replaceValue(slot, v);
		decl = fcn->next;
	}
}

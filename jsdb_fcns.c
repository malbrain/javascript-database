#include "jsdb.h"

static int debug = 0;

// closures

value_t newClosure( 
	fcnDeclNode *fcn,
	uint32_t level,
	Node *table,
	valueframe_t *oldScope)
{
	closure_t *result = jsdb_alloc(sizeof(closure_t) + sizeof(valueframe_t) * (level + 1), true);
	value_t v;

	for (int i=0; i<=level; i++) {
		result->frames[i] = oldScope[i];
		incrFrameCnt(oldScope[i]);
	}

	result->proto = newObject();
	incrRefCnt(result->proto);

	result->count = level + 1;
	result->table = table;
	result->fcn = fcn;

	v.bits = vt_closure;
	v.closure = result;
	v.refcount = 1;
	return v;
}

// function expressions

value_t eval_fcnexpr (Node *a, environment_t *env) {
	fcnDeclNode *fn = (fcnDeclNode *)a;
	uint32_t level = vec_count(env->framev) - 1;
	return newClosure(fn, level, env->table, env->framev);
}

// do function call

value_t fcnCall (value_t fcnClosure, value_t *args, value_t thisVal) {
	closure_t *closure = fcnClosure.closure;
	fcnDeclNode *fcn = closure->fcn;
	valueframe_t *newFramev = NULL;
	environment_t newenv[1];
	uint32_t body, params;
	frame_t *frame;
	value_t v;

	frame = jsdb_alloc(sizeof(value_t) * fcn->nsymbols + sizeof(frame_t), true);
	frame->count = fcn->nsymbols;
	frame->args->array = args;
	frame->thisVal = thisVal;
	frame->name = fcn->name;

	incrRefCnt(thisVal);

	for (int i = 0; i < closure->count; i++) {
		vec_push(newFramev, closure->frames[i]);
		incrFrameCnt(closure->frames[i]);
	}

	vec_push(newFramev, frame);
	incrFrameCnt(frame);
	body = fcn->body;

	// process parameter list

	if ((params = fcn->params)) do {
		listNode *ln = (listNode *)(closure->table + params);
		symNode *param = (symNode *)(closure->table + ln->elem);

		if (param->frameidx < vec_count(args))
			frame->values[param->frameidx] = args[param->frameidx];
		else
			break;

		incrRefCnt(args[param->frameidx]);
		params -= sizeof(listNode) / sizeof(Node);
	} while (closure->table[params].type);

	//  prepare new environment

	newenv->table = closure->table;
	newenv->framev = newFramev;

	//  install function expression closure

	if (fcn->hdr->type == node_fcnexpr)
		if (fcn->name) {
			value_t slot = dispatch(fcn->name, newenv);
			replaceSlotValue (slot.lval, fcnClosure);
		}

	installFcns(fcn->fcn, closure->table, frame);
	v = dispatch(body, newenv);

	if (v.type == vt_control && v.ctl == flag_return)
		v = frame->rtnVal;

	// tear down the frames

	for (int i = vec_count(newFramev); i--; )
		abandonFrame(newFramev[i]);

	vec_free(newFramev);
	decrRefCnt(v);
	return v;
}

// function calls

value_t eval_fcncall (Node *a, environment_t *env) {
	fcnCallNode *fc = (fcnCallNode *)a;
	value_t fcn, v, thisVal;
	value_t *args = NULL;
	uint32_t argList;
	fcnDeclNode *fd;

	// process arg list

	if ((argList = fc->args)) do {
		listNode *ln = (listNode *)(env->table + argList);
		v = dispatch(ln->elem, env);
		vec_push(args, v);
		incrRefCnt(v);

		argList -= sizeof(listNode) / sizeof(Node);
	} while (env->table[argList].type);

	//  capture "this" value from the name evaluation

	v.bits = vt_undef;
	replaceSlotValue(&env->framev[vec_count(env->framev) - 1]->nextThis, v);

	fcn = dispatch(fc->name, env);

	thisVal = env->framev[vec_count(env->framev) - 1]->nextThis;

	if (fcn.type == vt_propfcn)
		return ((propFcnEval)fcn.propfcn)(args, env->framev[vec_count(env->framev) - 1]->nextThis);

	if (fcn.type != vt_closure) {
		stringNode *sn = (stringNode *)(env->table);
		printf("%.*s not function closure line: %d\n", sn->hdr->aux, sn->string, a->lineno);
		exit(1);
	}

	if ((fc->hdr->flag & flag_typemask) == flag_newobj) {
		thisVal = newObject();
		thisVal.oval->proto = fcn.closure->proto;
		incrRefCnt(fcn.closure->proto);
		incrRefCnt(thisVal);
	}

	v = fcnCall(fcn, args, thisVal);

	for (int idx = 0; idx < vec_count(args); idx++)
		abandonValue(args[idx]);

	vec_free(args);

	if ((fc->hdr->flag & flag_typemask) == flag_newobj && !v.type) {
		decrRefCnt(thisVal);
		v = thisVal;
	}

	abandonValue(fcn);
	return v;
}

void installFcns(uint32_t decl, Node *table, valueframe_t frame) {

	while (decl) {
		fcnDeclNode *fcn = (fcnDeclNode *)(table + decl);
		symNode *sn = (symNode *)(table + fcn->name);
		value_t v, *slot = &frame->values[sn->frameidx];
		v.bits = vt_fcndef;
		v.aux = sn->level;
		v.fcn = fcn;

		replaceSlotValue(slot, v);
		decl = fcn->next;
	}
}

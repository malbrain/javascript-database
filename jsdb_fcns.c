#include "jsdb.h"

static int debug = 0;

// closures

value_t newClosure( 
	fcnDeclNode *func,
	valueframe_t *oldScope)
{
	closure_t *result = jsdb_alloc(sizeof(closure_t), true);
	value_t v;

	for (int i=0; i<vec_count(oldScope); i++) {
		vec_push(result->frames, oldScope[i]);
		incrRefCnt(oldScope[i]);
	}

	result->func = func;
	v.bits = vt_closure;
	v.closure = result;
	return v;
}

// function expressions

value_t eval_fcnexpr (Node *a, environment_t *env) {
	fcnDeclNode *fn = (fcnDeclNode *)a;
	return newClosure(fn, env->framev);
}

// function calls

value_t eval_fcncall (Node *a, environment_t *env) {
    fcnCallNode *fc = (fcnCallNode *)a;
    valueframe_t *newFramev = NULL;
	uint32_t body, args, params;
    environment_t newenv[1];
    value_t closure, v;
	fcnDeclNode *fd;
    frame_t *frame;
    int i;

    closure = dispatch(fc->name, env);

    if (closure.type != vt_closure) {
        return makeError(a, env, "not function closure");
        exit(1);
    }

    if (debug)
        printf ("closure: %d - params, %d - symbols", closure.closure->func->nparams, closure.closure->func->nsymbols);

    frame = jsdb_alloc(sizeof(value_t) * closure.closure->func->nsymbols + sizeof(frame_t), true);
    frame->count = closure.closure->func->nsymbols;

    for (i = 0; i < vec_count(closure.closure->frames); i++) {
        incrRefCnt(closure.closure->frames[i]);
        vec_push(newFramev, closure.closure->frames[i]);
    }

    body = closure.closure->func->body;
    vec_push(newFramev, frame);

	// process arg and parameter lists

    if ((args = fc->args))
	  if ((params = closure.closure->func->params)) do {
        listNode *ln = (listNode *)(env->table + params);
    	symNode *param = (symNode *)(env->table + ln->elem);

        ln = (listNode *)(env->table + args);
        v = dispatch(ln->elem, env);

        if (v.refcount)
            incr_ref_cnt(v);

		params -= sizeof(listNode) / sizeof(Node);
		args -= sizeof(listNode) / sizeof(Node);
        frame->values[param->frameidx] = v;

        if (debug) {
            printValue(v, 0);
            printf(", ");
        }
    } while (env->table[params].type && env->table[args].type);

    if (debug)
        printf("\n");

    //  prepare new environment

    newenv->framev = newFramev;
    newenv->table = env->table;

    //  install function expression name

    if (closure.closure->func->hdr->type == node_fcnexpr)
        if (closure.closure->func->name) {
           value_t slot = dispatch(closure.closure->func->name, newenv);
           replaceSlotValue (slot.lval, &closure);
        }

    installFcns(closure.closure->func->fcn, env->table, newFramev);
    v = dispatch(body, newenv);

    if (v.type == vt_control && v.ctl == flag_return)
        v = frame->rtnVal;

    for (i = 0; i < vec_count(newFramev); i++)
        if (!decrRefCnt(newFramev[i]))
            deleteFrame(newFramev[i]);

    return v;
}

void installFcns(uint32_t decl, Node *table, valueframe_t *framev) {

	while (decl) {
		fcnDeclNode *fn = (fcnDeclNode *)(table + decl);
		symNode *sn = (symNode *)(table + fn->name);
		value_t v, *slot = &framev[sn->level]->values[sn->frameidx];
		v.bits = vt_fcndef;
		v.f = fn;

		replaceSlotValue(slot, &v);
		decl = fn->next;
	}
}

#include "jsdb.h"

static int debug = 0;

// closures

value_t newClosure( 
	fcnDeclNode *fcn,
	uint32_t level,
	Node *table,
	valueframe_t *oldScope)
{
	closure_t *result = jsdb_alloc(sizeof(closure_t), true);
	value_t v;

	for (int i=0; i<level; i++) {
		vec_push(result->frames, oldScope[i]);
		incrRefCnt(oldScope[i]);
	}

	result->fcn = fcn;
	result->table = table;
	v.bits = vt_closure;
	v.closure = result;
	return v;
}

// function expressions

value_t eval_fcnexpr (Node *a, environment_t *env) {
	fcnDeclNode *fn = (fcnDeclNode *)a;
	uint32_t level = vec_count(env->framev);
	return newClosure(fn, level, env->table, env->framev);
}

// function calls

value_t eval_fcncall (Node *a, environment_t *env) {
    fcnCallNode *fc = (fcnCallNode *)a;
    valueframe_t *newFramev = NULL;
	uint32_t body, args, params;
    environment_t newenv[1];
	closure_t *closure;
	fcnDeclNode *fd;
    frame_t *frame;
    value_t fcn, v;
    int i;

    fcn = dispatch(fc->name, env);

	if (fcn.type == vt_propfcn)
		return ((propFcnEval)fcn.propfcn)(fc->args, env);

	closure = fcn.closure;

    if (fcn.type != vt_closure) {
        printf("not function closure [%d]\n", __LINE__);
        exit(1);
    }

    if (debug)
        printf ("closure: %d - params, %d - symbols", closure->fcn->nparams, closure->fcn->nsymbols);

    frame = jsdb_alloc(sizeof(value_t) * closure->fcn->nsymbols + sizeof(frame_t), true);
    frame->count = closure->fcn->nsymbols;

    for (i = 0; i < vec_count(closure->frames); i++) {
        incrRefCnt(closure->frames[i]);
        vec_push(newFramev, closure->frames[i]);
    }

    body = closure->fcn->body;
    vec_push(newFramev, frame);

	// process arg and parameter lists

    if ((args = fc->args))
	  if ((params = closure->fcn->params)) do {
        listNode *ln = (listNode *)(closure->table + params);
    	symNode *param = (symNode *)(closure->table + ln->elem);

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
    } while (closure->table[params].type && env->table[args].type);

    if (debug)
        printf("\n");

    //  prepare new environment

    newenv->table = closure->table;
    newenv->framev = newFramev;

    //  install function expression name

    if (closure->fcn->hdr->type == node_fcnexpr)
        if (closure->fcn->name) {
           value_t slot = dispatch(closure->fcn->name, newenv);
           replaceSlotValue (slot.lval, &fcn);
        }

    installFcns(closure->fcn->fcn, closure->table, frame);
    v = dispatch(body, newenv);

    if (v.type == vt_control && v.ctl == flag_return)
        v = frame->rtnVal;

    for (i = 0; i < vec_count(newFramev); i++)
        if (!decrRefCnt(newFramev[i]))
            deleteFrame(newFramev[i]);

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

		replaceSlotValue(slot, &v);
		decl = fcn->next;
	}
}

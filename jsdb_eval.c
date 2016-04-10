#include "jsdb.h"

static bool debug = false;

int ArraySize[] = {
	sizeof(value_t),
	sizeof(int8_t),
	sizeof(uint8_t),
	sizeof(int16_t),
	sizeof(uint16_t),
	sizeof(int32_t),
	sizeof(uint32_t),
	sizeof(float),
	sizeof(double)
};

//  strings

value_t newString(
	uint8_t *value,
	uint32_t len)
{
	value_t v;
	v.bits = vt_string;

	if (len) {
		v.str = jsdb_alloc(len + 1, false);
		v.refcount = 1;
		v.str[len] = 0;
	}

	memcpy(v.str, value, len);
	v.aux  = len;
	return v;
}

value_t eval_arg(uint32_t *args, environment_t *env) {
	value_t v;

	if (*args) {
		listNode *ln = (listNode *)(env->table + *args);
		v = dispatch(ln->elem, env);

		if (ln->hdr->type == node_list)
			*args -= sizeof(listNode) / sizeof(Node);
		else
			*args = 0;
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
	case nn_infinity:
		v.bits = vt_infinite;
		v.negative = false;
		return v;
	case nn_nan:
		v.bits = vt_nan;
		return v;
	case nn_undef:
		v.bits = vt_undef;
		return v;
	case nn_null:
		v.bits = vt_null;
		return v;
	case nn_this:
		return env->framev[vec_count(env->framev) - 1]->thisVal;
	case nn_args:
		v.bits = vt_array;
		v.aval = env->framev[vec_count(env->framev) - 1]->args;
		return v;
	}

	fprintf(stderr, "Error in numNode type: %d\n", nn->hdr->aux);
	exit(1);
}

value_t eval_badop (Node *a, environment_t *env) {
	return makeError(a, env, "Node type not implemented");
}

value_t eval_typeof (Node *a, environment_t *env) {
	exprNode *en = (exprNode *)a;
	value_t v;

	if (en->expr)
		v = dispatch(en->expr, env);
	else
		v.bits = vt_undef;

	v.str = strtype(v.type);

	v.bits = vt_string;
	v.aux = strlen(v.str);
	return v;
}
value_t eval_noop (Node *a, environment_t *env) {
	value_t v;

	v.bits = vt_null;
	return v;
}

//	object.field access

value_t eval_access (Node *a, environment_t *env) {
	binaryNode *bn = (binaryNode *)a;
	value_t *slot, obj = dispatch(bn->left, env);
	value_t v, field = dispatch(bn->right, env);
	uint32_t idx;

	//  remember this object for next fcnCall
	//	note:  we have no literal object access

	v.bits = vt_lval;
	v.lval = &env->framev[vec_count(env->framev) - 1]->nextThis;
	replaceValue(v, obj);

	if (field.type != vt_string) {
		v.bits = vt_undef;
		return v;
	}

	// document property

	if (obj.type == vt_document) {
		v = lookupDoc(obj.document, field);
		abandonValue(field);
		return v;
	}

	// object property

	if (obj.type == vt_object) {
	  if ((slot = lookup(obj.oval, field, a->flag & flag_lval))) {
		if (a->flag & flag_lval) {
		 	v.bits = vt_lval;
			v.lval = slot;
		} else
			v = *slot;

		abandonValue(field);
		return v;
	  }
	}

	// array property

	if (obj.type == vt_array) {
	  if ((slot = lookup(obj.aval->obj, field, a->flag & flag_lval))) {
		if (a->flag & flag_lval) {
		 	v.bits = vt_lval;
			v.lval = slot;
		} else
			v = *slot;

		abandonValue(field);
		return v;
	  }
	}

	// check built-in properties

	obj.lvalue = a->flag & flag_lval ? 1 : 0;
	v = builtinProp(obj, field, env);
	abandonValue(field);
	return v;
}

value_t eval_lookup (Node *a, environment_t *env) {
	binaryNode *bn = (binaryNode *)a;
	value_t *slot, obj = dispatch(bn->left, env);
	value_t v, field = dispatch(bn->right, env);

	// remember object for this pointer

	v.bits = vt_lval;
	v.lval = &env->framev[vec_count(env->framev) - 1]->nextThis;
	replaceValue(v, obj);

	// document property

	if (obj.type == vt_document) {
		if (field.type == vt_string) {
			v = lookupDoc(obj.document, field);
			abandonValue(field);
			return v;
		}
	}

	// object property

	if (obj.type == vt_object) {
	  if (field.type == vt_string)
		if ((slot = lookup(obj.oval, field, a->flag & flag_lval))) {
		  if (a->flag & flag_lval) {
		 	v.bits = vt_lval;
			v.lval = slot;
		  } else
			v = *slot;

		  abandonValue(field);
		  return v;
		}
	}

	// array index

	if (obj.type == vt_array) {
	  int idx = conv2Int(field, true).nval;

	  if (~a->flag & flag_lval) {
		if (idx < vec_count(obj.aval->array)) {
		  char *lval = obj.aval->array + idx * ArraySize[obj.subType];

		  if (obj.subType == array_value)
			return *(value_t *)lval;
		  else
			return convArray2Value(lval, obj.subType);
		} else
		  return v.bits = vt_undef, v;
	  } else {
		int diff = idx - vec_count(obj.aval->values) + 1;

		if (diff > 0)
			vec_add (obj.aval->values, diff);

		v.bits = vt_lval;
		v.subType = obj.subType;
		v.slot = obj.aval->array + idx * ArraySize[obj.subType];
		return v;
	  }
	}

	//  document array index

	if (obj.type == vt_docarray) {
		int idx = conv2Int(field, true).nval;

		if (a->flag & flag_lval)
			return makeError(a, env, "Invalid document mutation");

		if (idx < obj.docarray->count) {
		  char *lval = obj.aval->array + idx * ArraySize[obj.subType];

		  if (obj.subType == array_value)
			v = *(value_t *)lval;
		  else
			v = convArray2Value(lval, obj.subType);
		} else
			v.bits = vt_undef;

		return v;
	}

	if (field.type == vt_string) {
		v = builtinProp(obj, field, env);
		abandonValue(field);
		return v;
	}

	v.bits = vt_undef;
	return v;
}

value_t eval_array (Node *n, environment_t *env) {
	arrayNode *an = (arrayNode *)n;
	value_t v, a = newArray(array_value);
	listNode *ln;
	uint32_t l;

	if ((l = an->exprlist)) do {
		ln = (listNode *)(env->table + l);
		l -= sizeof(listNode) / sizeof(Node);
		v = dispatch(ln->elem, env);
		incrRefCnt(v);
		vec_push(a.aval->values, v);
	} while (ln->hdr->type == node_list);

	return a;
}

value_t eval_enum (Node *n, environment_t *env) {
	binaryNode *bn = (binaryNode *)n;
	value_t name, obj = newObject();
	value_t value, slot;
	uint32_t e, l;
	listNode *ln;

	slot = dispatch(bn->left, env);
	replaceValue (slot, obj);

	value.bits = vt_int;
	value.nval = -1;

	if ((l = bn->right)) do {
		ln = (listNode *)(env->table + l);

		l -= sizeof(listNode) / sizeof(Node);

		bn = (binaryNode *)(env->table + ln->elem);
		name = dispatch(bn->left, env);

		if (bn->right) {
			value_t index = dispatch(bn->right, env);
			value.nval = conv2Int(index, true).nval;
		} else
			value.nval++;

		value_t *w = lookup(obj.oval, name, true);
		replaceValue (*w, value);
		abandonValue(name);
	} while (ln->hdr->type == node_list);

	return obj;
}

value_t eval_obj (Node *n, environment_t *env) {
	objNode *on = (objNode *)n;
	value_t v, o = newObject();
	uint32_t e, l;
	listNode *ln;

	if ((l = on->elemlist)) do {
		ln = (listNode *)(env->table + l);

		l -= sizeof(listNode) / sizeof(Node);

		binaryNode *bn = (binaryNode *)(env->table + ln->elem);
		v = dispatch(bn->left, env);

		if (v.type == vt_string) {
			value_t *w = lookup(o.oval, v, true);
			replaceValue (*w, dispatch(bn->right, env));
		}

		abandonValue(v);
	} while (ln->hdr->type == node_list);

	return o;
}

value_t eval_list(Node *n, environment_t *env)
{
	uint32_t list;
	listNode *ln;
	value_t v;

	if (list = n - env->table) do {
		ln = (listNode *)(env->table + list);
		v = dispatch (ln->elem, env);

		if (v.type == vt_control || ln->hdr->type == node_endlist)
			return v;

		list -= sizeof(listNode) / sizeof(Node);
		abandonValue(v);
	} while (true);

	v.bits = vt_undef;
	return v;
}

value_t eval_ref(Node *a, environment_t *env)
{
	symNode *sn = (symNode*)a;
	value_t v;

	v.bits = vt_lval;
	v.lval = &env->framev[sn->level]->values[sn->frameidx];
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

	// delayed fcn closures

	if (slot->type == vt_fcndef) {
		value_t lvalue;
		lvalue.bits = vt_lval;
		lvalue.lval = slot;
		return replaceValue(lvalue, newClosure (slot->fcn, slot->aux, env->table, env->framev));
	}

	return *slot;
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
		bool cond = conv2Bool(condVal, true).boolean;

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

	v.bits = vt_undef;
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
		cond = conv2Bool(condVal, true).boolean;
	} while (cond);

	v.bits = vt_undef;
	return v;
}

value_t eval_ifthen(Node *a, environment_t *env)
{
	ifThenNode *iftn = (ifThenNode*)a;
	value_t condVal, v;
	uint32_t stmt;
	bool cond;

	condVal = dispatch(iftn->condexpr, env);
	cond = conv2Bool(condVal, true).boolean;
	v.bits = vt_undef;

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
		v.bits = vt_undef;

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

	while (true) {
		if (fn->cond) {
			condVal = dispatch(fn->cond, env);
			cond = conv2Bool(condVal, true).boolean;

			if (!cond)
				break;
		}

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

	v.bits = vt_undef;
	return v;
}


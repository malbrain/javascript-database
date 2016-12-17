#include "js.h"
#include "js_malloc.h"

static bool debug = false;
extern bool MathNums;

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

value_t newString(void *value, uint32_t len) {
	value_t v;
	v.bits = vt_string;

	if (len) {
		v.str = js_alloc(len + 1, false);
		v.refcount = 1;
		v.str[len] = 0;
	}

	memcpy(v.str, value, len);
	v.aux  = len;
	return v;
}

//  evaluate object slot value

value_t evalProp(value_t *slot, value_t base, bool lval) {
	value_t v;

	if (slot->type == vt_propval)
		return callFcnProp(*slot, base, lval);

	if (!lval)
		return *slot;

	v.bits = vt_lval;
	v.lval = slot;
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
		if(MathNums) {
			v.bits = vt_dbl;
			v.dbl = nn->intval;
		} else {
			v.bits = vt_int;
			v.nval = nn->intval;
		}
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
		return env->topFrame->thisVal;
	case nn_args:
		return env->topFrame->arguments;
	}

	fprintf(stderr, "Error in numNode type: %d\n", (int)nn->hdr->aux);
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
	v.aux = strlen(v.string);
	return v;
}

value_t eval_noop (Node *a, environment_t *env) {
	value_t v;

	v.bits = vt_null;
	return v;
}

//	finish lookup/access operation

value_t evalAccess(value_t obj, value_t field, bool lVal) {
	value_t v, *slot, original = obj;

	//  convert primitive type lookup to object prototype

	if (!obj.objvalue && obj.type != vt_object)
	  if (builtinProto[original.type].type)
		obj = builtinProto[original.type];

	// object property on object like things

	if (obj.objvalue)
		obj = *obj.lval;

	while (obj.type) {

	  // document property

	  if (obj.type == vt_document)
		return lookupDoc(obj.document, field);

	  // object property

	  if (obj.type == vt_object) {
		if ((slot = lookup(obj.oval, field, lVal, false))) {
		  return evalProp(slot, original, lVal);
	    }

		obj = obj.oval->protoChain;
	    continue;
	  }

	  break;
	}

	v.bits = vt_undef;
	return v;
}

//	object.field access

value_t eval_access (Node *a, environment_t *env) {
	binaryNode *bn = (binaryNode *)a;
	value_t obj = dispatch(bn->left, env);
	value_t v, field = dispatch(bn->right, env);
	bool lVal = a->flag & flag_lval;

	//  remember this object/value for next fcnCall

	replaceSlot(&env->topFrame->nextThis, obj);

	if (obj.type == vt_lval)
		obj = *obj.lval;

	if (field.type != vt_string) {
		v.bits = vt_undef;
		goto accessXit;
	}

	if (obj.type == vt_closure) {
	  if (field.aux == 9 && !memcmp(field.str, "prototype", 9)) {
		if (lVal) {
		  v.bits = vt_lval;
		  v.lval = &obj.closure->protoObj;
		} else {
		  v = obj.closure->protoObj;
		}

		goto accessXit;
	  }
	}

	v = evalAccess(obj, field, lVal);

accessXit:
	abandonValue(field);
	abandonValue(obj);
	return v;
}

//	expr[x]

value_t eval_lookup (Node *a, environment_t *env) {
	binaryNode *bn = (binaryNode *)a;
	value_t obj = dispatch(bn->left, env);
	value_t v, field = dispatch(bn->right, env);
	bool lVal = a->flag & flag_lval;

	//  remember this object for next fcnCall

	replaceSlot(&env->topFrame->nextThis, obj);

	if (obj.type == vt_lval)
		obj = *obj.lval;

	// string character index

	if (obj.type == vt_string) {
		value_t idx = conv2Int(field, true);

		if (idx.type == vt_int)
		  if (!lVal)
			if (idx.nval < obj.aux) {
			  v = newString(obj.str + idx.nval, 1);
			  goto lookupXit;
			}
	 }

	// array numeric index

	if (obj.type == vt_array) {
		value_t idx = conv2Int(field, true);

		if (idx.type == vt_int && idx.nval >= 0) {
		  if (lVal) {
	 		int diff = idx.nval - vec_count(obj.aval->values) + 1;

	 		if (diff > 0)
				vec_add (obj.aval->values, diff);

			v.bits = vt_lval;
			v.subType = obj.subType;
			v.slot = obj.aval->array + idx.nval * ArraySize[obj.subType];
		  } else if (idx.nval < vec_count(obj.aval->array)) {
			char *lval = obj.aval->array + idx.nval * ArraySize[obj.subType];

			if (obj.subType == array_value)
			  v = *(value_t *)lval;
			else
			  v = convArray2Value(lval, obj.subType);
		  } else {
			v.bits = vt_undef;
		  }

		  goto lookupXit;
		}
	  }

	//  document array index

	if (obj.type == vt_docarray) {
		int idx = conv2Int(field, true).nval;

		if (lVal)
			return makeError(a, env, "Invalid document mutation");

		if (idx < obj.docarray->count) {
		  char *lval = obj.aval->array + idx * ArraySize[obj.subType];

		  if (obj.subType == array_value)
			v = *(value_t *)lval;
		  else
			v = convArray2Value(lval, obj.subType);
		} else
			v.bits = vt_undef;

		goto lookupXit;
	}

	v = evalAccess(obj, field, lVal);

lookupXit:
	abandonValue(field);
	abandonValue(obj);
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
	value_t name, obj = newObject(builtinProto[vt_object]);
	exprNode *en = (exprNode *)n;
	value_t value;
	listNode *ln;
	uint32_t l;

	value.bits = vt_int;
	value.nval = -1;

	if ((l = en->expr)) do {
		ln = (listNode *)(env->table + l);

		binaryNode *bn = (binaryNode *)(env->table + ln->elem);
		name = dispatch(bn->left, env);

		if (bn->right) {
			value_t index = dispatch(bn->right, env);
			value.nval = conv2Int(index, true).nval;
		} else
			value.nval++;

		replaceSlot(lookup(obj.oval, name, true, false), value);
		abandonValue(name);

		l -= sizeof(listNode) / sizeof(Node);
	} while (ln->hdr->type == node_list);

	return obj;
}

value_t eval_obj (Node *n, environment_t *env) {
	value_t v, o = newObject(builtinProto[vt_object]);
	objNode *on = (objNode *)n;
	listNode *ln;
	uint32_t l;

	if ((l = on->elemlist)) do {
		ln = (listNode *)(env->table + l);

		l -= sizeof(listNode) / sizeof(Node);

		binaryNode *bn = (binaryNode *)(env->table + ln->elem);
		v = dispatch(bn->left, env);

		if (v.type == vt_string)
			replaceSlot (lookup(o.oval, v, true, false), dispatch(bn->right, env));

		abandonValue(v);
	} while (ln->hdr->type == node_list);

	return o;
}

value_t eval_tern(Node *n, environment_t *env)
{
	ternaryNode *tn = (ternaryNode *)n;
	value_t v = dispatch (tn->condexpr, env);

	if (conv2Bool(v, true).boolean)
		return dispatch (tn->trueexpr, env);
	else
		return dispatch (tn->falseexpr, env);
}

value_t eval_list(Node *n, environment_t *env)
{
	uint32_t list;
	listNode *ln;
	value_t v;

	if ((list = n - env->table)) do {
		ln = (listNode *)(env->table + list);
		v = dispatch (ln->elem, env);

		if (v.type == vt_control || ln->hdr->type != node_list)
			return v;

		list -= sizeof(listNode) / sizeof(Node);
		abandonValue(v);
	} while (true);

	v.bits = vt_undef;
	return v;
}

value_t eval_ref(Node *a, environment_t *env)
{
	symNode *sym = (symNode*)a;
	value_t v;

	if (sym->frameIdx == 0) {
		stringNode *sn = (stringNode *)(env->table + sym->name);
		fprintf(stderr, "line %d symbol not assigned: %s\n", (int)a->lineNo, sn->string);
		exit(1);
	}

	v.bits = vt_lval;

	if (sym->level)
		v.lval = &env->closure->frames[sym->level - 1]->values[sym->frameIdx];
	else
		v.lval = &env->topFrame->values[sym->frameIdx];

	return v;
}

value_t eval_var(Node *a, environment_t *env)
{
	symNode *sym = (symNode*)a;
	value_t v, *slot;
	stringNode *sn;

	sn = (stringNode *)(env->table + sym->name);

	if (sym->frameIdx == 0) {
		fprintf(stderr, "line %d symbol not assigned: %s\n", (int)a->lineNo, sn->string);
		exit(1);
	}

	if (sym->level)
		slot = &env->closure->frames[sym->level - 1]->values[sym->frameIdx];
	else
		slot = &env->topFrame->values[sym->frameIdx];

	if (slot->refcount)
	  if (!slot->raw[-1].refCnt[0]) {
		fprintf(stderr, "line %d deleted variable: %s\n", (int)a->lineNo, sn->string);
		exit(1);
	  }

	if (a->flag & flag_lval) {
		v.bits = vt_lval;
		v.lval = slot;
		return v;
	}

	return *slot;
}

value_t eval_string(Node *a, environment_t *env)
{
	stringNode *sn = (stringNode *)a;
	value_t v;
	
	v.bits = vt_string;
	v.aux = sn->hdr->aux;
	v.string = sn->string;
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

	env->topFrame->values[0] = v;
	
	v.bits = vt_control;
	v.ctl = a->flag & flag_typemask;
	return v;
}

value_t eval_forin(Node *a, environment_t *env)
{
	forInNode *fn = (forInNode*)a;
	value_t slot, v, val;

	slot = dispatch(fn->var, env);

	if (slot.type != vt_lval)
		return makeError(a, env, "Not l-value");

	val = dispatch(fn->expr, env);

	switch (val.type) {
	case vt_array: {
		value_t *values = val.aval->values;

		for (int idx = 0; idx < vec_count(values); idx++) {
		  if (fn->hdr->aux == for_in) {
			if (values[idx].type == vt_undef)
				continue;

			v.bits = vt_int;
			v.nval = idx;
		  } else
			v = values[idx];

		  replaceValue (slot, v);

		  v = dispatch(fn->stmt, env);

		  if (v.type == vt_control) {
			if (v.ctl == flag_break)
				break;
			else if (v.ctl == flag_return)
				return v;
		  }
		}

		break;
	}
	case vt_object: {
		for (int idx = 0; idx < vec_count(val.oval->pairs); idx++) {
		  if (fn->hdr->aux == for_in)
			replaceValue (slot, val.oval->pairs[idx].name);
		  else
			replaceValue (slot, val.oval->pairs[idx].value);

		  v = dispatch(fn->stmt, env);

		  if (v.type == vt_control) {
			if (v.ctl == flag_break)
				break;
			else if (v.ctl == flag_return)
				return v;
		  }
		}
	}
	default: break;
	}

	v.bits = vt_undef;
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


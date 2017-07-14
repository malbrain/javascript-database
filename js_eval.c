#include "js.h"
#include "js_malloc.h"

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
		if(mathNums) {
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
		if (env->topFrame->thisVal.type == vt_lval)
			return *env->topFrame->thisVal.lval;

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
	value_t v, result;

	if (en->expr)
		v = dispatch(en->expr, env);
	else
		v.bits = vt_undef;

	result = newString(strtype(v.type), -1);
	abandonValue(v);


	return result;
}

value_t eval_noop (Node *a, environment_t *env) {
	value_t v;

	v.bits = vt_null;
	return v;
}

//	object.field access

value_t eval_access (Node *a, environment_t *env) {
	binaryNode *bn = (binaryNode *)a;
	value_t v, field, obj = dispatch(bn->left, env);
	bool lVal = (a->flag & flag_lval);

	field = dispatch(bn->right, env);

	if (obj.type == vt_lval)
		obj = *obj.lval;

	if (field.type == vt_string) {
		v = lookupAttribute(obj, field, lVal, false);
		env->topFrame->nextThis = obj;
	} else
		v.bits = vt_undef;

	abandonValue(field);
	return v;
}

//	expr[x]

value_t eval_lookup (Node *a, environment_t *env) {
	binaryNode *bn = (binaryNode *)a;
	value_t v, field, obj = dispatch(bn->left, env);
	bool lVal = (a->flag & flag_lval);

	field = dispatch(bn->right, env);

	if (obj.type == vt_lval)
		obj = *obj.lval;

	if (obj.type == vt_document)
		obj = convDocument(obj, lVal);

	// string character index

	if (obj.type == vt_string) {
		value_t idx = conv2Int(field, false);
		string_t *str = js_addr(obj);

		if (idx.type == vt_int)
		  if (!lVal)
			if (idx.nval < str->len) {
			  v = newString(str->val + idx.nval, 1);
			  goto lookupXit;
			}
	 }

	// array numeric index

	if (obj.type == vt_array) {
	  dbarray_t *dbaval = js_addr(obj);
	  value_t *values = obj.marshaled ? dbaval->valueArray : obj.aval->valuePtr;
	  uint32_t cnt = obj.marshaled ? dbaval->cnt : vec_cnt(values);
	  value_t idx;
	  int diff;

	  idx = conv2Int(field, false);

	  if (idx.type == vt_int && idx.nval >= 0) {
		if (lVal) {
		  if (obj.marshaled)
			obj = convDocument(obj, true);
	 	  diff = idx.nval - cnt + 1;

	 	  if (diff > 0)
			vec_add (obj.aval->valuePtr, diff);

		  v.bits = vt_lval;
		  v.subType = obj.subType;
		  v.addr = obj.aval->array + idx.nval * ArraySize[obj.subType];
	    } else if (idx.nval < cnt) {
		  char *lval = (char *)values + idx.nval * ArraySize[obj.subType];

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

	field = conv2Str(field, true, false);
	v = lookupAttribute(obj, field, lVal, false);
	env->topFrame->nextThis = obj;

lookupXit:
	abandonValue(field);
	return v;
}

value_t eval_array (Node *n, environment_t *env) {
	value_t v, a = newArray(array_value, n->aux);
	arrayNode *an = (arrayNode *)n;
	array_t *aval = a.addr;
	listNode *ln;
	uint32_t l;

	if ((l = an->exprlist)) do {
		int idx = vec_size(aval->valuePtr)++;
		ln = (listNode *)(env->table + l);
		l -= sizeof(listNode) / sizeof(Node);
		v = dispatch(ln->elem, env);

		replaceSlot(aval->valuePtr + idx, v);
	} while (ln->hdr->type == node_list);

	vec_push(env->literals, a);
	incrRefCnt(a);
	return a;
}

value_t eval_enum (Node *n, environment_t *env) {
	value_t name, obj = newObject(vt_object);
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

		replaceValue(lookup(obj, name, true, 0), value);
		abandonValue(name);

		l -= sizeof(listNode) / sizeof(Node);
	} while (ln->hdr->type == node_list);

	return obj;
}

value_t eval_obj (Node *n, environment_t *env) {
	value_t left, obj = newObject(vt_object);
	objNode *on = (objNode *)n;
	uint32_t l, cap, hashEnt;
	pair_t *pairs = NULL;
	string_t *namestr;
	void *hashTbl;
	value_t right;
	listNode *ln;

	if (n->aux)
		pairs = newVector (n->aux, sizeof(pair_t), true);

	cap = vec_max(pairs);
	obj.oval->pairsPtr = pairs;
	hashTbl = pairs + cap;
	hashEnt = hashBytes(cap);

	if ((l = on->elemlist)) do {
		int idx = vec_size(obj.oval->pairsPtr)++;
		ln = (listNode *)(env->table + l);

		l -= sizeof(listNode) / sizeof(Node);

		binaryNode *bn = (binaryNode *)(env->table + ln->elem);
		left = dispatch(bn->left, env);

		if (left.type == vt_string) {
		  int h = -lookupValue(obj, left, 0, false);
		  right = dispatch(bn->right, env);
		  replaceSlot (&pairs[idx].name, left);
		  replaceSlot (&pairs[idx].value, right);
		  hashStore(hashTbl, hashEnt, h, idx + 1);
		} else
		  abandonValue(left);

	} while (ln->hdr->type == node_list);

	vec_push(env->literals, obj);
	incrRefCnt(obj);
	return obj;
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

//	block scope entry/exit

value_t eval_block(Node *n, environment_t *env)
{
	blkEntryNode *be = (blkEntryNode *)n;
	symtab_t *oldSym = env->scope->symbols;
	value_t val;

	env->scope->count = be->symbols.frameIdx;
	env->scope->symbols = &be->symbols;

	val = dispatch(be->body, env);

	env->scope->count = oldSym ? oldSym->frameIdx : 0;
	env->scope->symbols = oldSym ? oldSym : NULL;

	//	abandon and reset the block scope variables

	for (int idx = 0; idx < be->symbols.scopeCnt; idx++) {
		if (decrRefCnt(env->scope->values[idx + be->symbols.baseIdx]))
			deleteValue(env->scope->values[idx + be->symbols.baseIdx]);

		env->scope->values[idx].bits = vt_undef;
	}

	return val;
}

value_t eval_list(Node *n, environment_t *env)
{
	listNode *ln = NULL;
	uint32_t list;
	value_t v;

	v.bits = vt_undef;

	if ((list = n - env->table)) do {
		ln = (listNode *)(env->table + list);
		v = dispatch (ln->elem, env);

		if (v.type == vt_control || ln->hdr->type != node_list)
			break;

		list -= sizeof(listNode) / sizeof(Node);
		abandonValue(v);
	} while (true);

	if (ln && ln->hdr->aux & aux_endstmt)
		abandonLiterals(env);

	return v;
}

value_t eval_var(Node *a, environment_t *env)
{
	symNode *sym = (symNode*)a;
	value_t v, *slot;
	stringNode *sn;

	sn = (stringNode *)(env->table + sym->name);

	if (sym->frameIdx == 0) {
		fprintf(stderr, "%s: line %d symbol not assigned: %s\n", env->first->script, (int)a->lineNo, sn->str.val);
		exit(1);
	}

	if (sym->level)
	  if (sym->hdr->flag & flag_scope)
		slot = &env->closure->scope[sym->level - 1]->values[sym->frameIdx];
	  else
		slot = &env->closure->scope[sym->level - 1]->frame->values[sym->frameIdx];
	else
	  if (sym->hdr->flag & flag_scope)
		slot = &env->scope->values[sym->frameIdx];
	  else
		slot = &env->topFrame->values[sym->frameIdx];

	if (slot->refcount)
	  if (!slot->raw[-1].refCnt[0]) {
		fprintf(stderr, "%s: line %d variable deleted: %s\n", env->first->script, (int)a->lineNo, sn->str.val);
		exit(1);
	  }

	if (a->flag & flag_lval) {
		v.bits = vt_lval;
		v.lval = slot;
		return v;
	}

	if ((a->flag & flag_operand) && slot->marshaled)
		slot = js_dbaddr(*slot, slot->addr);

	return *slot;
}

value_t eval_string(Node *a, environment_t *env)
{
	stringNode *sn = (stringNode *)a;
//	value_t v = newString(sn->str.val, sn->str.len);
	value_t v;

	v.bits = vt_string;
	v.addr = &sn->str;
	return v;
}

value_t eval_while(Node *a, environment_t *env)
{
	whileNode *wn = (whileNode*)a;
	if (evalDebug) printf("node_while\n");
	value_t v;

	if (wn->cond)
	  while (true) {
		value_t condVal = dispatch(wn->cond, env);
		bool cond = conv2Bool(condVal, true).boolean;

		if (!cond)
			break;

		v = dispatch(wn->stmt, env);

		if (v.type == vt_control) {
			if (v.ctl == ctl_break)
				break;
			else if (v.ctl == ctl_return)
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
			if (v.ctl == ctl_break)
				break;
			else if (v.ctl == ctl_return)
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

value_t eval_forin(Node *a, environment_t *env)
{
	symtab_t *oldSym = env->scope->symbols;
	forInNode *fn = (forInNode*)a;
	value_t slot, val;

	env->scope->count = fn->symbols.frameIdx;
	env->scope->symbols = &fn->symbols;

	slot = dispatch(fn->var, env);

	if (slot.type != vt_lval)
		val = makeError(a, env, "Not l-value");
	else
		val = dispatch(fn->expr, env);

	switch (val.type) {
	case vt_array: {
		dbarray_t *dbaval = js_addr(val);
		value_t *values = val.marshaled ? dbaval->valueArray : val.aval->valuePtr;
		uint32_t cnt = val.marshaled ? dbaval->cnt : vec_cnt(values);

		for (int idx = 0; idx < cnt; idx++) {
		  if (fn->hdr->aux == for_in) {
			if (values[idx].type == vt_undef)
				continue;

			val.bits = vt_int;
			val.nval = idx;
		  } else
			val = values[idx];

		  replaceValue (slot, val);

		  val = dispatch(fn->stmt, env);

		  if (val.type == vt_control) {
			if (val.ctl == ctl_break)
				break;
			else if (val.ctl == ctl_return)
				goto forxit;
		  }
		}

		break;
	}
	case vt_object: {
		dbobject_t *dboval = js_addr(val);
		pair_t *pairs = val.marshaled ? dboval->pairs : val.oval->pairsPtr;
		uint32_t cnt = val.marshaled ? dboval->cnt : vec_cnt(pairs);

		for (int idx = 0; idx < cnt; idx++) {
		  value_t v;

		  if (fn->hdr->aux == for_in) {
			v = pairs[idx].name;
			if (v.marshaled)
				v.addr = val.addr;
		  } else {
			v = pairs[idx].value;
			if (v.marshaled)
				v.addr = val.addr;
		  }

		  replaceValue (slot, v);
		  val = dispatch(fn->stmt, env);

		  if (val.type == vt_control) {
			if (val.ctl == ctl_break)
				break;
			else if (val.ctl == ctl_return)
				goto forxit;
		  }
		}
	}
	default: break;
	}

forxit:
	env->scope->count = oldSym ? oldSym->frameIdx : 0;
	env->scope->symbols = oldSym ? oldSym : NULL;

	//	abandon and reset the block scope variables

	for (int idx = 0; idx < fn->symbols.scopeCnt; idx++) {
		if (decrRefCnt(env->scope->values[idx + fn->symbols.baseIdx]))
			deleteValue(env->scope->values[idx + fn->symbols.baseIdx]);

		env->scope->values[idx].bits = vt_undef;
	}

	return val;
}

value_t eval_for(Node *a, environment_t *env)
{
	symtab_t *oldSym = env->scope->symbols;
	forNode *fn = (forNode*)a;
	value_t condVal, val;
	bool cond;

	val.bits = vt_undef;

	env->scope->count = fn->symbols.frameIdx;
	env->scope->symbols = &fn->symbols;

	if (fn->init) {
		val = dispatch(fn->init, env);

		if (val.type == vt_control)
			goto forxit;

		abandonValue(val);
	}

	while (true) {
		if (fn->cond) {
			condVal = dispatch(fn->cond, env);
			cond = conv2Bool(condVal, true).boolean;

			if (!cond)
				break;
		}

		val = dispatch(fn->stmt, env);

		if (val.type == vt_control) {
			if (val.ctl == ctl_break)
				break;
			else if (val.ctl == ctl_return)
				goto forxit;
		}

		abandonValue(val);

		if (fn->incr) {
			val = dispatch(fn->incr, env);

			if (val.type == vt_control)
				goto forxit;

			abandonValue(val);
		}
	}

forxit:
	env->scope->count = oldSym ? oldSym->frameIdx : 0;
	env->scope->symbols = oldSym ? oldSym : NULL;

	//	abandon and reset the block scope variables

	for (int idx = 0; idx < fn->symbols.scopeCnt; idx++) {
		if (decrRefCnt(env->scope->values[idx + fn->symbols.baseIdx]))
			deleteValue(env->scope->values[idx + fn->symbols.baseIdx]);

		env->scope->values[idx].bits = vt_undef;
	}

	return val;
}


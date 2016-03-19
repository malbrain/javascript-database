#include "jsdb.h"

static int debug = 0;

value_t conv2Bool(value_t cond) {
	value_t result;

	result.bits = vt_bool;

	switch (cond.type) {
	case vt_dbl: result.boolean = cond.dbl != 0; return result;
	case vt_int: result.boolean = cond.nval != 0; return result;
	case vt_status: result.boolean = cond.status == OK; return result;
	case vt_file: result.boolean = cond.file != NULL; return result;
	case vt_array: result.boolean = cond.aval != NULL; return result;
	case vt_object: result.boolean = cond.oval != NULL; return result;
	case vt_document: result.boolean = cond.document != NULL; return result;
	case vt_docarray: result.boolean = cond.docarray != NULL; return result;
	case vt_string: result.boolean = cond.aux > 0; return result;
	case vt_uninitialized: result.boolean = false; return result;
	case vt_closure: result.boolean = cond.closure != NULL; return result;
	case vt_docId: result.boolean = cond.docId.bits > 0; return result;
	case vt_bool: return cond;
	}

	result.boolean = false;
	return result;
}

value_t conv2Dbl (value_t val) {
	value_t result;

	result.bits = vt_dbl;

	switch (val.type) {
	case vt_dbl: result.dbl = val.dbl; return result;
	case vt_int: result.dbl = val.nval; return result;
	case vt_bool: result.dbl = val.boolean; return result;
	}

	result.dbl = 0;
	return result;
}

value_t conv2Int (value_t val) {
	value_t result;

	result.bits = vt_int;

	switch (val.type) {
	case vt_int: result.nval = val.nval; return result;
	case vt_dbl: result.nval = val.dbl; return result;
	case vt_bool: result.nval = val.boolean; return result;
	}

	result.nval = 0;
	return result;
}

value_t conv2Str (value_t val) {
	value_t result;

	result.bits = vt_string;
	return result;
}

value_t conv(value_t val, valuetype_t type) {
	switch (type) {
	case vt_bool: return conv2Bool(val);
	case vt_dbl: return conv2Dbl(val);
	case vt_int: return conv2Int(val);
	case vt_string: return conv2Str(val);
	}

	return val;
}

value_t op_add (Node *a, environment_t *env, value_t left, value_t right) {
	value_t val;

	switch (left.type) {
	case vt_string:
		val.bits = vt_string;
		val.str = jsdb_alloc(left.aux + right.aux, false);
		val.refcount = true;
		val.aux = left.aux + right.aux;
		memcpy (val.str, left.str, left.aux);
		memcpy (val.str + left.aux, right.str, right.aux);
		return val;

	case vt_int:
		val.bits = vt_int;
		val.nval = left.nval + right.nval;
		return val;

	case vt_dbl:
		val.bits = vt_dbl;
		val.dbl = left.dbl + right.dbl;
		return val;
	}

	return makeError (a, env, "Invalid addition");
}

value_t op_sub (Node *a, environment_t *env, value_t left, value_t right) {
	value_t val;

	switch (left.type) {
	case vt_int:
		val.bits = vt_int;
		val.nval = left.nval - right.nval;
		return val;

	case vt_dbl:
		val.bits = vt_dbl;
		val.dbl = left.dbl - right.dbl;
		return val;
	}

	return makeError (a, env, "Invalid subtraction");
}

value_t op_mpy (Node *a, environment_t *env, value_t left, value_t right) {
	value_t val;

	switch (left.type) {
	case vt_int:
		val.bits = vt_int;
		val.nval = left.nval * right.nval;
		return val;

	case vt_dbl:
		val.bits = vt_dbl;
		val.dbl = left.dbl * right.dbl;
		return val;
	}

	return makeError (a, env, "Invalid multliplication");
}

value_t op_div (Node *a, environment_t *env, value_t left, value_t right) {
	value_t val;

	switch (left.type) {
	case vt_int:
		val.bits = vt_int;
		if (right.nval) {
			val.nval = left.nval / right.nval;
			return val;
		}
		break;

	case vt_dbl:
		val.bits = vt_dbl;
		if (right.dbl) {
			val.dbl = left.dbl / right.dbl;
			return val;
		}
		break;
	}

	return makeError (a, env, "Invalid division");
}

int op_compare (value_t left, value_t right) {
	int c;

	if (left.aux == right.aux)
		return memcmp(left.str, right.str, left.aux);

	if (left.aux < right.aux) {
		c = memcmp(left.str, right.str, left.aux);
		if (!c)
			return -1;
		return c;
	}
	
	c = memcmp(left.str, right.str, right.aux);
	if (!c)
		return 1;
	return c;
}

bool op_lt (value_t left, value_t right) {
	switch (left.type) {
	case vt_int:
		return left.nval < right.nval;

	case vt_dbl:
		return left.dbl < right.dbl;

	case vt_string:
		return op_compare (left, right) < 0;
	}

	return false;
}

bool op_le (value_t left, value_t right) {
	switch (left.type) {
	case vt_int:
		return left.nval <= right.nval;

	case vt_dbl:
		return left.dbl <= right.dbl;

	case vt_string:
		return op_compare (left, right) <= 0;
	}

	return false;
}

bool op_eq (value_t left, value_t right) {
	switch (left.type) {
	case vt_int:
		return left.nval == right.nval;

	case vt_dbl:
		return left.dbl == right.dbl;

	case vt_string:
		return op_compare (left, right) == 0;
	}

	return false;
}

bool op_ne (value_t left, value_t right) {
	switch (left.type) {
	case vt_int:
		return left.nval != right.nval;

	case vt_dbl:
		return left.dbl != right.dbl;

	case vt_string:
		return op_compare (left, right) != 0;
	}

	return false;
}

bool op_ge (value_t left, value_t right) {
	switch (left.type) {
	case vt_int:
		return left.nval >= right.nval;

	case vt_dbl:
		return left.dbl >= right.dbl;

	case vt_string:
		return op_compare (left, right) >= 0;
	}

	return false;
}

bool op_gt (value_t left, value_t right) {
	switch (left.type) {
	case vt_int:
		return left.nval > right.nval;

	case vt_dbl:
		return left.dbl > right.dbl;

	case vt_string:
		return op_compare (left, right) > 0;
	}

	return false;
}

typedef value_t (*Mathfcnp)(Node *a, environment_t *env, value_t left, value_t right);
typedef bool (*Boolfcnp)(value_t left, value_t right);

Mathfcnp mathLink[] = {
op_add, op_sub, op_mpy, op_div
};

Boolfcnp boolLink[] = {
op_lt, op_le, op_eq, op_ne, op_ge, op_gt
};

value_t eval_math(Node *a, environment_t *env) {
	binaryNode *bn = (binaryNode *)a;
	value_t right = dispatch(bn->right, env);
	value_t left = dispatch(bn->left, env);
	value_t v;

	if (right.type > left.type)
		right = conv(right, left.type);
	else if (left.type > right.type)
		left = conv(left, right.type);

	if (a->aux < math_comp)
		return mathLink[a->aux](a, env, left, right);

	v.bits = vt_bool;
	v.boolean = boolLink[a->aux - math_comp - 1](left, right);
	return v;
}

value_t eval_neg(Node *a, environment_t *env) {
	exprNode *en = (exprNode *)a;
	value_t v = dispatch(en->expr, env);

	switch (v.type) {
	case vt_dbl: v.dbl = -v.dbl; return v;
	case vt_int: v.nval = -v.nval; return v;
	case vt_bool: v.boolean = !v.boolean; return v;
	}

	return makeError(a, env, "Invalid Negation");
}

value_t eval_assign(Node *a, environment_t *env)
{
	binaryNode *bn = (binaryNode*)a;
	value_t val, *w;

	if (debug) printf("node_assign\n");
	val = dispatch(bn->left, env);

	if (val.type != vt_lval)
		return makeError(a, env, "not lvalue");

	w = val.lval;
	val = dispatch(bn->right, env);

	if (bn->hdr->aux == pm_assign)
		return replaceSlotValue(w, &val);

	if (w->type != val.type)
		val = conv(val, w->type);

	switch (bn->hdr->aux) {
	case pm_add: *w = op_add (a, env, *w, val); break;
	case pm_sub: *w = op_sub (a, env, *w, val); break;
	case pm_mpy: *w = op_mpy (a, env, *w, val); break;
	case pm_div: *w = op_div (a, env, *w, val); break;
	}

	return *w;
}


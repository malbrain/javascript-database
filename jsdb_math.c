#ifdef _WIN32
#define _CRT_RAND_S
#endif

#include "jsdb.h"
#include "jsdb_math.h"

static int debug = 0;

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
		right = conv2Str(right);

		val.bits = vt_string;
		val.str = jsdb_alloc(left.aux + right.aux, false);
		val.refcount = true;
		val.aux = left.aux + right.aux;

		memcpy (val.str, left.str, left.aux);
		memcpy (val.str + left.aux, right.str, right.aux);
		return val;

	case vt_int:
		val.bits = vt_int;
		val.nval = left.nval + conv2Int(right).nval;
		return val;

	case vt_dbl:
		val.bits = vt_dbl;
		val.dbl = left.dbl + conv2Dbl(right).dbl;
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

	if (right.type > left.type) {
		v = conv(right, left.type);
		abandonValue(right);
		right = v;
	} else if (left.type > right.type) {
		v = conv(left, right.type);
		abandonValue(left);
		left = v;
	}

	v.bits = vt_bool;

	if (a->aux < math_comp)
		v = mathLink[a->aux](a, env, left, right);
	else
		v.boolean = boolLink[a->aux - math_comp - 1](left, right);

	abandonValue(right);
	abandonValue(left);
	return v;
}

value_t eval_neg(Node *a, environment_t *env) {
	exprNode *en = (exprNode *)a;
	value_t v = dispatch(en->expr, env);

	switch (v.type) {
	case vt_dbl: v.dbl = -v.dbl; return v;
	case vt_int: v.nval = -v.nval; return v;
	case vt_bool: v.boolean = !v.boolean; return v;
	case vt_infinite: v.negative = !v.negative; return v;
	}

	abandonValue(v);
	return makeError(a, env, "Invalid Negation");
}

value_t eval_assign(Node *a, environment_t *env)
{
	binaryNode *bn = (binaryNode*)a;
	value_t val, *w;

	if (debug) printf("node_assign\n");
	val = dispatch(bn->left, env);

	if (val.type != vt_lval) {
		abandonValue(val);
		return makeError(a, env, "not lvalue");
	}

	w = val.lval;
	val = dispatch(bn->right, env);

	if (bn->hdr->aux == pm_assign)
		return replaceSlotValue(w, val);

	if (w->type != val.type)
		val = conv(val, w->type);

	switch (bn->hdr->aux) {
	case pm_add: replaceSlotValue(w, op_add (a, env, *w, val)); break;
	case pm_sub: replaceSlotValue(w, op_sub (a, env, *w, val)); break;
	case pm_mpy: replaceSlotValue(w, op_mpy (a, env, *w, val)); break;
	case pm_div: replaceSlotValue(w, op_div (a, env, *w, val)); break;
	}

	return *w;
}

#include <errno.h>
#include <math.h>

value_t jsdb_mathop (uint32_t args, environment_t *env) {
	value_t arglist, op, s, x, y, rval;
	int openum;

	arglist = eval_arg(&args, env);
	s.bits = vt_status;

	if (arglist.type != vt_array) {
		fprintf(stderr, "Error: mathop => expecting argument array => %s\n", strtype(arglist.type));
		return s.status = ERROR_script_internal, s;
	}

	op = eval_arg(&args, env);
	rval.bits = vt_dbl;

	openum = conv2Int(op).nval;
	abandonValue(op);
	errno = 0;

	x = eval_arg(&args, env);
	y = eval_arg(&args, env);

	switch (openum) {
	case math_acos: {
		rval.dbl = acos(conv2Dbl(x).dbl);
		break;
	}
	case math_acosh: {
		rval.dbl = acosh(conv2Dbl(x).dbl);
		break;
	}
	case math_asin: {
		rval.dbl = asin(conv2Dbl(x).dbl);
		break;
	}
	case math_asinh: {
		rval.dbl = asinh(conv2Dbl(x).dbl);
		break;
	}
	case math_atan: {
		rval.dbl = atan(conv2Dbl(x).dbl);
		break;
	}
	case math_atanh: {
		rval.dbl = atanh(conv2Dbl(x).dbl);
		break;
	}
	case math_atan2: {
		rval.dbl = atan2(conv2Dbl(x).dbl, conv2Dbl(y).dbl);
		break;
	}
	case math_cbrt: {
		rval.dbl = cbrt(conv2Dbl(x).dbl);
		break;
	}
	case math_ceil: {
		rval.dbl = ceil(conv2Dbl(x).dbl);
		break;
	}
	case math_clz32: {
		break;
	}
	case math_cos: {
		rval.dbl = cos(conv2Dbl(x).dbl);
		break;
	}
	case math_cosh: {
		rval.dbl = cosh(conv2Dbl(x).dbl);
		break;
	}
	case math_exp: {
		rval.dbl = exp(conv2Dbl(x).dbl);
		break;
	}
	case math_expm1: {
		rval.dbl = exp(conv2Dbl(x).dbl) - 1.0;
		break;
	}
	case math_floor: {
		rval.dbl = floor(conv2Dbl(x).dbl);
		break;
	}
	case math_fround: {
		rval.dbl = round(conv2Dbl(x).dbl);
		break;
	}
	case math_imul: {
		rval.bits = vt_int;
		rval.nval = conv2Int(x).nval * conv2Int(y).nval;
		break;
	}
	case math_log: {
		rval.dbl = log(conv2Dbl(x).dbl);
		break;
	}
	case math_log1p: {
		rval.dbl = log(conv2Dbl(x).dbl) - 1.0;
		break;
	}
	case math_log10: {
		rval.dbl = log10(conv2Dbl(x).dbl);
		break;
	}
	case math_log2: {
		rval.dbl = log2(conv2Dbl(x).dbl);
		break;
	}
	case math_pow: {
		rval.dbl = pow(conv2Dbl(x).dbl, conv2Dbl(y).dbl);
		break;
	}
	case math_random: {
#ifdef _WIN32
		uint32_t rnd[1];
		errno_t err;

		if ((err = rand_s(rnd)))
			fprintf(stderr, "MathOps: random number failure %d\n", errno);

		rval.dbl = (double)rnd[0] / ((double)UINT_MAX + 1.0);
#else
#endif
		break;
	}
	case math_round: {
		rval.dbl = round(conv2Dbl(x).dbl);
		break;
	}
	case math_sign: {
		rval.dbl = acos(conv2Dbl(x).dbl);
		break;
	}
	case math_sin: {
		rval.dbl = sin(conv2Dbl(x).dbl);
		break;
	}
	case math_sinh: {
		rval.dbl = sinh(conv2Dbl(x).dbl);
		break;
	}
	case math_sqrt: {
		rval.dbl = sqrt(conv2Dbl(x).dbl);
		break;
	}
	case math_tan: {
		rval.dbl = tan(conv2Dbl(x).dbl);
		break;
	}
	case math_tanh: {
		rval.dbl = tanh(conv2Dbl(x).dbl);
		break;
	}
	case math_trunc: {
		rval.dbl = trunc(conv2Dbl(x).dbl);
		break;
	}
	}

	if (errno == EDOM)
		return s.status = ERROR_mathdomain, s;

	return rval;
}

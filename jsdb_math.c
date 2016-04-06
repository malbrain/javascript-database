#ifdef _WIN32
#define _CRT_RAND_S
#endif

#include <errno.h>
#include <math.h>

#include "jsdb.h"
#include "jsdb_math.h"

static int debug = 0;

value_t conv(value_t val, valuetype_t type) {
	value_t result = val;

	switch (val.type) {
	case vt_infinite:	return val;
	case vt_nan:		return val;
	}

	switch (type) {
	case vt_bool: 		result = conv2Bool(val); abandonValue(val); break;
	case vt_dbl: 		result = conv2Dbl(val); abandonValue(val); break;
	case vt_int: 		result = conv2Int(val); abandonValue(val); break;
	case vt_string:
		result = conv2Str(val);
		if (val.type != vt_string)
			abandonValue(val);
		break;
	}

	return result;
}

value_t op_add (value_t left, value_t right) {
	value_t val;

	switch (left.type) {
	case vt_string:
		return valueCat(left, right);

	case vt_int:
		val.bits = vt_int;
		val.nval = left.nval + conv2Int(right).nval;
		return val;

	case vt_dbl:
		val.bits = vt_dbl;
		val.dbl = left.dbl + conv2Dbl(right).dbl;
		return val;
	}

	val.bits = vt_nan;
	return val;
}

value_t op_sub (value_t left, value_t right) {
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

	val.bits = vt_nan;
	return val;
}

value_t op_mpy (value_t left, value_t right) {
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

	val.bits = vt_nan;
	return val;
}

value_t op_div (value_t left, value_t right) {
	value_t val;

	switch (left.type) {
	case vt_int:
		val.bits = vt_dbl;
		if (right.nval) {
			double result = (double)left.nval / (double)right.nval;
			double intpart = floor(result);

			if (result - intpart)
				val.bits = vt_dbl, val.dbl = result;
			else
				val.bits = vt_int, val.nval = intpart;

			return val;
		} else if (left.nval) {
			val.bits = vt_infinite;
			return val;
		}
		break;

	case vt_dbl:
		val.bits = vt_dbl;
		if (right.dbl) {
			val.dbl = left.dbl / right.dbl;
			return val;
		} else if (left.dbl) {
			val.bits = vt_infinite;
			return val;
		}
		break;
	}

	val.bits = vt_nan;
	return val;
}

value_t op_mod (value_t left, value_t right) {
	value_t val;

	switch (left.type) {
	case vt_int:
		if (right.nval) {
			val.bits = vt_int;
			val.nval = left.nval % right.nval;
			return val;
		}
		val.bits = vt_infinite;
		return val;

	case vt_dbl:
		if (right.dbl) {
			int64_t result = (int64_t)left.dbl % (int64_t)right.dbl;
			double fract = left.dbl - floor(left.dbl);

			if (fract) {
				val.bits = vt_dbl;
				val.dbl = (double)result + fract;
			} else {
				val.bits = vt_int;
				val.nval = result;
			}

			return val;

		} else if (left.dbl) {
			val.bits = vt_infinite;
			return val;
		}
		break;
	}

	val.bits = vt_nan;
	return val;
}

value_t op_rshift (value_t left, value_t right) {
	value_t val = conv2Int(left);

	val.nval >>= conv2Int(right).nval;
	return val;
}

value_t op_lshift (value_t left, value_t right) {
	value_t val = conv2Int(left);

	val.nval <<= conv2Int(right).nval;
	return val;
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
	case vt_infinite:
		if (left.negative)
		  if (right.type == vt_infinite && right.negative)
			return false;
		  else
			return true;

		return false;

	case vt_null:
		return right.type != vt_null;

	case vt_int:
		if (right.type == vt_infinite)
			return !right.negative;

		return left.nval < right.nval;

	case vt_dbl:
		if (right.type == vt_infinite)
			return !right.negative;

		return left.dbl < right.dbl;

	case vt_string:
		return op_compare (left, right) < 0;
	}

	return false;
}

bool op_le (value_t left, value_t right) {
	switch (left.type) {
	case vt_infinite:
		if (left.negative)
			return true;

		return right.type == vt_infinite && !right.negative;

	case vt_null:
		return true;

	case vt_int:
		if (right.type == vt_infinite)
			return !right.negative;

		return left.nval <= right.nval;

	case vt_dbl:
		if (right.type == vt_infinite)
			return !right.negative;

		return left.dbl <= right.dbl;

	case vt_string:
		return op_compare (left, right) <= 0;
	}

	return false;
}

bool op_eq (value_t left, value_t right) {
	switch (left.type) {
	case vt_infinite:
		return right.type == vt_infinite && right.negative == left.negative;

	case vt_null:
		return right.type == vt_null;

	case vt_int:
		if (right.type == vt_infinite)
			return false;

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
	case vt_infinite:
		return right.type != vt_infinite || right.negative != left.negative;

	case vt_null:
		return right.type != vt_null;

	case vt_int:
		if (right.type == vt_infinite)
			return true;

		return left.nval != right.nval;

	case vt_dbl:
		if (right.type == vt_infinite)
			return true;

		return left.dbl != right.dbl;

	case vt_string:
		return op_compare (left, right) != 0;
	}

	return false;
}

bool op_ge (value_t left, value_t right) {
	switch (left.type) {
	case vt_infinite:
		if (left.negative)
			return right.type != vt_infinite || !right.negative;

		return true;

	case vt_null:
		return right.type == vt_null;

	case vt_int:
		if (right.type == vt_infinite)
			return right.negative;

		return left.nval >= right.nval;

	case vt_dbl:
		if (right.type == vt_infinite)
			return right.negative;

		return left.dbl >= right.dbl;

	case vt_string:
		return op_compare (left, right) >= 0;
	}

	return false;
}

bool op_gt (value_t left, value_t right) {
	switch (left.type) {
	case vt_infinite:
		return !(right.type == vt_infinite && !right.negative);

	case vt_null:
		return false;

	case vt_int:
		if (right.type == vt_infinite)
			return right.negative;

		return left.nval > right.nval;

	case vt_dbl:
		if (right.type == vt_infinite)
			return !right.negative;

		return left.dbl > right.dbl;

	case vt_string:
		return op_compare (left, right) > 0;
	}

	return false;
}

bool op_lor (value_t left, value_t right) {
	return conv2Bool(left).boolean || conv2Bool(right).boolean;
}

bool op_land (value_t left, value_t right) {
	return conv2Bool(left).boolean && conv2Bool(right).boolean;
}

typedef value_t (*Mathfcnp)(value_t left, value_t right);
typedef bool (*Boolfcnp)(value_t left, value_t right);

Mathfcnp mathLink[] = {
op_add, op_sub, op_mpy, op_div, op_mod, op_lshift, op_rshift
};

Boolfcnp boolLink[] = {
op_lt, op_le, op_eq, op_ne, op_ge, op_gt, op_lor, op_land
};

value_t eval_math(Node *a, environment_t *env) {
	binaryNode *bn = (binaryNode *)a;
	value_t right = dispatch(bn->right, env);
	value_t left = dispatch(bn->left, env);
	value_t result;

	if (a->aux != math_lshift && a->aux != math_rshift)
	  if (right.type > left.type)
		right = conv(right, left.type);
	  else if (left.type > right.type)
		left = conv(left, right.type);

	if (a->aux < math_comp) {
		if (left.type == vt_infinite)
			return left;

		if (right.type == vt_infinite)
			return right;

		result = mathLink[a->aux](left, right);
	} else {
		result.bits = vt_bool;
		result.boolean = boolLink[a->aux - math_comp - 1](left, right);
	}

	abandonValue(right);
	abandonValue(left);
	return result;
}

value_t eval_neg(Node *a, environment_t *env) {
	exprNode *en = (exprNode *)a;
	value_t v = dispatch(en->expr, env);

	if (a->aux == neg_uminus)
	  switch (v.type) {
	  case vt_dbl: v.dbl = -v.dbl; return v;
	  case vt_int: v.nval = -v.nval; return v;
	  case vt_bool: v.boolean = !v.boolean; return v;
	  case vt_undef: v.bits = vt_nan; return v;
	  case vt_infinite: v.negative = !v.negative; return v;
	  }
	else {
	  v = conv2Bool(v);
	  v.boolean = !v.boolean;
	  return v;
	}

	abandonValue(v);
	return makeError(a, env, "Invalid Negation");
}

value_t eval_incr(Node *a, environment_t *env) {
	exprNode *en = (exprNode *)a;
	value_t slot = dispatch(en->expr, env);
	value_t val;

	if (slot.type == vt_lval)
		val.bits = slot.lval->type;
	else {
		abandonValue(slot);
		return makeError(a, env, "not lvalue");
	}

	if (slot.lval->type == vt_int)
	  switch (a->aux) {
	  case incr_before:
		val.nval = ++slot.lval->nval;
		return val;
	  case incr_after:
		val.nval = slot.lval->nval++;
		return val;
	  case decr_before:
		val.nval = --slot.lval->nval;
		return val;
	  case decr_after:
		val.nval = slot.lval->nval--;
		return val;
	  }
	else if(slot.lval->type == vt_dbl)
	  switch (a->aux) {
	  case incr_before:
		val.dbl = ++slot.lval->dbl;
		return val;
	  case incr_after:
		val.dbl = slot.lval->dbl++;
		return val;
	  case decr_before:
		val.dbl = --slot.lval->dbl;
		return val;
	  case decr_after:
		val.dbl = slot.lval->dbl--;
		return val;
	  }

	val.bits = vt_nan;
	return val;
}

value_t eval_assign(Node *a, environment_t *env)
{
	binaryNode *bn = (binaryNode*)a;
	value_t right, left, *w, val;

	if (debug) printf("node_assign\n");
	left = dispatch(bn->left, env);

	if (left.type != vt_lval) {
		abandonValue(left);
		return makeError(a, env, "not lvalue");
	}

	w = left.lval;
	right = dispatch(bn->right, env);
	left = *w;

	if (bn->hdr->aux == pm_assign)
		return replaceSlotValue(w, right);

	if (left.type && left.type < right.type)
		right = conv(right, left.type);
	else if (right.type && right.type < left.type)
		left = conv(left, right.type);	// old left can't be a vt_string

	// enable string concat onto end of the value

	decrRefCnt(*w);

	switch (bn->hdr->aux) {
	case pm_add: val = op_add (left, right); break;
	case pm_sub: val = op_sub (left, right); break;
	case pm_mpy: val = op_mpy (left, right); break;
	case pm_div: val = op_div (left, right); break;
	case pm_mod: val = op_mod (left, right); break;
	case pm_lshift: val = op_lshift (left, right); break;
	case pm_rshift: val = op_rshift (left, right); break;
	}

	incrRefCnt(val);
	abandonValue(left);
	return *w = val;
}

value_t jsdb_mathop (uint32_t args, environment_t *env) {
	value_t arglist, op, s, xarg, yarg, x, y, rval;
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

	if (vec_count(arglist.aval->array) > 0)
		xarg = arglist.aval->array[0];
	else
		xarg.bits = vt_nan;

	if (vec_count(arglist.aval->array) > 1)
		yarg = arglist.aval->array[1];
	else
		yarg.bits = vt_nan;

	switch (openum) {
	case math_acos: {
		x = conv2Dbl(xarg);
		abandonValue(xarg);

		rval.dbl = acos(x.dbl);
		break;
	}
	case math_acosh: {
		x = conv2Dbl(xarg);
		abandonValue(xarg);

		if (x.type == vt_nan)
			return x;

		rval.dbl = acosh(x.dbl);
		break;
	}
	case math_asin: {
		x = conv2Dbl(xarg);
		abandonValue(xarg);

		if (x.type == vt_nan)
			return x;

		rval.dbl = asin(x.dbl);
		break;
	}
	case math_asinh: {
		x = conv2Dbl(xarg);
		abandonValue(xarg);

		if (x.type == vt_nan)
			return x;

		rval.dbl = asinh(x.dbl);
		break;
	}
	case math_atan: {
		x = conv2Dbl(xarg);
		abandonValue(xarg);

		if (x.type == vt_nan)
			return x;

		rval.dbl = atan(x.dbl);
		break;
	}
	case math_atanh: {
		x = conv2Dbl(xarg);
		abandonValue(xarg);

		if (x.type == vt_nan)
			return x;

		rval.dbl = atanh(x.dbl);
		break;
	}
	case math_atan2: {
		x = conv2Dbl(xarg);
		abandonValue(xarg);

		if (x.type == vt_nan)
			return x;

		y = conv2Dbl(yarg);
		abandonValue(yarg);

		if (x.type == vt_nan)
			return y;

		rval.dbl = atan2(x.dbl, y.dbl);
		abandonValue(xarg);
		break;
	}
	case math_cbrt: {
		x = conv2Dbl(xarg);
		abandonValue(xarg);

		if (x.type == vt_nan)
			return x;

		rval.dbl = cbrt(x.dbl);
		break;
	}
	case math_ceil: {
		x = conv2Dbl(xarg);
		abandonValue(xarg);

		if (x.type == vt_nan)
			return x;

		rval.dbl = ceil(x.dbl);
		break;
	}
	case math_clz32: {
		x = conv2Dbl(xarg);
		abandonValue(xarg);

		if (x.type == vt_nan)
			return x;

		break;
	}
	case math_cos: {
		x = conv2Dbl(xarg);
		abandonValue(xarg);

		if (x.type == vt_nan)
			return x;

		rval.dbl = cos(x.dbl);
		break;
	}
	case math_cosh: {
		x = conv2Dbl(xarg);
		abandonValue(xarg);

		if (x.type == vt_nan)
			return x;

		rval.dbl = cosh(x.dbl);
		break;
	}
	case math_exp: {
		x = conv2Dbl(xarg);
		abandonValue(xarg);

		if (x.type == vt_nan)
			return x;

		rval.dbl = exp(x.dbl);
		break;
	}
	case math_expm1: {
		x = conv2Dbl(xarg);
		abandonValue(xarg);

		if (x.type == vt_nan)
			return x;

		rval.dbl = exp(x.dbl) - 1.0;
		break;
	}
	case math_floor: {
		x = conv2Dbl(xarg);
		abandonValue(xarg);

		if (x.type == vt_nan)
			return x;

		rval.bits = vt_int;
		rval.nval = floor(x.dbl);
		break;
	}
	case math_fround: {
		if (xarg.type == vt_int)
			return xarg;

		x = conv2Dbl(xarg);
		abandonValue(xarg);

		if (x.type == vt_nan)
			return x;

		rval.dbl = round(x.dbl);
		break;
	}
	case math_imul: {
		x = conv2Int(xarg);
		abandonValue(xarg);

		if (x.type == vt_nan)
			return x;

		y = conv2Int(yarg);
		abandonValue(yarg);

		if (y.type == vt_nan)
			return y;

		rval.bits = vt_int;
		rval.nval = x.nval * y.nval;
		break;
	}
	case math_log: {
		x = conv2Dbl(xarg);
		abandonValue(xarg);

		if (x.type == vt_nan)
			return x;

		rval.dbl = log(x.dbl);
		break;
	}
	case math_log1p: {
		x = conv2Dbl(xarg);
		abandonValue(xarg);

		if (x.type == vt_nan)
			return x;

		rval.dbl = log(x.dbl) - 1.0;
		break;
	}
	case math_log10: {
		x = conv2Dbl(xarg);
		abandonValue(xarg);

		if (x.type == vt_nan)
			return x;

		rval.dbl = log10(x.dbl);
		break;
	}
	case math_log2: {
		x = conv2Dbl(xarg);
		abandonValue(xarg);

		if (x.type == vt_nan)
			return x;

		rval.dbl = log2(x.dbl);
		break;
	}
	case math_pow: {
		x = conv2Dbl(xarg);
		abandonValue(xarg);

		y = conv2Dbl(yarg);
		abandonValue(yarg);

		if (x.type == vt_nan)
			return x;

		if (y.type == vt_nan)
			return y;

		rval.dbl = pow(x.dbl, y.dbl);
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
		x = conv2Dbl(xarg);
		abandonValue(xarg);

		if (x.type == vt_nan)
			return x;

		rval.dbl = round(x.dbl);
		break;
	}
	case math_sign: {
		x = conv2Dbl(xarg);
		abandonValue(xarg);

		if (x.type == vt_nan)
			return x;

		rval.dbl = acos(x.dbl);
		break;
	}
	case math_sin: {
		x = conv2Dbl(xarg);
		abandonValue(xarg);

		if (x.type == vt_nan)
			return x;

		rval.dbl = sin(x.dbl);
		break;
	}
	case math_sinh: {
		x = conv2Dbl(xarg);
		abandonValue(xarg);

		if (x.type == vt_nan)
			return x;

		rval.dbl = sinh(x.dbl);
		break;
	}
	case math_sqrt: {
		x = conv2Dbl(xarg);
		abandonValue(xarg);

		if (x.type == vt_nan)
			return x;

		rval.dbl = sqrt(x.dbl);
		break;
	}
	case math_tan: {
		x = conv2Dbl(xarg);
		abandonValue(xarg);

		if (x.type == vt_nan)
			return x;

		rval.dbl = tan(x.dbl);
		break;
	}
	case math_tanh: {
		x = conv2Dbl(xarg);
		abandonValue(xarg);

		if (x.type == vt_nan)
			return x;

		rval.dbl = tanh(x.dbl);
		break;
	}
	case math_trunc: {
		x = conv2Dbl(xarg);
		abandonValue(xarg);

		if (x.type == vt_nan)
			return x;

		rval.dbl = trunc(x.dbl);
		break;
	}
	}

	if (errno == EDOM)
		return s.status = ERROR_mathdomain, s;

	return rval;
}

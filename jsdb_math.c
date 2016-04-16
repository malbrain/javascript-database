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

	switch (type) {
	case vt_bool: 		result = conv2Bool(val, true); break;
	case vt_dbl: 		result = conv2Dbl(val, true); break;
	case vt_int: 		result = conv2Int(val, true); break;
	}

	return result;
}

value_t op_add (value_t left, value_t right) {
	value_t val;

	if (right.type == vt_infinite)
	  if (left.type == vt_infinite) {
		if (right.negative ^ left.negative)
		  return val.bits = vt_nan, val;
		else
		  return right;
	  } else
		return right;

	switch (left.type) {
	case vt_int:
		val.bits = vt_int;
		val.nval = left.nval + right.nval;
		return val;

	case vt_dbl:
		val.bits = vt_dbl;
		val.dbl = left.dbl + right.dbl;
		return val;

	case vt_infinite:
		return left;
	}

	val.bits = vt_nan;
	return val;
}

value_t op_sub (value_t left, value_t right) {
	value_t val;

	if (right.type == vt_infinite)
	  if (left.type == vt_infinite)
		if (!right.negative ^ left.negative)
		  return val.bits = vt_nan, val;
		else
		  return right;
	  else
		return right.negative ^= 1, right;

	switch (left.type) {
	case vt_int:
		val.bits = vt_int;
		val.nval = left.nval - right.nval;
		return val;

	case vt_dbl:
		val.bits = vt_dbl;
		val.dbl = left.dbl - right.dbl;
		return val;

	case vt_infinite:
		return left;
	}

	val.bits = vt_nan;
	return val;
}

value_t op_mpy (value_t left, value_t right) {
	bool sign = false;
	value_t val;

	if (right.type == vt_infinite) {
	  if (left.type == vt_infinite)
		sign = left.negative;
	  else if (left.type == vt_int)
		sign = left.nval < 0;
	  else if (left.type == vt_dbl)
		sign = left.dbl < 0;

	  if (left.type == vt_int && !left.nval || left.type == vt_dbl && !left.dbl)
		return val.bits = vt_nan, val;

	  return right.negative ^= sign, right;
	}

	switch (left.type) {
	case vt_int:
		val.bits = vt_int;
		val.nval = left.nval * right.nval;
		return val;

	case vt_dbl:
		val.bits = vt_dbl;
		val.dbl = left.dbl * right.dbl;
		return val;

	case vt_infinite:
		if (left.type == vt_infinite)
			sign = left.negative;
		else if (left.type == vt_int)
			sign = left.nval < 0;
		else if (left.type == vt_dbl)
			sign = left.dbl < 0;

		if (right.type == vt_int && !right.nval || right.type == vt_dbl && !right.dbl)
			return val.bits = vt_nan, val;

		return left.negative ^= sign, left;
	}

	val.bits = vt_nan;
	return val;
}

value_t op_div (value_t left, value_t right) {
	bool sign = false;
	value_t val;

	if (right.type == vt_infinite) {
	  if (left.type == vt_infinite)
		return val.bits = vt_nan, val;

	  val.bits = vt_int;
	  val.nval = 0;
	  return val;
	}

	switch (left.type) {
	case vt_int:
		val.bits = vt_dbl;
		if (right.nval) {
			double result = (double)left.nval / (double)right.nval;
			int64_t intpart = (int64_t)result;

			if (result - intpart)
				val.bits = vt_dbl, val.dbl = result;
			else
				val.bits = vt_int, val.nval = intpart;

			return val;
		} else if (left.nval) {
			val.bits = vt_infinite;
			val.negative = left.nval < 0 ^ right.nval < 0;
			return val;
		}
		return val.bits = vt_nan, val;

	case vt_dbl:
		val.bits = vt_dbl;
		if (right.dbl) {
			val.dbl = left.dbl / right.dbl;
			return val;
		} else if (left.dbl) {
			val.bits = vt_infinite;
			val.negative = left.dbl < 0 ^ right.dbl < 0;
			return val;
		}
		return val.bits = vt_nan, val;

	case vt_infinite:
	  	if (right.type == vt_int)
			sign = right.nval < 0;
	  	if (right.type == vt_dbl)
			sign = right.dbl < 0;

		left.negative ^= sign;
		return left;
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
		val.bits = vt_nan;
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

		}
		val.bits = vt_nan;
		return val;
	}

	val.bits = vt_nan;
	return val;
}

value_t op_bitxor (value_t left, value_t right) {
	left.nval ^= right.nval;
	left.bits = vt_int;
	return left;
}

value_t op_bitand (value_t left, value_t right) {
	left.nval &= right.nval;
	left.bits = vt_int;
	return left;
}

value_t op_bitor (value_t left, value_t right) {
	left.nval |= right.nval;
	left.bits = vt_int;
	return left;
}

value_t op_rushift (value_t left, value_t right) {
	left.nval = (uint64_t)left.nval >> right.nval;
	left.bits = vt_int;
	return left;
}

value_t op_rshift (value_t left, value_t right) {
	left.nval >>= right.nval;
	left.bits = vt_int;
	return left;
}

value_t op_lshift (value_t left, value_t right) {
	left.nval <<= right.nval;
	left.bits = vt_int;
	return left;
}

int op_compare (value_t left, value_t right) {
	int c;

	if (left.aux == right.aux)
		c = memcmp(left.str, right.str, left.aux);

	else if (left.aux < right.aux) {
		c = memcmp(left.str, right.str, left.aux);
		if (!c)
			c = -1;
	} else {
		c = memcmp(left.str, right.str, right.aux);
		if (!c)
			c = 1;
	}

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
	return conv2Bool(left, false).boolean || conv2Bool(right, false).boolean;
}

bool op_land (value_t left, value_t right) {
	return conv2Bool(left, false).boolean && conv2Bool(right, false).boolean;
}

typedef value_t (*Mathfcnp)(value_t left, value_t right);
typedef bool (*Boolfcnp)(value_t left, value_t right);

Mathfcnp mathLink[] = {
op_add, op_sub, op_mpy, op_div, op_mod
};

Mathfcnp bitLink[] = {
op_bitor, op_bitand, op_bitxor, op_lshift, op_rshift, op_rushift
};

Boolfcnp boolLink[] = {
op_lt, op_le, op_eq, op_ne, op_ge, op_gt, op_lor, op_land
};

value_t eval_math(Node *a, environment_t *env) {
	binaryNode *bn = (binaryNode *)a;
	value_t right = dispatch(bn->right, env);
	value_t left = dispatch(bn->left, env);
	value_t result;

	// math operation

	if (a->aux < math_comp) {
		if (a->aux == math_add && (left.type == vt_string || right.type == vt_string)) {
			if (left.type != vt_string)
				left = conv2Str(left, true);
			if (right.type != vt_string)
				right = conv2Str(right, true);

			return valueCat(left, right);
		}

		if (right.type == vt_string || right.type < left.type)
			right = conv(right, left.type);

		if(left.type == vt_string || left.type < right.type)
			left = conv(left, right.type);

		if (left.type > vt_number || right.type > vt_number)
			return result.bits = vt_nan, result;

		return mathLink[a->aux](left, right);
	}

	// shift operation

	if (a->aux < math_bits) {
		if (right.type != vt_int)
			right = conv2Int(right, true);
		else if (left.type != vt_int)
			left = conv2Int(left, true);

		return bitLink[a->aux - math_comp - 1](left, right);
	}

	// comparison operation

	result.bits = vt_bool;

	if (left.type == vt_string && right.type == vt_string)
		return result.boolean = boolLink[a->aux - math_bits - 1](left, right), result;

	// convert strings to numeric

	if (left.type == vt_string)
		left = conv(left, right.type);
	if (right.type == vt_string)
		right = conv(right, left.type);

	//	convert types upward

	if (right.type < left.type)
		right = conv(right, left.type);
	else if (left.type < right.type)
		left = conv(left, right.type);

	if (left.type == vt_nan || right.type == vt_nan)
		result.boolean = false;
	else
		result.boolean = boolLink[a->aux - math_bits - 1](left, right);

	return result;
}

value_t eval_neg(Node *a, environment_t *env) {
	exprNode *en = (exprNode *)a;
	value_t v = dispatch(en->expr, env);

	if (a->aux == neg_uminus)
	  switch (v.type) {
	  case vt_dbl:		v.dbl = -v.dbl; return v;
	  case vt_int:		v.nval = -v.nval; return v;
	  case vt_bool:		v.boolean = !v.boolean; return v;
	  case vt_undef:	v.bits = vt_nan; return v;
	  case vt_infinite:	v.negative = !v.negative; return v;
	  case vt_nan:		return v;
	  }
	else if (a->aux == neg_bitnot) {
	  v.nval = ~conv2Int(v, true).nval;
	  v.bits = vt_int;
	  return v;
	} else {
	  v = conv2Bool(v, true);
	  v.boolean = !v.boolean;
	  return v;
	}

	abandonValue(v);
	return makeError(a, env, "Invalid Negation");
}

void addToArray(char *lval, int type, int term) {
	switch (type) {
	case array_int8:
		*(int8_t *)lval += term;
		return;

	case array_uint8:
		*(uint8_t *)lval += term;
		return;

	case array_int16:
		*(int16_t *)lval += term;
		return;

	case array_uint16:
		*(uint16_t *)lval += term;
		return;

	case array_int32:
		*(int32_t *)lval += term;
		return;

	case array_uint32:
		*(uint32_t *)lval += term;
		return;

	case array_float32:
		*(float *)lval += term;
		return;

	case array_float64:
		*(double *)lval += term;
		return;
	}
}

value_t incrArray(int type, value_t element) {
	value_t val;

	switch (element.type) {
	case incr_before:
		addToArray(element.slot, element.subType, 1);
		return convArray2Value(element.slot, element.subType);

	case incr_after:
		val = convArray2Value(element.slot, element.subType);
		addToArray(element.slot, element.subType, 1);
		return val;

	case decr_before:
		addToArray(element.slot, element.subType, -1);
		return convArray2Value(element.slot, element.subType);

	case decr_after:
		val = convArray2Value(element.slot, element.subType);
		addToArray(element.slot, element.subType, -1);
		return val;
	}

	val.bits = vt_nan;
	return val;
}

value_t eval_incr(Node *a, environment_t *env) {
	exprNode *en = (exprNode *)a;
	value_t slot = dispatch(en->expr, env);
	value_t val;

	if (slot.type != vt_lval) {
		abandonValue(slot);
		return makeError(a, env, "not lvalue");
	}

	if (slot.subType)
		return incrArray(a->aux, slot);

	val.bits = ((value_t *)(slot.lval))->type;

	switch (a->aux) {
	case incr_before:
		if (((value_t *)(slot.lval))->type == vt_int)
			return val.nval = ++((value_t *)(slot.lval))->nval, val;
		if (((value_t *)(slot.lval))->type == vt_dbl)
			return val.dbl = ++((value_t *)(slot.lval))->dbl, val;
		break;
	case incr_after:
		if (((value_t *)(slot.lval))->type == vt_int)
			return val.nval = ((value_t *)(slot.lval))->nval++, val;
		if (((value_t *)(slot.lval))->type == vt_dbl)
			return val.dbl = ((value_t *)(slot.lval))->dbl++, val;
		break;
	case decr_before:
		if (((value_t *)(slot.lval))->type == vt_int)
			return val.nval = --((value_t *)(slot.lval))->nval, val;
		if (((value_t *)(slot.lval))->type == vt_dbl)
			return val.dbl = --((value_t *)(slot.lval))->dbl, val;
		break;
	case decr_after:
		if (((value_t *)(slot.lval))->type == vt_int)
			return val.nval = ((value_t *)(slot.lval))->nval--, val;
		if (((value_t *)(slot.lval))->type == vt_dbl)
			return val.dbl = ((value_t *)(slot.lval))->dbl--, val;
		break;
	}

	val.bits = vt_nan;
	return val;
}

value_t eval_assign(Node *a, environment_t *env)
{
	binaryNode *bn = (binaryNode*)a;
	value_t right, left, val;

	if (debug) printf("node_assign\n");
	left = dispatch(bn->left, env);

	if (left.type != vt_lval) {
		abandonValue(left);
		return makeError(a, env, "not lvalue");
	}

	right = dispatch(bn->right, env);

	if (bn->hdr->aux == pm_assign)
		return replaceValue(left, right);

	if (left.subType)
		val = convArray2Value(left.slot, left.subType);
	else
		val = *left.lval;

	// enable string concat onto end of the value

	if (bn->hdr->aux == pm_add && (val.type == vt_string || right.type == vt_string)) {
		if (val.type != vt_string)
			val = conv2Str(val, true);
		if (right.type != vt_string)
			right = conv2Str(right, true);

		decrRefCnt(val);
		val = valueCat(val, right);
		incrRefCnt(val);
		return replaceValue(left, val);
	}

	if (val.type > vt_number || right.type > vt_number)
		return val.bits = vt_nan, val;

	if (val.type == vt_string && right.type == vt_string)
		return val.bits = vt_nan, val;
		
	if (bn->hdr->aux > pm_math) {
		right = conv(right, vt_int);
		val = conv(val, vt_int);
	}

	if (right.type == vt_string || right.type < val.type)
		right = conv(right, val.type);

	if(val.type == vt_string || val.type < right.type)
		val = conv(val, right.type);

	switch (bn->hdr->aux) {
	case pm_add: val = op_add (val, right); break;
	case pm_sub: val = op_sub (val, right); break;
	case pm_mpy: val = op_mpy (val, right); break;
	case pm_div: val = op_div (val, right); break;
	case pm_mod: val = op_mod (val, right); break;
	case pm_and: val = op_bitand (val, right); break;
	case pm_xor: val = op_bitxor (val, right); break;
	case pm_or:	 val = op_bitor (val, right); break;
	case pm_lshift: val = op_lshift (val, right); break;
	case pm_rshift: val = op_rshift (val, right); break;
	}

	return replaceValue(left, val);
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

	openum = conv2Int(op, true).nval;
	errno = 0;

	if (vec_count(arglist.aval->values) > 0)
		xarg = arglist.aval->values[0];
	else
		xarg.bits = vt_nan;

	if (vec_count(arglist.aval->values) > 1)
		yarg = arglist.aval->values[1];
	else
		yarg.bits = vt_nan;

	switch (openum) {
	case math_acos: {
		x = conv2Dbl(xarg, true);

		rval.dbl = acos(x.dbl);
		break;
	}
	case math_acosh: {
		x = conv2Dbl(xarg, true);

		if (x.type == vt_nan)
			return x;

		rval.dbl = acosh(x.dbl);
		break;
	}
	case math_asin: {
		x = conv2Dbl(xarg, true);

		if (x.type == vt_nan)
			return x;

		rval.dbl = asin(x.dbl);
		break;
	}
	case math_asinh: {
		x = conv2Dbl(xarg, true);

		if (x.type == vt_nan)
			return x;

		rval.dbl = asinh(x.dbl);
		break;
	}
	case math_atan: {
		x = conv2Dbl(xarg, true);

		if (x.type == vt_nan)
			return x;

		rval.dbl = atan(x.dbl);
		break;
	}
	case math_atanh: {
		x = conv2Dbl(xarg, true);

		if (x.type == vt_nan)
			return x;

		rval.dbl = atanh(x.dbl);
		break;
	}
	case math_atan2: {
		x = conv2Dbl(xarg, true);

		if (x.type == vt_nan)
			return x;

		y = conv2Dbl(yarg, true);

		if (x.type == vt_nan)
			return y;

		rval.dbl = atan2(x.dbl, y.dbl);
		break;
	}
	case math_cbrt: {
		x = conv2Dbl(xarg, true);

		if (x.type == vt_nan)
			return x;

		rval.dbl = cbrt(x.dbl);
		break;
	}
	case math_ceil: {
		x = conv2Dbl(xarg, true);

		if (x.type == vt_nan)
			return x;

		rval.dbl = ceil(x.dbl);
		break;
	}
	case math_clz32: {
		x = conv2Dbl(xarg, true);

		if (x.type == vt_nan)
			return x;

		break;
	}
	case math_cos: {
		x = conv2Dbl(xarg, true);

		if (x.type == vt_nan)
			return x;

		rval.dbl = cos(x.dbl);
		break;
	}
	case math_cosh: {
		x = conv2Dbl(xarg, true);

		if (x.type == vt_nan)
			return x;

		rval.dbl = cosh(x.dbl);
		break;
	}
	case math_exp: {
		x = conv2Dbl(xarg, true);

		if (x.type == vt_nan)
			return x;

		rval.dbl = exp(x.dbl);
		break;
	}
	case math_expm1: {
		x = conv2Dbl(xarg, true);

		if (x.type == vt_nan)
			return x;

		rval.dbl = exp(x.dbl) - 1.0;
		break;
	}
	case math_floor: {
		x = conv2Dbl(xarg, true);

		if (x.type == vt_nan)
			return x;

		rval.bits = vt_int;
		rval.nval = floor(x.dbl);
		break;
	}
	case math_fround: {
		if (xarg.type == vt_int)
			return xarg;

		x = conv2Dbl(xarg, true);

		if (x.type == vt_nan)
			return x;

		rval.dbl = round(x.dbl);
		break;
	}
	case math_imul: {
		x = conv2Int(xarg, true);

		if (x.type == vt_nan)
			return x;

		y = conv2Int(yarg, true);

		if (y.type == vt_nan)
			return y;

		rval.bits = vt_int;
		rval.nval = x.nval * y.nval;
		break;
	}
	case math_log: {
		x = conv2Dbl(xarg, true);

		if (x.type == vt_nan)
			return x;

		rval.dbl = log(x.dbl);
		break;
	}
	case math_log1p: {
		x = conv2Dbl(xarg, true);

		if (x.type == vt_nan)
			return x;

		rval.dbl = log(x.dbl) - 1.0;
		break;
	}
	case math_log10: {
		x = conv2Dbl(xarg, true);

		if (x.type == vt_nan)
			return x;

		rval.dbl = log10(x.dbl);
		break;
	}
	case math_log2: {
		x = conv2Dbl(xarg, true);

		if (x.type == vt_nan)
			return x;

		rval.dbl = log2(x.dbl);
		break;
	}
	case math_pow: {
		x = conv2Dbl(xarg, true);

		y = conv2Dbl(yarg, true);

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
		x = conv2Dbl(xarg, true);

		if (x.type == vt_nan)
			return x;

		rval.dbl = round(x.dbl);
		break;
	}
	case math_sign: {
		x = conv2Dbl(xarg, true);

		if (x.type == vt_nan)
			return x;

		rval.dbl = acos(x.dbl);
		break;
	}
	case math_sin: {
		x = conv2Dbl(xarg, true);

		if (x.type == vt_nan)
			return x;

		rval.dbl = sin(x.dbl);
		break;
	}
	case math_sinh: {
		x = conv2Dbl(xarg, true);

		if (x.type == vt_nan)
			return x;

		rval.dbl = sinh(x.dbl);
		break;
	}
	case math_sqrt: {
		x = conv2Dbl(xarg, true);

		if (x.type == vt_nan)
			return x;

		rval.dbl = sqrt(x.dbl);
		break;
	}
	case math_tan: {
		x = conv2Dbl(xarg, true);

		if (x.type == vt_nan)
			return x;

		rval.dbl = tan(x.dbl);
		break;
	}
	case math_tanh: {
		x = conv2Dbl(xarg, true);

		if (x.type == vt_nan)
			return x;

		rval.dbl = tanh(x.dbl);
		break;
	}
	case math_trunc: {
		x = conv2Dbl(xarg, true);

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

//  implement Math system object

var Math = {};

//  engine implemented functions

Math._ops = enum {
	acos,
	acosh,
	asin,
	asinh,
	atan,
	atanh,
	atan2,
	cbrt,
	ceil,
	clz32,
	cos,
	cosh,
	exp,
	expm1,
	floor,
	fround,
	imul,
	log,
	log1p,
	log10,
	log2,
	pow,
	random,
	round,
	sign,
	sin,
	sinh,
	sqrt,
	tan,
	tanh,
	trunc
};

Math.E			= 2.7182818284590452354;	// e
Math.LOG2E		= 1.4426950408889634074;	// log_2 e
Math.LOG10E		= 0.43429448190325182765;	// log_10 e
Math.LN2		= 0.69314718055994530942;	// log_e 2
Math.LN10		= 2.30258509299404568402;	// log_e 10
Math.PI			= 3.14159265358979323846;	// pi
Math.PI_2		= 1.57079632679489661923;	// pi/2
Math.PI_4		= 0.78539816339744830962;	// pi/4
Math.SQRT2		= 1.41421356237309504880;	// sqrt(2)
Math.SQRT1_2	= 0.70710678118654752440;	// 1/sqrt(2)

Math.max = function() {
	var max = -Infinity, idx;

	for (idx = 0; idx < arguments.length; idx += 1)
		if (arguments[idx] > max)
			max = arguments[idx];

	return max;
};

Math.min = function() {
	var max = Infinity, idx;

	for (idx = 0; idx < arguments.length; idx += 1)
		if (arguments[idx] < max)
			max = arguments[idx];

	return max;
};

Math.hypot = function() {
	var sum = 0.0, idx;

	for (idx = 0; idx < arguments.length; idx += 1)
		sum += arguments[idx] * arguments[idx];

	return Math.sqrt(sum);
};

Math.abs = function(x) {
	if (x < 0)
		return -x;

	return x;
};

Math.acos	= function() { return jsdb_mathop(arguments, Math._ops.acos); };
Math.acosh	= function() { return jsdb_mathop(arguments, Math._ops.acosh); };
Math.asin	= function() { return jsdb_mathop(arguments, Math._ops.asin); };
Math.asinh	= function() { return jsdb_mathop(arguments, Math._ops.asinh); };
Math.atan	= function() { return jsdb_mathop(arguments, Math._ops.atan); };
Math.atanh	= function() { return jsdb_mathop(arguments, Math._ops.atanh); };
Math.atan2	= function() { return jsdb_mathop(arguments, Math._ops.atan2); };
Math.cbrt	= function() { return jsdb_mathop(arguments, Math._ops.cbrt); };
Math.ceil	= function() { return jsdb_mathop(arguments, Math._ops.ceil); };
Math.clz32	= function() { return jsdb_mathop(arguments, Math._ops.clz32); };
Math.cos	= function() { return jsdb_mathop(arguments, Math._ops.cos); };
Math.cosh	= function() { return jsdb_mathop(arguments, Math._ops.cosh); };
Math.exp	= function() { return jsdb_mathop(arguments, Math._ops.exp); };
Math.expm1	= function() { return jsdb_mathop(arguments, Math._ops.expm1); };
Math.floor	= function() { return jsdb_mathop(arguments, Math._ops.floor); };
Math.fround	= function() { return jsdb_mathop(arguments, Math._ops.fround); };
Math.imul	= function() { return jsdb_mathop(arguments, Math._ops.imum); };
Math.log	= function() { return jsdb_mathop(arguments, Math._ops.log); };
Math.log1p	= function() { return jsdb_mathop(arguments, Math._ops.log1p); };
Math.log10	= function() { return jsdb_mathop(arguments, Math._ops.log10); };
Math.log2	= function() { return jsdb_mathop(arguments, Math._ops.log2); };
Math.pow	= function() { return jsdb_mathop(arguments, Math._ops.pow); };
Math.random	= function() { return jsdb_mathop(arguments, Math._ops.random); };
Math.round	= function() { return jsdb_mathop(arguments, Math._ops.round); };
Math.sign	= function() { return jsdb_mathop(arguments, Math._ops.sign); };
Math.sin	= function() { return jsdb_mathop(arguments, Math._ops.sin); };
Math.sinh	= function() { return jsdb_mathop(arguments, Math._ops.sinh); };
Math.sqrt	= function() { return jsdb_mathop(arguments, Math._ops.sqrt); };
Math.tan	= function() { return jsdb_mathop(arguments, Math._ops.tan); };
Math.tanh	= function() { return jsdb_mathop(arguments, Math._ops.tanh); };
Math.trunc	= function() { return jsdb_mathop(arguments, Math._ops.trunc); };

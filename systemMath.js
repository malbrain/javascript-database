//  implement Math system object

Math = {};

//  engine implemented functions

enum Math.ops {
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
}

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

Math.acos	= function() { return jsdb_mathop(arguments, Math.ops.acos); };
Math.acosh	= function() { return jsdb_mathop(arguments, Math.ops.acosh); };
Math.asin	= function() { return jsdb_mathop(arguments, Math.ops.asin); };
Math.asinh	= function() { return jsdb_mathop(arguments, Math.ops.asinh); };
Math.atan	= function() { return jsdb_mathop(arguments, Math.ops.atan); };
Math.atanh	= function() { return jsdb_mathop(arguments, Math.ops.atanh); };
Math.atan2	= function() { return jsdb_mathop(arguments, Math.ops.atan2); };
Math.cbrt	= function() { return jsdb_mathop(arguments, Math.ops.cbrt); };
Math.ceil	= function() { return jsdb_mathop(arguments, Math.ops.ceil); };
Math.clz32	= function() { return jsdb_mathop(arguments, Math.ops.clz32); };
Math.cos	= function() { return jsdb_mathop(arguments, Math.ops.cos); };
Math.cosh	= function() { return jsdb_mathop(arguments, Math.ops.cosh); };
Math.exp	= function() { return jsdb_mathop(arguments, Math.ops.exp); };
Math.expm1	= function() { return jsdb_mathop(arguments, Math.ops.expm1); };
Math.floor	= function() { return jsdb_mathop(arguments, Math.ops.floor); };
Math.fround	= function() { return jsdb_mathop(arguments, Math.ops.fround); };
Math.imul	= function() { return jsdb_mathop(arguments, Math.ops.imum); };
Math.log	= function() { return jsdb_mathop(arguments, Math.ops.log); };
Math.log1p	= function() { return jsdb_mathop(arguments, Math.ops.log1p); };
Math.log10	= function() { return jsdb_mathop(arguments, Math.ops.log10); };
Math.log2	= function() { return jsdb_mathop(arguments, Math.ops.log2); };
Math.pow	= function() { return jsdb_mathop(arguments, Math.ops.pow); };
Math.random	= function() { return jsdb_mathop(arguments, Math.ops.random); };
Math.round	= function() { return jsdb_mathop(arguments, Math.ops.round); };
Math.sign	= function() { return jsdb_mathop(arguments, Math.ops.sign); };
Math.sin	= function() { return jsdb_mathop(arguments, Math.ops.sin); };
Math.sinh	= function() { return jsdb_mathop(arguments, Math.ops.sinh); };
Math.sqrt	= function() { return jsdb_mathop(arguments, Math.ops.sqrt); };
Math.tan	= function() { return jsdb_mathop(arguments, Math.ops.tan); };
Math.tanh	= function() { return jsdb_mathop(arguments, Math.ops.tanh); };
Math.trunc	= function() { return jsdb_mathop(arguments, Math.ops.trunc); };

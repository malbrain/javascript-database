enum builtinProp {
	builtinStr,
	builtinObj,
	builtinArray,
	builtinNum,
	builtinBool,
	builtinFcn
}

String = function(v) {
	if (this)
		this.__setBaseVal(v.toString());
	else
		return v.toString();
};

Object = function(v) {
	var ans = {};
	ans.__setBaseVal(v);
	return ans;
};

Object.assign = function() {
	var target, names, values;

	if (arguments.length < 1)
		return {};

	target = arguments[0];

	if (typeof target == "object")
	  for (var i = 1; i < arguments.length; i++)
		if (getObject(arguments[i], &names, &values))
		  for( var j = 0; j < names.length; j++)
			target[names[j]] = values[j];

	return target;
};

Array = function() {
	var ans = [];

	if(arguments.length == 1 && typeof arguments[0] == "integer" ) {
		ans[arguments[0]-1]=undefined;
		return ans;
	}

	for( var idx = 0; idx < arguments.length; idx++)
		ans[idx] = arguments[idx];

	return ans;
};

Number = function(n) {
	if (this)
		this.__setBaseVal(n);
	else
		return n;
};

Boolean = function() {
	if (arguments.length == 0)
		if (this)
			this.__setBaseVal(false);
		else
			return false;

	else if (arguments[0])
		if (this)
			this.__setBaseVal(true);
		else
			return true;

	else if (this)
		this.__setBaseVal(false);
	else
		return false;
};

Number.EPSILON = 2.220446049250313E-16;
Number.MAX_SAFE_INTEGER = 65536 * 65536 * 65536 * 32768;
Number.MAX_VALUE = 1.7976931348623157E308;
Number.MIN_SAFE_INTEGER = -65536 * 65536 * 65536 * 32768;
Number.MIN_VALUE = 5E-324;
Number.NaN = 0/0;
Number.NEGATIVE_INFINITY = -1/0;
Number.POSITIVE_INFINITY = 1/0;

Number.isNaN = function(x) { return typeof x == "NaN"; };
Number.isFinite = function(x) { return typeof x == "number" || typeof x == "integer"; };
Number.isInteger = function(x) { return typeof x == "integer"; };
Number.isSafeInteger = function(x) { return true; };
Number.parseFloat = function(x) { return x + 0; };
Number.parseInt = function(x) { return x + 0; };

Function = function() {
	return eval(arguments);
};

jsdb_installProps(String, builtinProp.builtinStr);
jsdb_installProps(Object, builtinProp.builtinObj);
jsdb_installProps(Array, builtinProp.builtinArray);
jsdb_installProps(Number, builtinProp.builtinNum);
jsdb_installProps(Boolean, builtinProp.builtinBool);
jsdb_installProps(Function, builtinProp.builtinFcn);

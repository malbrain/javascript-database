var builtinProp = enum {
	builtinStr,
	builtinObj,
	builtinArray,
	builtinNum,
	builtinBool,
	builtinDate,
	builtinFcn
};

var Object = function() {
	var ans;

	if(this)
		ans = this;
	else
		ans = {};

	if (arguments.length > 0)
		ans.__setBaseVal(arguments[0]);

	return ans;
};

jsdb_installProps(Object, builtinProp.builtinObj, _values.vt_object);

Object.assign = function() {
	var target, names, values;

	if (arguments.length < 1)
		return {};

	target = arguments[0];

	if (typeof target == "object")
	  for (var i = 1; i < arguments.length; i++)
		for (var key in arguments[i])
		  target[key] = arguments[i][key];

	return target;
};

var Function = function() {
	return eval(arguments);
};

//Object.setPrototypeOf(Function, Object.prototype);

jsdb_installProps(Function, builtinProp.builtinFcn, _values.vt_closure);

Function.prototype.bind = function() {
};

Function.prototype.toString = function() {
	return "Fcn: " + this.name;
};

var Array = function() {
	var ans = [];

	if(arguments.length == 1 && typeof arguments[0] == "integer" ) {
		ans[arguments[0]-1]=undefined;
		return ans;
	}

	for( var idx = 0; idx < arguments.length; idx++)
		ans[idx] = arguments[idx];

	return ans;
};

//Object.setPrototypeOf(Array, Object.prototype);

jsdb_installProps(Array, builtinProp.builtinArray, _values.vt_array);

Array.prototype.push = function() {
	var nxt = this.length;

	for( var idx = 0; idx < arguments.length; idx++)
		this[nxt++] = arguments[idx];
	
	return nxt;
};

Array.prototype.sort = function() {
	var array = this;

	if (this.length > 1)
		qsort (0, this.length - 1);

	return array;

	function swap (i, j) {
		var t = array[i];
		array[i] = array[j];
		array[j] = t;
	}

	function partition(left, right) {
		var pv = array[Math.floor((right + left) / 2)];
		var j = right;
		var i = left;

		while (i <= j) {
		  while (array[i] < pv)
			i++;

		  while (array[j] > pv)
			j--;

		  if (i <= j)
			swap (i++, j--);
		}

		return i;
	}

	function qsort(left, right) {
		var idx;

		if (left < right) {
			idx = partition(left, right);

			if (left < idx - 1)
				qsort(left, idx - 1);
			if (idx < right)
				qsort (idx, right);
		}
	}
};

var _JSONtype = enum {
	JSONstringify = 1,
	JSONparse
};

var JSON = {};

JSON.stringify = function(value) {
	return jsdb_json(_JSONtype.JSONstringify, value);
};

JSON.parse = function(value) {
	return jsdb_json(_JSONtype.JSONparse, value);
};

var Misc = {};

Misc.ops = enum {
	fromCharCode,
	newDate
};

var Date = function() {
	return jsdb_miscop(arguments, Misc.ops.newDate);
};

jsdb_installProps(Date, builtinProp.builtinDate, _values.vt_date);

var String = function(v) {
	if (this)
		this.__setBaseVal(v.toString());
	else
		return v.toString();
};

jsdb_installProps(String, builtinProp.builtinStr, _values.vt_string);

String.fromCharCode = function() {
	return jsdb_miscop(arguments, Misc.ops.fromCharCode);
};

var Number = function(n) {
	if (this)
		this.__setBaseVal(n);
	else
		return n;
};

jsdb_installProps(Number, builtinProp.builtinNum, _values.vt_int, _values.vt_dbl, _values.vt_infinite, _values.vt_null, _values.vt_nan);

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

var Boolean = function() {
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

jsdb_installProps(Boolean, builtinProp.builtinBool, _values.vt_bool);


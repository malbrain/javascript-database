function Function() {
	return eval(arguments);
};

jsdb_installProps(Function, builtinProp.builtinFcn, _values.vt_closure);

Function.prototype.bind = function() {
};

Function.prototype.toString = function() {
	return "Fcn " + this.name;
};

function Array() {
	var ans = [];

	if(arguments.length == 1)
	  if(typeof arguments[0] == "integer" || typeof arguments[0] == "number") {
		if (arguments[0])
			ans[arguments[0]-1]=undefined;
		return ans;
	}

	for( var idx = 0; idx < arguments.length; idx++)
		ans[idx] = arguments[idx];

	return ans;
};

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

function Date() {
	return jsdb_miscop(arguments, Misc.ops.newDate);
};

jsdb_installProps(Date, builtinProp.builtinDate, _values.vt_date);

function String(v) {
	if (this)
		this.setValue(v.toString());
	else
		return v.toString();
};

jsdb_installProps(String, builtinProp.builtinStr, _values.vt_string);

String.prototype.toString = function() {
	return this.getValue();
};

String.fromCharCode = function() {
	return jsdb_miscop(arguments, Misc.ops.fromCharCode);
};

function Number(n) {
	if (this)
		this.setValue(n);
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

function Boolean() {
	if (arguments.length == 0)
		if (this)
			this.setValue(false);
		else
			return false;

	else if (arguments[0])
		if (this)
			this.setValue(true);
		else
			return true;

	else if (this)
		this.setValue(false);
	else
		return false;
};

jsdb_installProps(Boolean, builtinProp.builtinBool, _values.vt_bool);


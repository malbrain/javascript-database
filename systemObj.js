var builtinProp = enum {
	builtinStr,
	builtinObj,
	builtinArray,
	builtinNum,
	builtinBool,
	builtinDate,
	builtinFcn
};

var Misc = {};

Misc.ops = enum {
	fromCharCode,
	newDate
};

Date = function() {
	return jsdb_miscop(arguments, Misc.ops.newDate);
};

String = function(v) {
	if (this)
		this.__setBaseVal(v.toString());
	else
		return v.toString();
};

String.fromCharCode = function() {
	return jsdb_miscop(arguments, Misc.ops.fromCharCode);
};

Object = function(v) {
	var ans = {};
	ans.__setBaseVal(v);
	return ans;
};

Object._ops = enum {
	keys,
	values
};

Object.keys = function() { return jsdb_objectOp(arguments, Object._ops.keys); };
Object.values = function() { return jsdb_objectOp(arguments, Object._ops.values); };

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
jsdb_installProps(Date, builtinProp.builtinDate);
jsdb_installProps(Function, builtinProp.builtinFcn);

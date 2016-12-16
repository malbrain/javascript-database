var builtinProp = enum {
	builtinStr,
	builtinObj,
	builtinArray,
	builtinNum,
	builtinBool,
	builtinDate,
	builtinFcn
};

var Number = function(n) {
	if (this)
		this.__setBaseVal(n);
	else
		return n;
};


jsdb_installProps(Number, builtinProp.builtinNum, _values.vt_int, _values.vt_dbl, _values.vt_infinite, _values.vt_null, _values.vt_nan);

var o = {
  prop: 37,
  f: function() {
    return this.prop;
  }
};

print(o.f()); // logs 37

var Object = function() {
	var ans = {};

	if (arguments.length > 0)
		ans.__setBaseVal(arguments[0]);

	return ans;
};

jsdb_installProps(Object, builtinProp.builtinObj, _values.vt_object);

var Function = function() {
	return eval(arguments);
};

jsdb_installProps(Function, builtinProp.builtinFcn, _values.vt_closure);

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

Function.prototype = Object.prototype;

Object.setPrototypeOf(Function, Object.prototype);

Function.prototype.bind = function() {
};

Function.prototype.toString = function() {
	return "Fcn: " + this.name;
};

print(Object.prototype);

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

jsdb_installProps(Array, builtinProp.builtinArray, _values.vt_array);

print(Array.prototype);

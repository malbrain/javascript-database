var builtinProp = enum {
	builtinStr,
	builtinObj,
	builtinArray,
	builtinNum,
	builtinBool,
	builtinDate,
	builtinFcn
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

jsdb_installProps(Array, builtinProp.builtinArray);
jsdb_installProps(Number, builtinProp.builtinNum);

var result = {
		version : 1,
		allocator : "me"
	};

var array = [ result ];
print(array);

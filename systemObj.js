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

Object = function(v) {
	var ans = {};
	ans.__setBaseVal(v);
	return ans;
};

String = function(v) {
	if (this)
		this.__setBaseVal(v.toString());
	else
		return v.toString();
};

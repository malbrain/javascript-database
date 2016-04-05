Array = function(n) {
	var ans = [];
	if(n)
		ans[n-1]=undefined;
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

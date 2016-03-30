//  implement Math system object

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

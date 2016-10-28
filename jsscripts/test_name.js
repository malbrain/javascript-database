enum builtinProp {
	builtinStr,
	builtinObj,
	builtinArray,
	builtinNum,
	builtinBool,
	builtinFcn
}

Function = function() {
	return eval(arguments);
};

jsdb_installProps(Function, builtinProp.builtinFcn);

function Test (a,b,c) {
	print(Test.displayName);
	return { test : a, test2:b, test3:c, name : Test.displayName };
}

print(Test(1,2,3));

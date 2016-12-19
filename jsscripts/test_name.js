
function Test (a,b,c) {
	print(Test.displayName);
	return { test : a, test2:b, test3:c, name : Test.displayName };
}

print(Test(1,2,3));

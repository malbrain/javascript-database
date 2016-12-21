
function Test (a,b,c) {
	return { test : a, test2:b, test3:c, name : Test.displayName };
}

print("Call Test(1,2,3), expecting { test : a, test2:b, test3:c, name : Test.displayName }: ", Test(1,2,3));

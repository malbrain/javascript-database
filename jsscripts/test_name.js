print("\n\nbegin test_name.js");
print("------------------");

function Test (a,b,c) {
	return { test : a, test2:b, test3:c, name : Test.displayName };
}

print("Call Test(1,2,3), expecting { test : 1, test2:2, test3:3, name : Test.displayName }: ", Test(1,2,3));

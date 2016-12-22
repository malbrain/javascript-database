print("\n\nbegin test_args.js");
print("------------------");

function Test() {
	print(arguments);
	print(arguments[0]);
}

print("Test arguments & arguments[0] for Test(1,2,3)");
Test(1,2,3);

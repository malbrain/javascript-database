print("\n\nbegin test_es6scope.js");
print("------------------");

var x = 1;
print ("1: ", x);

var y = 2;
print ("2: ", y);

var z = 3;
print ("3: ", z);

var a = 99;

if (x) {
	let a = 12;
	print("15: ", a+z);
}

print("99: ", a);

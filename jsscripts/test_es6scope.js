print("\n\nbegin test_es6scope.js");
print("------------------");

let x = 1;
print ("1: ", x);

var y = 2;
print ("2: ", y);

let z = 3;
print ("3: ", z);

var a = 99;

if (x) {
	z = 12;
	print("12: ", z);
	let a = 12;
	print("24: ", a+z);
}

print("99: ", a);

print("\n\nbegin test_es6scope.js");
print("------------------");

let x = 1;
print ("x = 1: ", x);

var y = 2;
print ("y = 2: ", y);

let z = 3;
print ("z = 3: ", z);

var a = 99;

for (let idx = 0; idx < 5; idx++) {
	print ("\nLoop idx: ", idx);
	z = 12;
	print("z = 12: ", z);
	let a = z + idx;
	print("a = z + idx: ", a);
	print("a + z: ", a+z);
}

print("\na = 99: ", a);

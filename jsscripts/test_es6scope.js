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

print("\nstress test of 1000000 for loop blocks");

var cnt = 0;
var start = Date();

for (let idx = 0; idx < 1000000; idx++) {
	let incr = idx * 2;
	cnt += incr;
}

print ("expecting total 999999000000: ", cnt);

// milliseconds / 1000000 are nanoseconds

print ("nanoseconds per entry/exit: ", (Date() - start));

print("\na = 99: ", a);

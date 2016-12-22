print("\n\nbegin test_math.js");
print("------------------");
var v = [1.2,2.2,3.2];
print("Vector :", v);

print("Multiplied: ", v[0] * v[1] * v[2]);

var a = 1;

for(var i = 0; i < v.length; i++)
	a /= v[i];

print("1/", v[0] * v[1] * v[2], " = ", a);

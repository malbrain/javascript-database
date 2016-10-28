var v = [1.2,2.2,3.2];
print(v[0], v[1], v[2]);
print(v[0] * v[1] * v[2]);

var a = 1;

for(var i = 0; i < v.length; i++)
	a /= v[i];

print(i, ": vec div = ", a);

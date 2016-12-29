print("\n\nbegin test_var.js");
print("------------------");
var a,b,c,d,e,f,g,h,i,j;

var array = [];

for(var i = 0; i < 10; i++)
	array[i] = i;

print ("array w/10 consecutive elements: ", array);

var validation = [];

validation[20] = 2889.0000000000045;

print("expecting 2889.0000000000045: ", validation[20]);


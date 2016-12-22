print("\n\nbegin test_for.js");
print("------------------");
var array = [3,2,1];

print("array: ", array);

for( var idx in array)
	print("idx values in array: ", idx);

for( var val of array)
	print("val values of array: ", val);

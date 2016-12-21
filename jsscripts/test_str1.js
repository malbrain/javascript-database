var array = ["a", 2, 3, "d"];

print('Expecting ["a", 2, 3, "d"] : ', array);

var str = " ";

for(var i = 0; i < 1024 * 1024; i++)
	str += " ";

print("Expecting 1048577: ", str.length);


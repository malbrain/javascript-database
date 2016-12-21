var result = {
		version : 1,
		allocator : "me"
	};

var array = [ result, 2, "abc" ];
print("Array containing an object: ", array);

var out = [];
var src = [1,2,3,4,5];

for(var i = 0; i < src.length; i++)
	out[i] = src[i];

print("copy ", src, " to ", out);

var input = "This is a test only";
var Nb = 4;

var state = [[],[],[],[]];  // initialise 4xNb byte-array 'state' with input

for (var i=0; i<4*Nb; i++) {
  print ("make entry ", i, " for '", input[i], "' at [", i%4, "][", Math.floor(i/4), "]");
  state[i%4][Math.floor(i/4)] = input[i];
}

print("make Two dimensional array: ", state);

var t = new Array(4);

for (var i=0; i<4*Nb; i++)
	t[i] = state[i%4][Math.floor(i/4)];  // shift into temp copy

print("reduce Two dimensional array: ", t);

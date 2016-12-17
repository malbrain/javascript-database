var result = {
		version : 1,
		allocator : "me"
	};

var array = [ result, 2, "abc" ];
print(array);

var out = [];
var src = [1,2,3,4,5];

for(var i = 0; i < src.length; i++)
	out[i] = src[i];

print(out);

var input = "This is a test only";
var Nb = 4;

var state = [[],[],[],[]];  // initialise 4xNb byte-array 'state' with input

for (var i=0; i<4*Nb; i++)
  state[i%4][Math.floor(i/4.0)] = input[i];

print(state);

var t = new Array(4);

for (var r=1; r<4; r++)
  for (var c=0; c<4; c++)
	t[c] = state[r][(c+r)%Nb];  // shift into temp copy

print(t);

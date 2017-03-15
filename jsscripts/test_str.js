print("\n\nbegin test_str.js");
print("------------------");

var plainText = "ROMEO: \"But, soft! what light through yonder window breaks?\n\
It is the east, and Juliet is the sun.\n\
Arise, fair sun, and kill the envious moon,\n\
Who is already sick and pale with grief,\n\
That thou her maid art far more fair than she:\n\
Be not her maid, since she is envious;\n\
Her vestal livery is but sick and green\n\
And none but fools do wear it; cast it off.\n\
It is my lady, O, it is my love!\n\
O, that she knew she were!\n\
She speaks yet she says nothing: what of that?\n\
Her eye discourses; I will answer it.\n\
I am too bold, \'tis not to me she speaks:\n\
Two of the fairest stars in all the heaven,\n\
Having some business, do entreat her eyes\n\
To twinkle in their spheres till they return.\n\
What if her eyes were there, they in her head?\n\
The brightness of her cheek would shame those stars,\n\
As daylight doth a lamp; her eyes in heaven\n\
Would through the airy region stream so bright\n\
That birds would sing and think it were not night.\n\
See, how she leans her cheek upon her hand!\n\
O, that I were a glove upon that hand,\n\
That I might touch that cheek!\n\
JULIET: Ay me!\n\
ROMEO: She speaks:\n\
O, speak again, bright angel! for thou art\n\
As glorious to this night, being o\'er my head\n\
As is a winged messenger of heaven\n\
Unto the white-upturned wondering eyes\n\
Of mortals that fall back to gaze on him\n\
When he bestrides the lazy-pacing clouds\n\
And sails upon the bosom of the air.";

function ChunkStr(str, size) {
	var n = Math.ceil(str.length / size);
	var chunks = new Array(n);

	for (var i = 0; i < n; i++)
		chunks[i] =  str.substr(i * size, size);

	return chunks;
}

var array = ["aaaaaaaaaaaaaaaa", "bbbbbbbbbbbbbbbb", "cccccccccccccccc", 4];

print ('\nmake array["aaaaaaaaaaaaaaaa", "bbbbbbbbbbbbbbbb", "cccccccccccccccc", 4]: ', array);
print("array[0].charCodeAt(6): ", array[0].charCodeAt(6));
print("Expecting typeof array[0]: ", typeof array[0]);

print("\narray.join w/'->': ", array.join("->"));
print("\nmake 16 byte chunks\n");
var x = ChunkStr(plainText, 16);
print(x);

print("\nrejoin 16 byte chunks\n");
print(x.join(""));

print("\nmake more 16 byte chunks\n");
print(ChunkStr("1234567890abcdef1234567890abcdef1234567890abcdef1234567890abcdef", 16));

var test = new String(5);
print("\nmake new String(5):", test);

var testing = { count : 5 };
print("\nmake testing object ", testing, " w/count: ", testing.count);

array = Array(4);
print ("\nmake Array(4): ", array);

var item = {item:"abc"};
print("make item: ", item);

array.test = item;
print("\nadd item to array: ", array.test);

var test = String(testing.count);

print("\n make new string: ", test);

while (test.length < 3) test = "0" + test;

print("\nextend test to 3 chars: ", test);

var str = " ";
var sav = str;

for(var i = 0; i < 1024 * 1024; i++)
	str += " ";

print("original 1 byte string: ", sav.length);
print("\n make 1M char string: ", str.length);


function foo(x) {
    return x.a;
}
print( "\n\nExpecting: {\"x\":10,\"y\":20} - ", foo({a:{x:10,y:20},b:7}) );


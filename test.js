var x = 1;

function test(a) {
	return function () { return a++ + x++;};
}
var func = test(2);

print (func());
print (func());

var buff = "print(arguments);";
jsdb_parseEval("TestPgm", buff, ["abc"]);

buff = "print(func());";
jsdb_parseEval("TestPgm", buff, ["abc"]);

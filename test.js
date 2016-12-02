var x = 1;

function test(a) {
	return function () { return a++ + x++;};
}
var func = test(2);

print (func());
print (func());

var j = [[1,2,3]];
print(JSON.stringify(j));

var j = [[1,2,3,["a", "b"]], "done"];
print(JSON.stringify(j));

var j = [["a","b","c"]];
print(JSON.stringify(j));

print(["abc", 3]);
var buff = "print(arguments);";
jsdb_parseEval("TestPgm", buff, ["abc"]);

buff = "print(func());";
jsdb_parseEval("TestPgm", buff, ["abc"]);

var x = 1;

function test(a) {
	return function () { return a++ + x++;};
}
var func = test(2);

print (func());
print (func());

var j = {a:[1,2,3],b:4,c:{b:4.5}};
var txt = JSON.stringify(j);
print(txt);
print(JSON.parse(txt));

var j = {a:[],b:4,c:{}};
var txt = JSON.stringify(j);
print(txt);
print(JSON.parse(txt));

j = [[1,2,3]];
txt = JSON.stringify(j);
print(txt);
print(JSON.parse(txt));

j = [[1,2,3,["a", "b"]], "done"];
txt = JSON.stringify(j);
print(txt);
print(JSON.parse(txt));

j = [["a","b","c"]];
txt = JSON.stringify(j);
print(txt);
print(JSON.parse(txt));

print(["abc", 3]);
var buff = "print(arguments);";
jsdb_parseEval("TestPgm", buff, ["abc",3]);

buff = "print(func());";
jsdb_parseEval("TestPgm", buff, ["abc"]);

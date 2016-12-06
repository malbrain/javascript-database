var x = 1;

function test(a) {
	return function () { return a++ + x++;};
}
var func = test(2);

print (func());
print (func());

var k = "{\"a\":1,\"b\":\"4\",\"c\":{\"d\":4.5}}";
print(JSON.parse(k));

var k = "{\"a\":[1,2,3,true,false,null],\"b\":\"4\",\"c\":{\"b\":4.5.5}}";
print(JSON.parse(k));

var j = {a:[1,2,3,true,false,null],b:4,c:{b:4.5E-1, d:1/0, e:-1/0}};
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

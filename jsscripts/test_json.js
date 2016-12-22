print("\n\nbegin test_json.js");
print("------------------");
var x = 1;

function test(a) {
	return function () { return a++ + x++;};
}
var func = test(2);

print (func());
print (func());

var k = '{"a":1,"b":"4","c":{"d":4.5}}';
var p = JSON.parse(k);
print("Parse of <", k, ">\nis ", p, "\n");

k = '{"a":[1,2,3,true,false,null],"b":"4","c":{"b":4.5.5}}';
p = JSON.parse(k);
print("Parse of <", k, ">\nis ", p, "\n");

var j = {a:[1,2,3,true,false,null],b:4,c:{b:4.5E-1, d:1/0, e:-1/0}};
var txt = JSON.stringify(j);
print("stringify of <", j, ">\nis ", txt);
p = JSON.parse(txt);
print("and reparse is <", p, ">\n");

j = {a:[],b:4,c:{d:5}};
txt = JSON.stringify(j);
print("stringify of <", j, ">\nis ", txt);
p = JSON.parse(txt);
print("and reparse is <", p, ">\n");

print ("property j.c.d = ", j.c.d);

j = [[1,2,3]];
txt = JSON.stringify(j);
print("stringify of <", j, ">\nis ", txt);
p = JSON.parse(txt);
print("and reparse is <", p, ">\n");

j = [[1,2,3,["a", "b"]], "done"];
txt = JSON.stringify(j);
print("stringify of <", j, ">\nis ", txt);
p = JSON.parse(txt);
print("and reparse is <", p, ">\n");

j = [["a","b","c"]];
txt = JSON.stringify(j);
print("stringify of <", j, ">\nis ", txt);
p = JSON.parse(txt);
print("and reparse is <", p, ">\n");

var buff = "print(arguments);";
jsdb_parseEval("TestPgm", buff, ["abc",3]);

buff = "print(func());";
jsdb_parseEval("TestPgm", buff, ["abc"]);

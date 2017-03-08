print("\n\nbegin test_obj.js");
print("------------------");

var doc = { field: 1,};
print("Expecting { field: 1,} : ", doc);

doc = [1,2];
print("Expecting [1,2] : ", doc);

function NewObj (a,b,c) {
	this.sum = a+b+c;
}

NewObj.prototype.type = "NewObj";

var test = new NewObj(1,2,3);

print("Expecting NewObj: ", test.type);

var nested = {field: 1, sub: {a:1, b:2}};
nested.sub.a = "changed";

print("Expecting {\"field\": 1, \"sub\": {\"a\":\"changed\", \"b\":2}}: ", nested);


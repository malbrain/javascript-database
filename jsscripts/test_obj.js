var doc = { field: 1,};
print(doc);

doc = [1,2];
print(doc);

function NewObj (a,b,c) {
	this.sum = a+b+c;
}

print("begin test");
NewObj.prototype.type = "NewObj";

var test = new NewObj(1,2,3);

print(test.type);



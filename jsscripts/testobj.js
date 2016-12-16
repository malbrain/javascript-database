function NewObj (a,b,c) {
	this.sum = a+b+c;
}

print("begin test");
NewObj.prototype.type = "NewObj";

var test = new NewObj(1,2,3);

print(test.type);



print("\n\nbegin test_obj.js");
print("------------------");

function parse(base, options) {
	var optVals = new Array(DbOptions.MaxParam);

	var ans = options["onDisk"];
	print ("Inside Parse expecting true: ", ans);
	
    for (var name in DbOptions)
        optVals[DbOptions[name]] = base[name];

    for (var name in options)
        optVals[DbOptions[name]] = options[name];

	return ans;
}

function Tst(name, opts) {
	print("Inside Tst expecting true: ", parse(Tst, opts));
}

var opts = {onDisk:true};
print ("Expecting Fcn Tst true: ", parse(Tst, opts));

var db = new Tst("testing", opts);
print("Expecting Fcn Tst true: ", Tst, opts["onDisk"]);

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



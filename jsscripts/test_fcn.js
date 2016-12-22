print("\n\nbegin test_fcn.js");
print("------------------");

function x(y,z){
	if(z){
		return 1;
	}else{
		return 2;
	}
}

print("expecting 1: ", x("yes1", true));

function a(y,z){
	if(z){
		return [y,z];
	}else{
		return ["no z"];
	}
}

print("Expecting 1:", x("yes1", true));
print('Expecting ["yes2", true] :', a("yes2", true));


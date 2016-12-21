function x(y,z){
	if(z){
		return 1;
	}else{
		return 2;
	}
}

print("expecting 1");
x("yes1", true);

function a(y,z){
	if(z){
		print(y,z);
	}else{
		print("no z");
	}
}

print("Expecting yes2true:", x("yes1", true));
print("Expecting undefined:", a("yes2", true));


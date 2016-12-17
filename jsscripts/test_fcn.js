function x(y,z){
	if(z){
		return 1;
	}else{
		return 2;
	}
}

x("yes1", true);

function a(y,z){
	if(z){
		print(y,z);
	}else{
		print("no z");
	}
}

print(x("yes1", true));
print(a("yes2", true));


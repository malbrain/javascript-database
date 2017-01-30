print("\n\nbegin test_proto.js");
print("------------------");
var o = {
  prop: 37,
  f: function() {
    return this.prop;
  }
};
print("Expecting 37: ", o.f());

function dbl(a) { return a + a;}

print("Fcn dbl: ", dbl);

print("Object prototype: ", Object.prototype);

//print("Array prototype: ", Array.prototype);

//Function.prototype.toString = function() {
//	return "Fcn: " + this.name;
//};

//print("Function prototye: ", Function.prototype);



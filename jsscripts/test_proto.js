print("\n\nbegin test_proto.js");
print("------------------");

var o = {
  prop: 37,
  f: function() {
    return this.prop;
  }
};

print("Array prototype: ", Array.prototype);
print("Object prototype: ", Object.prototype);
print("Function prototype: ", Function.prototype);

print("Expecting 37: ", o.f());

function dbl(a) { return a + a;}

print("Fcn dbl: ", dbl);

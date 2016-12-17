var o = {
  prop: 37,
  f: function() {
    return this.prop;
  }
};

print(o.f()); // logs 37

Function.prototype.toString = function() {
	return "Fcn: " + this.name;
};

print("Object prototype: ", Object.prototype);

print("Array prototype: ", Array.prototype);

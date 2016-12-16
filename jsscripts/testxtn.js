Object.extend = function(destination, source) {
  for (var property in source) {
    destination[property] = source[property];
  }
  return destination;
};


var Class = {
  create: function() {
    return function() {
      this.init.apply(this, arguments);
    };
  }
};

//print("make material");
var material = {};

//print("make material.BaseMaterial");
material.BaseMaterial = Class.create();

//print("set material.BaseMaterial.prototype");
material.BaseMaterial.prototype = {
	test:2,
	init: function() {},
	getColor: function(u,v) {}
};

var tst = new material.BaseMaterial();

//print("make material.solid");
material.solid = Class.create();
//print("made material.solid");

material.solid.prototype = Object.extend(
	new material.BaseMaterial(), {
	init: function(color, refl, refr, trans, gloss) {
		this.color = color;
		this.reflection = refl;
		this.transparency = trans;
		this.gloss = gloss;
		this.hasTexture = false;
	},
	getColor: function(u,v) {return this.color;},
	toString: function() {return Object.prototype.toString(this);}
  }	
);

print("material.solid.prototype: ", material.solid.prototype);

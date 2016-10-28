function Node(l,r,i) {
	this.left = l;
	this.right = r;
	this.item = i;
};

Node.prototype.chk = function() {
	return this.right;
};

var x = new Node(1,2,3);

print(x.chk());

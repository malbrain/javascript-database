print("\n\nbegin test_fcn2.js");
print("------------------");

function TreeNode(left,right,item){
   this.left = left;
   this.right = right;
   this.item = item;
}

TreeNode.prototype.itemCheck = function(){
   if (this.left==null) return this.item;
   else return this.item + this.left.itemCheck() - this.right.itemCheck();
};

function bottomUpTree(item,depth){
   if (depth>0){
      return new TreeNode(
          bottomUpTree(2*item-1, depth-1)
         ,bottomUpTree(2*item, depth-1)
         ,item
      );
   }
   else {
      return new TreeNode(null,null,item);
   }
}

print("Expecting 0: ", bottomUpTree(0,0).itemCheck());

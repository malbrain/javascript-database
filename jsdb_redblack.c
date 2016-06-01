#include "jsdb.h"
#include "jsdb_db.h"

//	red/black entry

#define getRb(x,y)	((RedBlack *)getObj(x,y))
#define RB_bits		24

//	red-black tree descent stack

typedef struct {
	int lvl;				// height of the stack
	int gt;					// node is > given key
	int8_t left[RB_bits];	// went left(1) or right (0)
	DbAddr entry[RB_bits];	// stacked tree nodes
} PathStk;

RedBlack *rbFind (DbMap *map, DbAddr *root, uint8_t *key, uint32_t len, PathStk *path);
RedBlack *rbNext(DbMap *map, PathStk *path);

void rbInsert (DbMap *map, DbAddr *root, DbAddr slot, PathStk *path);
void rbRemove (DbMap *map, DbAddr *root, DbAddr slot, PathStk *path);

//  compare two keys, returning > 0, = 0, or < 0
//  as the comparison value
//	-1 -> go right
//	1 -> go left

int rbKeyCmp (RedBlack *node, uint8_t *key2, uint32_t len2) {
	uint32_t len1 = node->keyLen;
	int ans;

	if( ans = memcmp (node->key, key2, len1 > len2 ? len2 : len1) )
		return ans > 0 ? 1 : -1;

	if( len1 > len2 )
		return 1;
	if( len1 < len2 )
		return -1;

	return 0;
}

//  find next key greater/equal given key and produce path stack

RedBlack *rbFind(DbMap *map, DbAddr *root, uint8_t *key, uint32_t len, PathStk *path) {
  RedBlack *node;
  DbAddr slot;

  path->left[0] = 0;
  path->lvl = -1;
  path->gt = 0;

  if( (slot.bits = root->bits) )
   while ((path->entry[++path->lvl].bits = slot.bits)) {
	node = getObj(map,slot);

	path->entry[path->lvl].rbcmp = rbKeyCmp (node, key, len);

	if( path->entry[path->lvl].rbcmp == 0 )
		return node;

	if ((path->left[path->lvl] = path->entry[path->lvl].rbcmp > 0))
		slot = node->left;
	else
		slot = node->right;
   }
  else
	return NULL;

  path->gt = 1;
  return node;
}

//	left rotate parent node

void rbLeftRotate (DbMap *map, DbAddr *root, DbAddr slot, RedBlack *parent, int cmp) {
	RedBlack *x = getObj(map,slot);
	DbAddr right = x->right;
	RedBlack *y = getObj(map,right);

	x->right = y->left;

	if( !parent ) //	is x the root node?
		root->bits = right.bits;
	else if( cmp == 1 )
		parent->left = right;
	else
		parent->right = right;

	y->left = slot;
}

//	right rotate parent node

void rbRightRotate (DbMap *map, DbAddr *root, DbAddr slot, RedBlack *parent, int cmp) {
	RedBlack *x = getObj(map,slot);
	DbAddr left = x->left;
	RedBlack *y = getObj(map,left);

	x->left = y->right;

	if( !parent ) //	is y the root node?
		root->bits = left.bits;
	else if( cmp == 1 )
		parent->left = left;
	else
		parent->right = left;

	y->right = slot;
}

//	insert slot into rbtree at path point

void rbInsert (DbMap *map, DbAddr *root, DbAddr slot, PathStk *path) {
	RedBlack *parent = getObj(map,path->entry[path->lvl]);
	RedBlack *uncle, *grand, *entry;
	int lvl = path->lvl;

	entry = getObj(map, slot);

	if( path->entry[lvl].rbcmp == 1 )
		parent->left = slot;
	else
		parent->right = slot;

	entry->red = 1;

	while( lvl > 0 && parent->red ) {
	  grand = getObj(map,path->entry[lvl-1]);

	  if( path->entry[lvl-1].rbcmp == 1 ) { // was grandparent left followed?
		uncle = getObj(map,grand->right);
		if( grand->right.bits && uncle->red ) {
		  parent->red = 0;
		  uncle->red = 0;
		  grand->red = 1;

		  // move to grandparent & its parent (if any)

	  	  slot = path->entry[--lvl];
		  if( !lvl )
			break;
	  	  parent = getObj(map,path->entry[--lvl]);
		  continue;
		}

		// was the parent right link followed?
		// if so, left rotate parent

	  	if( path->entry[lvl].rbcmp == -1 ) {
		  rbLeftRotate(map, root, path->entry[lvl], grand, path->entry[lvl-1].rbcmp);
		  parent = getObj(map,slot);	// slot was rotated to parent
		}

		parent->red = 0;
		grand->red = 1;

		//	get pointer to grandparent's parent

		if( lvl>1 )
	    	grand = getObj(map,path->entry[lvl-2]);
		else
			grand = NULL;

		//  right rotate the grandparent slot

		slot = path->entry[lvl-1];
		rbRightRotate(map, root, slot, grand, path->entry[lvl-2].rbcmp);
		return;
	  } else {	// symmetrical case
		uncle = getObj(map,grand->left);
		if( grand->left.bits && uncle->red ) {
		  uncle->red = 0;
		  parent->red = 0;
		  grand->red = 1;

		  // move to grandparent & its parent (if any)
	  	  slot = path->entry[--lvl];
		  if( !lvl )
			break;
	  	  parent = getObj(map,path->entry[--lvl]);
		  continue;
		}

		// was the parent left link followed?
		// if so, right rotate parent

	  	if( path->entry[lvl].rbcmp == 1 ) {
		  rbRightRotate(map, root, path->entry[lvl], grand, path->entry[lvl-1].rbcmp);
		  parent = getObj(map,slot);	// slot was rotated to parent
		}

		parent->red = 0;
		grand->red = 1;

		//	get pointer to grandparent's parent

		if( lvl>1 )
	    	grand = getObj(map,path->entry[lvl-2]);
		else
			grand = NULL;

		//  left rotate the grandparent slot

		slot = path->entry[lvl-1];
		rbLeftRotate(map, root, slot, grand, path->entry[lvl-2].rbcmp);
		return;
	  }
	}

	//	reset root color

	getRb(map, *root)->red = 0;
}

//	delete slot from rbtree at path point

void rbRemove (DbMap *map, DbAddr *root, DbAddr slot, PathStk *path) {
	RedBlack *node = getObj (map,slot), *parent, *sibling, *grand;
	uint8_t red = node->red, lvl, idx;
	DbAddr left;

	if( (lvl =  path->lvl) ) {
		parent = getObj(map,path->entry[lvl - 1]);
		parent->right = node->left;
	} else
		root->bits = node->left.bits;

	if( node->left.bits )
		node = getObj(map,node->left);
	else {
		--path->lvl;
		return;
	}

	//	fixup colors

	if( !red )
	 while( !node->red && lvl ) {
		left = parent->left;
		sibling = getObj(map,left);
		if( sibling->red ) {
		  sibling->red = 0;
		  parent->red = 1;
		  if( lvl > 1 )
		  	grand = getObj(map,path->entry[lvl-2]);
		  else
			grand = NULL;
		  rbRightRotate(map, root, path->entry[lvl-1], grand, -1);
		  sibling = getObj(map,parent->left);

		  for( idx = ++path->lvl; idx > lvl - 1; idx-- )
			path->entry[idx] = path->entry[idx-1];

		  path->entry[idx] = left; 
		}

		if( !sibling->right.bits || !getRb(map,sibling->right)->red )
		  if( !sibling->left.bits || !getRb(map,sibling->left)->red ) {
			sibling->red = 1;
			node = parent;
			parent = grand;
			lvl--;
			continue;
		  }

		if( !sibling->left.bits || !getRb(map,sibling->left)->red ) {
			if( sibling->right.bits )
			  getRb(map,sibling->right)->red = 0;

			sibling->red = 1;
			rbLeftRotate (map, root, parent->left, parent, 1);
			sibling = getObj(map,parent->left);
		}

		getRb(map, sibling->left)->red = 0;
		sibling->red = parent->red;
		parent->red = 0;
		rbRightRotate(map, root, path->entry[lvl-1], grand, -1);
		break;
	 }

	getRb(map, *root)->red = 0;
}

//	add or find red/black tree entry
//  return payload address

void *rbAdd (DbMap *parent, DbAddr *root, void *key, uint32_t keyLen) {
	DbAddr child, prev;
	PathStk path[1];
	RedBlack *entry;

	if ((entry = rbFind(parent, root, key, keyLen, path)))
		return entry->payload;

	//	add new entry to the red/black tree

	if ((child.bits = allocBlk(parent, sizeof(RedBlack) + keyLen))) {
		entry = getObj(parent, child);
		entry->keyLen = keyLen;
		memcpy (entry->key, key, keyLen);
		rbInsert (parent, root, child, path);
	}

	return entry->payload;
}

//	return next entry in red/black tree path

RedBlack *rbNext(DbMap *parent, PathStk *path) {
	return NULL;
}

//	enumerate red/black tree node addresses

void rbList(DbMap *parent, DbAddr *root, RbFcnPtr fcn, void *params) {
	PathStk path[1];
	RedBlack *entry;

	rbFind(parent, root, NULL, 0, path);

	while ((entry = rbNext(parent, path)))
		fcn(parent, entry, params);
}

#include "jsdb.h"
#include "jsdb_db.h"
#include "jsdb_redblack.h"

//  compare two keys, returning > 0, = 0, or < 0
//  as the comparison value
//	-1 -> go right
//	1 -> go left

int rbKeyCmp (RedBlack *node, uint8_t *key2, uint32_t len2)
{
uint32_t len1 = node->nameLen;
int ans;

	if( ans = memcmp (node->name, key2, len1 > len2 ? len2 : len1) )
		return ans > 0 ? 1 : -1;

	if( len1 > len2 )
		return 1;
	if( len1 < len2 )
		return -1;

	return 0;
}

//  find next key greater/equal given key and produce path stack

uint64_t rbFind(DbMap *map, uint8_t *key, uint32_t len, PathStk *path)
{
RedBlack *node;
DbAddr slot;

  path->lvl = -1;
  path->gt = 0;

  if( (slot.bits = map->arena->childRoot.bits) )
   while ((path->entry[++path->lvl].bits = slot.bits)) {
	node = getObj(map,slot);

	path->entry[path->lvl].rbcmp = rbKeyCmp (node, key, len);

	if( path->entry[path->lvl].rbcmp == 0 )
		return slot.bits;

	if( path->entry[path->lvl].rbcmp > 0 )
		slot = node->left;
	else
		slot = node->right;
   }
  else
	return 0;

  path->gt = 1;
  return slot.bits;
}

//	left rotate parent node

void rbLeftRotate (DbMap *map, DbAddr slot, RedBlack *parent, int cmp)
{
RedBlack *x = getObj(map,slot);
DbAddr right = x->right;
RedBlack *y = getObj(map,right);

	x->right = y->left;

	if( !parent ) //	is x the root node?
		map->arena->childRoot.bits = right.bits;
	else if( cmp == 1 )
		parent->left = right;
	else
		parent->right = right;

	y->left = slot;
}

//	right rotate parent node

void rbRightRotate (DbMap *map, DbAddr slot, RedBlack *parent, int cmp)
{
RedBlack *x = getObj(map,slot);
DbAddr left = x->left;
RedBlack *y = getObj(map,left);

	x->left = y->right;

	if( !parent ) //	is y the root node?
		map->arena->childRoot.bits = left.bits;
	else if( cmp == 1 )
		parent->left = left;
	else
		parent->right = left;

	y->right = slot;
}

//	insert slot into rbtree at path point

void rbInsert (DbMap *map, DbAddr slot, PathStk *path)
{
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
		  rbLeftRotate(map, path->entry[lvl], grand, path->entry[lvl-1].rbcmp);
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
		rbRightRotate(map, slot, grand, path->entry[lvl-2].rbcmp);
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
		  rbRightRotate(map, path->entry[lvl], grand, path->entry[lvl-1].rbcmp);
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
		rbLeftRotate(map, slot, grand, path->entry[lvl-2].rbcmp);
		return;
	  }
	}

	//	reset root color

	getRb(map, map->arena->childRoot)->red = 0;
}

//	delete slot from rbtree at path point

void rbRemove (DbMap *map, DbAddr slot, PathStk *path)
{
RedBlack *node = getObj (map,slot), *parent, *sibling, *grand;
uint8_t red = node->red, lvl, idx;
DbAddr left;

	if( (lvl =  path->lvl) ) {
		parent = getObj(map,path->entry[lvl - 1]);
		parent->right = node->left;
	} else
		map->arena->childRoot.bits = node->left.bits;

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
		  rbRightRotate(map, path->entry[lvl-1], grand, -1);
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
			rbLeftRotate (map, parent->left, parent, 1);
			sibling = getObj(map,parent->left);
		}

		getRb(map, sibling->left)->red = 0;
		sibling->red = parent->red;
		parent->red = 0;
		rbRightRotate(map, path->entry[lvl-1], grand, -1);
		break;
	 }

	getRb(map,map->arena->childRoot)->red = 0;
}


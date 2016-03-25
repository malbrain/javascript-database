#include "jsdb.h"
#include "jsdb_db.h"

bool artindex_hasdup (DbMap *index, DbAddr *base, uint8_t *suffix) {

	return false;
}

bool artindexKey (DbMap *index, uint8_t *keyBuff, uint32_t keyLen, uint8_t *suffix, uint32_t set) {
	DbAddr *base = artInsertKey(index, artIndexAddr(index)->root, set, keyBuff, keyLen), *tail, newNode;
	ARTSuffix *suffixNode;

	//  enforce unique constraint

	if (indexAddr(index)->opts & index_unique)
	  if (base->type == KeySuffix)
		if (artindex_hasdup (index, base, suffix)) {
			unlockLatch(base->latch);
			return false;
		}

	// splice the suffix node into the tree

	if (base->type == KeySuffix) {
		suffixNode = getObj(index, *base);
		unlockLatch(base->latch);
	} else {
		newNode.bits = allocateNode(index, set, KeySuffix, sizeof(ARTSuffix));
		suffixNode = getObj(index, newNode);
		suffixNode->next->bits = base->bits;
		suffixNode->next->mutex = 0;
		base->bits = newNode.bits;
	}

	tail = artInsertKey(index, suffixNode->suffix, set, suffix, sizeof(SuffixBytes));

	//  is this a duplicate key operation?

	if (tail->type == KeyEnd) {
		unlockLatch(tail->latch);
		return true;
	}

	//  install a KeyEnd marker at the end of the suffix bytes

	newNode.bits = 0;
	newNode.type = KeyEnd;
	newNode.ttype = AddKey;

	tail->bits = newNode.bits;
	return true;
}

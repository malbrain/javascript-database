#include "jsdb.h"
#include "jsdb_db.h"

bool artindex_hasdup (DbMap *index, DbAddr *base, uint8_t *suffix) {

	return false;
}

Status artIndexKey (DbMap *map, DbMap *index, DbAddr docAddr, DocId docId, uint32_t set, uint64_t txnId) {
	uint8_t *keys, suffix[sizeof(SuffixBytes)];
	DbDoc *doc = getObj(map, docAddr);
	DbAddr *base, *tail, newNode;
	uint32_t off = 0, size = 0;
	uint8_t buff[MAX_key];
	ARTEnd *endNode;
	IndexKey *key;
	int type;

	store64(suffix, docId.bits);
	store64(suffix + sizeof(uint64_t), -txnId);
	store64(suffix + 2 * sizeof(uint64_t), doc->docTxn.bits);

	keys = getObj(index, indexAddr(index)->keys);
	base = artIndexAddr(index)->root;
	key = (IndexKey *)keys;

	//	add each key field to the ARTree

	while (key->type != key_end) {
		int len = keyFld(doc, key, buff, MAX_key - size);

		if (len < 0)
			return ERROR_keytoolong;

		base = artInsertKeyFld(index, base, set, buff, len);
		size += len;

		off += sizeof(IndexKey) + key->len;
		key = (IndexKey *)(keys + off);

		if (key->type == key_end)
			break;

		// are we continuing with the next field
		// in a current index key?

		if (base->type == FldEnd) {
			endNode = getObj(index, *base);
			base = endNode->next;
			continue;
		}

		// splice a FldEnd node into the tree

		newNode.bits = artAllocateNode(index, set, FldEnd, sizeof(ARTEnd));
		endNode = getObj(index, newNode);
		endNode->pass->bits = base->bits;
		endNode->pass->mutex = 0;
		base->bits = newNode.bits;
	}

	//  enforce unique constraint

	if (indexAddr(index)->opts & index_unique)
	  if (base->type == Suffix)
		if (artindex_hasdup (index, base, suffix)) {
			unlockLatch(base->latch);
			return ERROR_duplicatekey;
		}

	// splice a suffix node into the tree
	//	after the key.

	if (base->type == Suffix) {
		endNode = getObj(index, *base);
		unlockLatch(base->latch);
	} else {
		newNode.bits = artAllocateNode(index, set, Suffix, sizeof(ARTEnd));
		endNode = getObj(index, newNode);
		endNode->next->bits = base->bits;
		endNode->next->mutex = 0;
		base->bits = newNode.bits;
	}

	//  append the suffix string to the end of the key

	tail = artInsertKeyFld(index, endNode->next, set, suffix, sizeof(SuffixBytes));

	//  and mark the end of the key

	tail->type = KeyEnd;
	unlockLatch(tail->latch);
	return OK;
}

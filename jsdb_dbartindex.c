#include "jsdb.h"
#include "jsdb_db.h"
#include "jsdb_dbtxn.h"

bool artindex_hasdup (DbMap *index, DbAddr *base, uint8_t *suffix) {

	return false;
}

Status artindexKey (array_t *docStore, uint32_t idx, DbDoc *doc, DocId docId, uint32_t set, uint64_t txnId) {
	DbMap *index = docStore->values[idx].hndl;
	uint8_t keyBuff[MAX_key], suffix[sizeof(SuffixBytes)];
	DbAddr *base, *tail, newNode;
	ARTSuffix *suffixNode;
	int keyLen;

	store64(suffix, docId.bits);
	store64(suffix + sizeof(uint64_t), -txnId);
	store64(suffix + 2 * sizeof(uint64_t), doc->docTxn.bits);

	keyLen = makeKey(keyBuff, doc, index);

	base = artInsertKey(index, artIndexAddr(index)->root, set, keyBuff, keyLen);

	//  enforce unique constraint

	if (indexAddr(index)->opts & index_unique)
	  if (base->type == KeySuffix)
		if (artindex_hasdup (index, base, suffix)) {
			unlockLatch(base->latch);
			return ERROR_duplicatekey;
		}

	// splice the suffix node into the tree

	if (base->type == KeySuffix) {
		suffixNode = getObj(index, *base);
		unlockLatch(base->latch);
	} else {
		newNode.bits = artAllocateNode(index, set, KeySuffix, sizeof(ARTSuffix));
		suffixNode = getObj(index, newNode);
		suffixNode->next->bits = base->bits;
		suffixNode->next->mutex = 0;
		base->bits = newNode.bits;
	}

	tail = artInsertKey(index, suffixNode->suffix, set, suffix, sizeof(SuffixBytes));

	//  is this a duplicate key operation?

	if (tail->type == KeyEnd) {
		unlockLatch(tail->latch);
		return OK;
	}

	//  install a KeyEnd marker at the end of the suffix bytes

	newNode.bits = 0;
	newNode.type = KeyEnd;
	newNode.ttype = AddKey;

	tail->bits = newNode.bits;

	addTxnStep(docStore, idx, &doc->docTxn, keyBuff, keyLen, KeyInsert, set);
	return OK;
}

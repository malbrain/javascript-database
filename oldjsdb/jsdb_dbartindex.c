#include "jsdb.h"
#include "jsdb_db.h"

bool artindex_hasdup (DbMap *index, DbAddr *base, KeySuffix *suffix) {

	return false;
}

Status artIndexKey (DbMap *index, DbDoc *doc, DocId docId, uint32_t set) {
	DbAddr *base, *tail, newNode;
	uint32_t off = 0, size = 0;
	ARTEnd *endNode, *sfxNode;
	uint8_t buff[MAX_key];
	KeySuffix suffix[1];
	uint8_t *keys;
	IndexKey *key;
	value_t field;
	int len;

	keys = getObj(index, indexAddr(index)->keys);
	base = artIndexAddr(index)->root;
	key = (IndexKey *)keys;

	//	add each key field to the ARTree

	while (key->type != key_end) {
		value_t name;

		name.bits = vt_string;
		name.str = key->name;
		name.aux = key->len;

		field = lookupDoc((document_t *)(doc + 1), name);

		len = keyFld(field, key, buff, MAX_key - size);

		if (len < 0)
			return ERROR_keytoolong;

		base = artAppendKeyFld(index, base, set, buff, len);
		size += len;

		off += sizeof(IndexKey) + key->len;
		key = (IndexKey *)(keys + off);

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
		base = endNode->next;
	}

	store64(suffix->docId, docId.bits);

	//  enforce unique constraint

	if (indexAddr(index)->opts & index_unique)
	  if (base->type == Suffix)
		if (artindex_hasdup (index, base, suffix)) {
			unlockLatch(base->latch);
			return ERROR_duplicatekey;
		}

	// splice a suffix node into the tree
	//	after the last key field.

	if (base->type == Suffix) {
		sfxNode = getObj(index, *base);
		unlockLatch(base->latch);
	} else {
		newNode.bits = artAllocateNode(index, set, Suffix, sizeof(ARTEnd));
		sfxNode = getObj(index, newNode);
		sfxNode->next->bits = base->bits;
		sfxNode->next->mutex = 0;
		base->bits = newNode.bits;
	}

	//  append the suffix string to the end of the key

	tail = artAppendKeyFld(index, sfxNode->next, set, suffix->bytes, sizeof(KeySuffix));

	//  and mark the end of the key

	tail->type = KeyEnd;
	unlockLatch(tail->latch);
	return OK;
}

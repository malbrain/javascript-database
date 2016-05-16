#include "jsdb.h"
#include "jsdb_db.h"

bool btreeindex_hasdup (DbMap *index, DbAddr *base, uint8_t *suffix) {

	return false;
}

Status btreeIndexKey (DbMap *map, DbMap *index, DbAddr docAddr, DocId docId, uint64_t keySeq) {
	BtreeIndex *btree = btreeIndex(index);
	DbDoc *doc = getObj(map, docAddr);
	uint8_t buff[MAX_key], *keys;
	uint32_t off = 0, size = 0;
	KeySuffix *suffix;
	IndexKey *key;
	value_t field;
	int fldLen;

	keys = getObj(index, btree->index->keys);
	key = (IndexKey *)keys;

	//	add each key field to the Btree key

	while (key->type != key_end) {
		uint8_t next[MAX_key];
		value_t name;

		name.bits = vt_string;
		name.str = key->name;
		name.aux = key->len;

		field = lookupDoc((document_t *)(doc + 1), name);
		fldLen = keyFld(field, key, next, MAX_key - size);

		if (fldLen < 0)
			return ERROR_keytoolong;

		if (fldLen < 128)
			buff[size++] = fldLen;
		else
			buff[size++] = fldLen/256 | 0x80, buff[size++] = fldLen;

		memcpy (buff + size, next, fldLen);
		size += fldLen;

		off += sizeof(IndexKey) + key->len;
		key = (IndexKey *)(keys + off);
	}

	if (!size)
		return OK;

	buff[size++] = 0;	// mark last key field
	suffix = (KeySuffix *)(buff + size);
	store64(suffix->docId, docId.bits);
	store64(suffix->keySeq, ~keySeq);
	size += sizeof(KeySuffix);

	return btreeInsertKey(index, buff, size, 0, Btree_indexed);
}


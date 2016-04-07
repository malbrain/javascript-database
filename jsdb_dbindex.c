#include "jsdb.h"
#include "jsdb_db.h"

#ifdef _WIN32
#define strncasecmp _strnicmp
#endif

bool type_cmp (uint8_t *type, int amt, char *val) {
	if (strlen(val) != amt)
		return false;
	if (!memcmp(type, val, amt))
		return true;
	return false;
}

uint32_t keyCat (uint8_t *keyBuff, uint32_t keyLen, value_t val, uint8_t rev) {
	uint8_t *src = val.key, mask, fwd;
	int len, idx;

	switch (val.type) {
	case vt_int:
		len = sizeof(val.nval);
		break;
	case vt_dbl:
		len = sizeof(val.dbl);
		break;
	case vt_bool:
		len = 1;
		break;
	case vt_objId:
		len = val.aux;
		break;
	case vt_string:
		len = val.aux + 1;
		break;
	default:
		fprintf(stderr, "Invalid key value type: %s\n", strtype(val.type));
		exit(1);
	}

	keyBuff += keyLen;

	if (keyLen + len > MAX_key - sizeof(SuffixBytes)) {
		fprintf(stderr, "Overflow of key length: %d\n", keyLen + len);
		exit(1);
	}

	if (!rev)
		fwd = 0;
	else
		fwd = 0xff;

	if (val.type == vt_objId) {
		for (idx = 0; idx < len; idx++)
			keyBuff[idx] = val.str[idx] ^ fwd;

		return len;
	}

	if (val.type == vt_string) {
		for (idx = 0; idx < len - 1; idx++)
			keyBuff[idx] = val.str[idx] ^ fwd;

		keyBuff[len - 1] = fwd;
		return len;
	}

	mask = 0;

	if (val.type == vt_dbl && src[len-1] & 0x80)
		mask = 0x7f;

	keyBuff[0] = src[len-1] ^ mask ^ 0x80 ^ fwd;

	if (mask)
		mask = 0xff;

	for (idx = 1; idx < len; idx++)
		keyBuff[idx] = src[len - idx - 1] ^ mask ^ fwd;

	return len;
}

uint32_t eval_option(uint8_t *opt, int amt) {
	if (type_cmp (opt, amt, "fwd"))
		return 0;

	if (type_cmp (opt, amt, "rev"))
		return key_reverse;

	if (type_cmp (opt, amt, "int"))
		return key_int;

	if (type_cmp (opt, amt, "dbl"))
		return key_dbl;

	if (type_cmp (opt, amt, "str"))
		return key_str;

	return 0;
}

uint32_t key_options(value_t option) {
	uint8_t *opt = option.str;
	uint32_t len = option.aux;
	uint32_t val = 0, amt;

	if (option.type == vt_int)
		if (option.nval > 0)
			return key_first;
		else
			return key_first | key_reverse;

	else if (option.type == vt_dbl)
		if (option.dbl > 0)
			return key_first;
		else
			return key_first | key_reverse;

	else if (option.type == vt_string && len) do {
		for (amt = 0; amt < len; amt++)
			if(opt[amt] == ':')
				break;
		val |= eval_option(opt, amt);
		if (amt < len)
			amt++;
		opt += amt;
	} while ((len -= amt));

	else
		return key_first;

	return val;
}

uint64_t compile_keys(DbMap *index, object_t *keys, uint32_t set) {
	uint32_t size = sizeof(IndexKey), idx, off = 0;
	uint8_t *base;
	DbAddr addr;

	for (idx = 0; idx < vec_count(keys->names); idx++)
		size += keys->names[idx].aux + sizeof(IndexKey);

	addr.bits = allocMap(index, size);
	base = getObj(index, addr);

	for (idx = 0; idx < vec_count(keys->names); idx++) {
		IndexKey *key = (IndexKey *)(base + off);
		uint32_t len = keys->names[idx].aux;
		off += len + sizeof(IndexKey);

		assert(off <= size);

		key->len = len;
		key->type = key_options(keys->values[idx]);
		memcpy (key->name, keys->names[idx].str, len);
	}

	return addr.bits;
}

uint32_t makeKey (uint8_t *keyBuff, DbDoc *doc, DbMap *index) {
	uint8_t *keys = getObj(index, indexAddr(index)->keys);
	document_t *document = (document_t *)(doc + 1);
	uint32_t keyLen = 0;
	uint32_t off = 0;
	bool fwd;

	while (true) {
		IndexKey *key = (IndexKey *)(keys + off);
		bool rev = key->type & key_reverse;
		value_t name, field;

		if (key->type == key_end)
			return keyLen;

		name.str = key->name;
		name.aux = key->len;

		field = lookupDoc(document, name);

		switch (key->type & key_mask) {
		case key_end:
			if (key->type & key_first)
				switch (field.type) {
				case vt_int:	key->type |= key_int; continue;
				case vt_dbl:	key->type |= key_dbl; continue;
				case vt_string:	key->type |= key_str; continue;
				case vt_bool:	key->type |= key_bool; continue;
				case vt_objId:	key->type |= key_objId; continue;
			}
			break;
		case key_int:
			keyLen += keyCat (keyBuff, keyLen, conv2Int(field, false), rev);
			break;
		case key_dbl:
			keyLen += keyCat (keyBuff, keyLen, conv2Dbl(field, false), rev);
			break;
		case key_bool:
			keyLen += keyCat (keyBuff, keyLen, conv2Bool(field, false), rev);
			break;
		case key_objId:
			keyLen += keyCat (keyBuff, keyLen, conv2ObjId(field, false), rev);
			break;
		}

		off += sizeof(IndexKey) + key->len;
	}
}

//  open/create an index

value_t createIndex(DbMap *docStore, value_t type, value_t keys, value_t name, uint32_t size, bool onDisk, bool unique, bool sparse, value_t partial, uint32_t set) {
	char hndlType;
	DbMap *index;
	value_t val;

	if (!strncasecmp(type.str, "btree", type.aux)) {
		index = createMap(name, docStore, sizeof(BtreeIndex), sizeof(BtreeLocal), size, onDisk);
		btreeInit(index);
		hndlType = hndl_btreeIndex;
	} else if (!strncasecmp(type.str, "art", type.aux)) {
		index = createMap(name, docStore, sizeof(ArtIndex), 0, size, onDisk);
		hndlType = hndl_artIndex;
 	} else {
		fprintf(stderr, "Error: createIndex => invalid type: => %.*s\n", type.aux, type.str);
		val.bits = vt_handle;
		val.status = ERROR_script_internal;
		return val;
	}

	val.bits = vt_handle;
	val.refcount = true;
	val.aux = hndlType;
	val.hndl = index;

	if (!index->created)
		return val;

	if (unique)
		indexAddr(index)->opts |= index_unique;

	if (sparse)
		indexAddr(index)->opts |= index_sparse;

	if (partial.type != vt_null && partial.type != vt_undef)
	  if (partial.type != vt_endlist)
		indexAddr(index)->partial.bits = marshal_doc(index, partial, set);

	indexAddr(index)->keys.bits = compile_keys(index, keys.oval, set);

	index->arena->type = hndlType;
	return val;
}

bool indexKey (DbMap *index, uint8_t *keyBuff, uint32_t keyLen, uint8_t *suffix, uint32_t set) {

	if (index->arena->type == hndl_artIndex)
		return artindexKey (index, keyBuff, keyLen, suffix, set);

	if (index->arena->type == hndl_btreeIndex) {
		return true;
	}

	fprintf(stderr, "Invalid index type: %d\n", index->arena->type);
	exit(1);
}


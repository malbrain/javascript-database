#include "jsdb.h"
#include "jsdb_db.h"

bool typeCmp (value_t type, char *val) {
	if (type.type != vt_string)
		return false;
	if (strlen(val) != type.aux)
		return false;
	if (!memcmp(type.str, val, type.aux))
		return true;
	return false;
}

uint32_t keyCat (uint8_t *keyBuff, uint32_t keyLen, value_t val, uint8_t fwd) {
	uint8_t *src = val.key, mask;
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
	case vt_string:
		len = val.aux + 1;
		break;
	default:
		fprintf(stderr, "Invalid key value type: %s\n", strtype(val.type));
		exit(1);
	}

	keyBuff += keyLen;

	if (keyLen + len > MAX_key - sizeof(KeySuffix)) {
		fprintf(stderr, "Overflow of key length: %d\n", keyLen + len);
		exit(1);
	}

	if (fwd)
		fwd = 0;
	else
		fwd = 0xff;

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

uint32_t makeKey (uint8_t *keyBuff, DbDoc *doc, DbMap *index) {
	document_t *keys = getObj(index, indexAddr(index)->keys);
	document_t *document = (document_t *)(doc + 1);
	uint32_t keyLen = 0;
	bool fwd;
	int idx;

	for (idx = 0; idx < vec_count(keys->names); idx++) {
		value_t type = indexDoc(keys, idx + keys->count);
		value_t name = indexDoc(keys, idx);
		value_t field, val, str;

		field = lookupDoc(document, name);
		val = indexDoc(document, idx + document->count);

		if (typeCmp(type, "int:fwd"))
			keyLen += keyCat (keyBuff, keyLen, conv2Int(val), true);

		else if (typeCmp(type, "int:rev"))
			keyLen += keyCat (keyBuff, keyLen, conv2Int(val), false);

		else if (typeCmp(type, "dbl:fwd"))
			keyLen += keyCat (keyBuff, keyLen, conv2Dbl(val), true);

		else if (typeCmp(type, "dbl:rev"))
			keyLen += keyCat (keyBuff, keyLen, conv2Dbl(val), false);

		else if (typeCmp(type, "str:fwd"))
			keyLen += keyCat (keyBuff, keyLen, conv2Str(val), true);

		else if (typeCmp(type, "str:rev"))
			keyLen += keyCat (keyBuff, keyLen, conv2Str(val), false);

		else
			fprintf(stderr, "Invalid Key field: %.*s type: %.*s\n", field.aux, field.str, type.aux, type.str), exit(1);
	}

	return keyLen;
}

bool insertKey (DbMap *index, uint8_t *keyBuff, uint32_t keyLen, uint8_t *suffix, uint32_t set) {

	return true;
}

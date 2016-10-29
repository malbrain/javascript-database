#include "js.h"
#include "js_dbindex.h"

#ifdef _WIN32
#define strncasecmp _strnicmp
#endif

int keyFld (value_t field, IndexKey *key, uint8_t *buff, uint32_t max) {
	value_t val;
	int len;

	while (true) {
	  switch (key->type & key_mask) {
	  case key_undef:
		if (key->type & key_first)
			switch (field.type) {
			case vt_int:	key->type |= key_int; continue;
			case vt_dbl:	key->type |= key_dbl; continue;
			case vt_string:	key->type |= key_str; continue;
			case vt_bool:	key->type |= key_bool; continue;
			case vt_objId:	key->type |= key_objId; continue;
			default: break;
		}
		break;

	  case key_int:
		val = conv2Int(field, false);

		if (max < sizeof(uint64_t) + 2)
			return -1;

		len = store64(buff, 0, val.nval);
		break;

	  case key_dbl:
		val = conv2Dbl(field, false);

		if (max < sizeof(uint64_t) + 2)
			return -1;

		// store double as int

		len = store64(buff, 0, val.nval);

		// if sign bit not set (negative), flip all the bits

		if (~buff[0] & 0x80)
			for (int idx = 0; idx < len; idx++)
				buff[idx] ^= 0xff;

		break;

	  case key_bool:
		val = conv2Bool(field, false);
		len = 1;

		if (len > max)
			return -1;

		buff[0] = val.boolean ? 1 : 0;
		break;

	  case key_objId:
		val = conv2ObjId(field, false);
		len = val.aux;

		if (len > max)
			return -1;

		memcpy(buff, val.str, len);
		break;

	  default:
		val = conv2Str(field, false);
		len = val.aux;

		if (len > max)
			return -1;

		memcpy(buff, val.str, len);
		buff[len++] = 0;
		break;
	  }

	  break;
	}

	if (key->type & key_reverse)
	  for (int idx = 0; idx < len; idx++)
		buff[idx] ^= 0xff;

	abandonValue(val);
	return len;
}

bool type_cmp (uint8_t *type, int amt, char *val) {
	if (strlen(val) != amt)
		return false;
	if (!memcmp(type, val, amt))
		return true;
	return false;
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

	if (type_cmp (opt, amt, "bool"))
		return key_bool;

	if (type_cmp (opt, amt, "objId"))
		return key_objId;

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

void compileKeys(DbObject *obj, object_t *keys) {
	uint8_t *base = (uint8_t *)(obj + 1);
	uint32_t idx, off = 0;

	for (idx = 0; idx < vec_count(keys->pairs); idx++) {
		IndexKey *key = (IndexKey *)(base + off);
		uint32_t len = keys->pairs[idx].name.aux;
		off += len + sizeof(IndexKey);

		assert(off <= obj->size);

		key->len = len;
		key->type = key_options(keys->pairs[idx].value);
		memcpy (key->name, keys->pairs[idx].name.str, len);
	}
}

uint16_t keyGenerator(uint8_t *buff, Doc *doc, DbObject *spec) {
uint32_t off = 0, size = 0;
uint16_t keyLen = 0;
value_t field, name;
uint8_t *keys;
IndexKey *key;
int fldLen;

	keys = (uint8_t *)(spec + 1);

	//	add each key field to the key

	while (off < spec->size) {
		key = (IndexKey *)(keys + off);

		name.bits = vt_string;
		name.str = key->name;
		name.aux = key->len;

		field = lookupDoc((document_t *)(doc + 1), name);
		fldLen = keyFld(field, key, buff + size, MAX_key - size);

		if (fldLen < 0)
			break;

		off += sizeof(IndexKey) + key->len;
		size += fldLen;
	}

	return size;
}

bool partialEval(Doc *doc, DbObject *spec) {

	return true;
}


#include "js.h"
#include "js_dbindex.h"
#include "database/db.h"
#include "database/db_api.h"
#include "database/db_map.h"
#include "database/db_handle.h"

#ifdef _WIN32
#define strncasecmp _strnicmp
#endif

int keyFld (value_t field, IndexKey *key, char *buff, uint32_t max) {
	string_t *valstr;
	value_t val;
	int len = 0;

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
		valstr = js_addr(val);
		len = valstr->len;

		if (len > max)
			return -1;

		memcpy(buff, valstr->val, len);
		break;

	  default:
		val = conv2Str(field, false, false);
		valstr = js_addr(val);
		len = valstr->len;

		if (len > max)
			return -1;

		memcpy(buff, valstr->val, len);
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

bool type_cmp (char *type, int amt, char *val) {
	if (strlen(val) != amt)
		return false;
	if (!memcmp(type, val, amt))
		return true;
	return false;
}

uint32_t eval_option(char *opt, int amt) {
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

	if (type_cmp (opt, amt, "string"))
		return key_str;

	if (type_cmp (opt, amt, "objId"))
		return key_objId;

	return 0;
}

uint32_t key_options(value_t option) {
	string_t *optstr = js_addr(option);
	uint32_t len = optstr->len;
	uint32_t val = 0, amt;
	char *opt = optstr->val;

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

void compileKeys(DbHandle hndl[1], Params *params) {
	object_t *keys = params[IdxKeySpec].obj;
	pair_t *pairs = keys->marshaled ? keys->pairArray : keys->pairsPtr;
	uint32_t cnt = keys->marshaled ? keys->cnt : vec_cnt(pairs);
	Handle *arena = bindHandle(hndl);
	uint32_t idx, off = 0;
	uint32_t size = 0;
	string_t *namestr;
	uint8_t *base;
	DbAddr slot;
	DbMap *map;

	if (!arena)
		return;

	map = arena->map;
	
	for( idx = 0; idx < cnt; idx++) {
		namestr = js_addr(pairs[idx].name);
		size += namestr->len + sizeof(IndexKey);
	}

	slot.bits = allocBlk(map, size, true);
	base = getObj(map, slot);

	releaseHandle(arena);

	for (idx = 0; idx < cnt; idx++) {
		namestr = js_addr(pairs[idx].name);
		IndexKey *key = (IndexKey *)(base + off);
		uint32_t len = namestr->len;
		off += len + sizeof(IndexKey);

		assert(off <= size);

		key->len = len;
		key->type = key_options(pairs[idx].value);
		memcpy (key->name, namestr->val, len);
	}
}

value_t lookupVer(Ver *ver, char *key, uint32_t len) {
	value_t v;

	v.bits = vt_undef;
	return v;
}

uint16_t keyGenerator(char *buff, Ver *ver, char *spec, uint32_t specLen) {
uint16_t off = 0, size = 0;
value_t field;
uint8_t *keys;
IndexKey *key;
int fldLen;

	keys = (uint8_t *)(spec + 1);

	//	add each key field to the key

	while (off < specLen) {
		key = (IndexKey *)(keys + off);

		field = lookupVer(ver, key->name, key->len);
		fldLen = keyFld(field, key, buff + size, MAX_key - size);

		if (fldLen < 0)
			break;

		off += sizeof(IndexKey) + key->len;
		size += fldLen;
	}

	return size;
}

bool evalPartial(Ver *ver, char *spec, uint32_t specLen) {

	return true;
}


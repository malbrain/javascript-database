#include "js.h"
#include "js_dbindex.h"

#ifdef _WIN32
#define strncasecmp _strnicmp
#endif

int keyFld (value_t *field, IndexKeySpec *spec, IndexKeyValue *keyValue) {
	uint8_t *buff = keyValue->keyBytes + keyValue->keyLen;
	uint32_t max = MAX_key - keyValue->keyLen;
	value_t val, src;
	string_t *str;
	int len = 0;

	if (field)
		src = *field;
	else
		src.bits = vt_undef;

	while (true) {
	  switch (spec->fldType & key_mask) {
	  case key_undef:
		if (spec->fldType & key_first)
			switch (src.type) {
			case vt_int:	spec->fldType |= key_int; continue;
			case vt_dbl:	spec->fldType |= key_dbl; continue;
			case vt_string:	spec->fldType |= key_str; continue;
			case vt_bool:	spec->fldType |= key_bool; continue;
			case vt_objId:	spec->fldType |= key_objId; continue;
			default: break;
		}
		break;

	  case key_int:
		val = conv2Int(src, false);

		if (max < sizeof(uint64_t) + 2)
			return -1;

		len = store64(buff, 0, val.nval);
		break;

	  case key_dbl:
		val = conv2Dbl(src, false);

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
		val = conv2Bool(src, false);
		len = 1;

		if (len > max)
			return -1;

		buff[0] = val.boolean ? 1 : 0;
		break;

	  case key_objId:
		val = conv2ObjId(src, false);
		str = js_addr(val);
		len = str->len;

		if (len > max)
			return -1;

		memcpy(buff, str->val, len);
		break;

	  default:
		val = conv2Str(src, false, false);
		str = js_addr(val);

		len = str->len;

		if (len > max)
			return -1;

		memcpy(buff, str->val, len);
		buff[len++] = 0;
		break;
	  }

	  break;
	}

	if (spec->fldType & key_reverse)
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
	string_t *str = js_addr(option);
	uint32_t len = str->len;
	uint32_t val = 0, amt;
	char *opt = str->val;

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

//	compile keys into permanent spot in the index

DbAddr compileKeys(DbMap *map, Params *params) {
	object_t *keys = getParamIdx(params, IdxKeySpec);
	pair_t *pairs = keys->pairArray;
	uint32_t idx, off;
	string_t *str;
	uint32_t size;
	uint8_t *base;
	DbAddr slot;

	size = sizeof(value_t);

	for( idx = 0; idx < keys->cnt; idx++) {
		str = getParamOff(params, pairs[idx].name.offset);
		size += str->len + sizeof(IndexKeySpec);
	}

	//	allocate space to compile key structure

	slot.bits = allocBlk(map, size, true);

	base = getObj(map, slot);
	off = sizeof(uint32_t);

	//	fill in key structure

	for (idx = 0; idx < keys->cnt; idx++) {
		str = getParamOff(params, pairs[idx].name.offset);
		IndexKeySpec *spec = (IndexKeySpec *)(base + off);

		memcpy (spec->fldName, str->val, str->len);
		spec->hash = hashStr(spec->fldName, *spec->nameLen);
		spec->fldType = key_options(pairs[idx].value);
		*spec->nameLen = str->len;

		off += str->len + sizeof(IndexKeySpec);
		assert(off <= size);
	}

	*(uint32_t *)base = off;
	return slot;
}

//  build an array of keys for an index
//	return an array of docStore addresses
//	containing the key values

DbAddr *buildKeys(Handle *docHndl, Handle *idxHndl, value_t document, ObjId docId, Ver *prevVer) {
	bool binaryFlds = idxHndl->map->arenaDef->params[IdxBinary].boolVal;
	DbIndex *index = dbindex(idxHndl->map);
	uint8_t *base = getObj(idxHndl->map, index->keys);
	object_t *keyObj = (object_t *)(base + sizeof(uint32_t));
	uint64_t nxtVersion = prevVer ? prevVer->version : 1;
	JsVersion *version = (JsVersion *)(prevVer + 1);
	char buff[MAX_key + sizeof(IndexKeyValue)];
	uint16_t depth = 0, off = sizeof(uint32_t);
	uint32_t keyMax = *(uint32_t *)base;
	KeyStack stack[MAX_array_fields];
	IndexKeyValue *keyValue;
	DbAddr *vec = NULL;
	value_t *val, name;
	IndexKeyValue *key;
	IndexKeySpec *spec;
	uint64_t next;
	DbAddr addr;
	int fldLen;
	int size;
	int idx;

  //	create IndexKeyVelue structure in keyBuff

  keyValue = (IndexKeyValue *)buff;

  //	initialize key with index childId

  keyValue->keyLen = store64(keyValue->keyBytes, 0, nxtVersion);

  //	add each key field to the key, or multi-key

  while (true) {
	if (off < keyMax)
	  spec = (IndexKeySpec *)(base + off);
	else {
	  //  add completed key string to keys vector

	  *keyValue->baseLen = keyValue->keyLen += store64(keyValue->keyBytes, keyValue->keyLen, docId.addr);

	  keyValue->keyLen += store64(keyValue->keyBytes, keyValue->keyLen, nxtVersion);

	  // try to find key in previous version

	  addr.bits = 0;

	  if (prevVer) {
		value_t val, *prev;
		val.bits = vt_string;
		val.addr = keyValue->baseLen;
	  	prev = lookup(js_addr(*version->keys), val, false, hashStr(keyValue->keyBytes, *keyValue->baseLen));

		if (prev)
		  addr.bits = prev->arenaAddr.bits;
	  }

	  if (!addr.bits) {
		size = sizeof(IndexKeyValue) + keyValue->keyLen;
		next = dbAllocDocStore(docHndl, sizeof(IndexKeyValue) + keyValue->keyLen, false);
		addr.bits = next;
		memcpy (getObj(docHndl->map, addr), keyValue, size);
	  }

	  vec_push(vec, addr);

	  // are we finished?

	  if (!depth--)
		break;

	  // advance to next array element in multi-key

	  off = stack[depth].off;
	  idx = stack[depth].idx++;
	  keyValue->keyLen = stack[depth].keyLen;

	  if (idx < stack[depth].cnt)
	  	val = stack[depth++].values + idx;

	  continue;
	}
		
	name.bits = vt_string;
	name.addr = spec->nameLen;
	val = lookup(keyObj, name, false, spec->hash);

	if (val->type == vt_array) {
	  array_t *aval = js_addr(*val);

	  stack[depth].values = val->marshaled ? aval->valueArray : aval->valuePtr;
	  stack[depth].cnt = val->marshaled ? aval->cnt : vec_cnt(aval->valuePtr);
	  stack[depth].keyLen = keyValue->keyLen;
	  stack[depth].off = off;
	  stack[depth].idx = 1;

	  if (stack[depth].cnt)
	  	val = stack[depth].values;
	  else
		val = NULL;

	  if (stack[depth].cnt > 1)
		depth++;
	}

	fldLen = keyFld(val, spec, keyValue);

	if (fldLen < 0)
		break;

	off += sizeof(IndexKeySpec) + *spec->nameLen;
	keyValue->keyLen += fldLen;
  }

  return vec;
}

#include "jsdb.h"
#include "jsdb_db.h"

#ifdef _WIN32
#define strncasecmp _strnicmp
#endif

int keyFld (value_t field, IndexKey *key, uint8_t *buff, uint32_t max) {
	value_t name, val;
	int len;

	while (true) {
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
		val = conv2Int(field, false);
		len = sizeof(uint64_t);
		if (len > max)
			return -1;

		store64(buff, val.nval);

		// flip the sign bit
		buff[0] ^= 0x80;
		break;
	  case key_dbl:
		val = conv2Dbl(field, false);
		len = sizeof(double);
		if (len > max)
			return -1;

		store64(buff, val.dbl);

		// if sign bit set, flip all the bits
		// otherwise just flip the sign bit

		if (buff[0] & 0x80)
			for (int idx = 0; idx < len; idx++)
				buff[idx] ^= 0xff;
		else
			buff[0] ^= 0x80;

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

uint64_t compile_keys(DbMap *index, object_t *keys, uint32_t set) {
	uint32_t size = sizeof(IndexKey), idx, off = 0;
	uint8_t *base;
	DbAddr addr;

	for (idx = 0; idx < vec_count(keys->pairs); idx++)
		size += keys->pairs[idx].name.aux + sizeof(IndexKey);

	addr.bits = allocMap(index, size);
	base = getObj(index, addr);

	for (idx = 0; idx < vec_count(keys->pairs); idx++) {
		IndexKey *key = (IndexKey *)(base + off);
		uint32_t len = keys->pairs[idx].name.aux;
		off += len + sizeof(IndexKey);

		assert(off <= size);

		key->len = len;
		key->type = key_options(keys->pairs[idx].value);
		memcpy (key->name, keys->pairs[idx].name.str, len);
	}

	return addr.bits;
}

//  open/create an index

value_t createIndex(DbMap *docStore, value_t type, value_t keys, value_t name, uint32_t size, bool unique, bool sparse, value_t partial, uint32_t set) {
	DbMap *index;
	value_t val;

	if (!strncasecmp(type.str, "btree", type.aux)) {
		index = createMap(name, docStore, sizeof(BtreeIndex), sizeof(BtreeLocal), size, docStore->onDisk);
		btreeInit(index);
		index->arena->type = hndl_btreeIndex;
	} else if (!strncasecmp(type.str, "art", type.aux)) {
		index = createMap(name, docStore, sizeof(ArtIndex), 0, size, docStore->onDisk);
		index->arena->type = hndl_artIndex;
 	} else {
		fprintf(stderr, "Error: createIndex => invalid type: => %.*s\n", type.aux, type.str);
		val.bits = vt_handle;
		val.status = ERROR_script_internal;
		return val;
	}

	val.bits = vt_handle;
	val.aux = index->arena->type;
	val.refcount = true;
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
	return val;
}

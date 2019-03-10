#include "js.h"
#include "js_props.h"
#include "js_string.h"

#include "js_db.h"
#include "js_dbindex.h"
#include "database/btree1/btree1.h"
#include "database/artree/artree.h"

#include <stddef.h>

#ifdef _WIN32
#define strncasecmp _strnicmp
#endif

extern DbMap memMap[1];

bool compareDups(DbMap *map, DbCursor *dbCursor) {
	Ver *ver = findCursorVer(dbCursor, map, NULL);

	return ver ? true : false;
}

//	insert a key into an index

DbStatus insertIdxKey (Handle *idxHndl, IndexKeyValue *keyValue, uint8_t *bytes) {
	uint32_t totLen = keyValue->keyLen + keyValue->docIdLen + keyValue->addrLen;
	DbStatus stat = OK;

	switch (*idxHndl->map->arena->type) {
	case Hndl_artIndex:
		if (keyValue->unique)
			stat = artInsertUniq(idxHndl, bytes, totLen, keyValue->keyLen, compareDups, &keyValue->deferred);
		else
			stat = artInsertKey(idxHndl, bytes, totLen);
		break;

	case Hndl_btree1Index:
		stat = btree1InsertKey(idxHndl, bytes, totLen, 0, Btree1_indexed);
		break;
	}

	return stat;
}

//	delete a key from an index

DbStatus deleteIdxKey (Handle *idxHndl, IndexKeyValue *keyValue, uint8_t *bytes) {
	uint32_t totLen = keyValue->keyLen + keyValue->docIdLen + keyValue->addrLen;
	Status stat = OK;

	switch (*idxHndl->map->arena->type) {
	case Hndl_artIndex:
		stat = artDeleteKey(idxHndl, bytes, totLen, keyValue->keyLen);
		break;

	case Hndl_btree1Index:
		stat = btree1DeleteKey(idxHndl, bytes, totLen);
		break;
	}

	return stat;
}

//  un-install version's keys

DbStatus removeKeys(Handle **idxHndls, Ver *ver, DbMmbr *mmbr, DbAddr *slot) {
	Handle *docHndl = idxHndls[0];
	DbStatus stat = OK;

	if (!mmbr) {
	  if (ver->keys->addr)
		mmbr = getObj(docHndl->map, *ver->keys);
	  else
		return stat;
	}

	while ((slot = revMmbr(mmbr, &slot->bits))) {
	  IndexKeyValue *keyValue = getObj(docHndl->map, *slot);

	  if (!atomicAdd64(keyValue->refCnt, (uint64_t)(-1LL))) {
		if ((stat = deleteIdxKey(idxHndls[keyValue->keyIdx], keyValue, (uint8_t *)(keyValue + 1))))
		  return stat;
	  }
	}

	return stat;
}

//  install the document version keys

DbStatus installKeys(Handle **idxHndls, Ver *ver) {
	DbAddr *slot = NULL;
	DbStatus stat;
	DbMmbr *mmbr;

	if (ver->keys->addr)
		mmbr = getObj(idxHndls[0]->map, *ver->keys);
	else
		return OK;

	while ((slot = allMmbr(mmbr, &slot->bits))) {
	  IndexKeyValue *keyValue = getObj(idxHndls[0]->map, *slot);

	  if (atomicAdd64(keyValue->refCnt, 1ULL) == 1) {
		if ((stat = insertIdxKey(idxHndls[keyValue->keyIdx], keyValue, (uint8_t *)(keyValue + 1))))
		  break;
		else
		  ver->deferred |= keyValue->deferred;
	  }
	}

	if (!slot)
		return OK;

	if (!(stat = removeKeys(idxHndls, ver, mmbr, slot)))
		stat = ERROR_key_constraint_violation;

	return stat;
}

//	allocate docStore power-of-two memory

uint64_t allocDocStore(Handle *docHndl, uint32_t size, bool zeroit) {
DbAddr *free = listFree(docHndl,0);
DbAddr *wait = listWait(docHndl,0);

	return allocObj(docHndl->map, free, wait, -1, size, zeroit);
}

int keyFld (value_t src, IndexKeySpec *spec, IndexKeyValue *keyValue, uint8_t *bytes, bool binaryFlds) {
	uint8_t *buff = bytes + keyValue->keyLen;
	uint32_t max = MAX_key - keyValue->keyLen;
	uint32_t len = 0, off, idx;
	string_t *str;
	value_t val;

	off = binaryFlds ? 2 : 0;

	while (true) {
	  switch (spec->fldType & key_mask) {
	  case key_undef:
		if (spec->fldType & key_first)
			switch (src.type) {
			case vt_int:	spec->fldType |= key_int; continue;
			case vt_dbl:	spec->fldType |= key_dbl; continue;
			case vt_string:	spec->fldType |= key_str; continue;
			case vt_bool:	spec->fldType |= key_bool; continue;
			default: break;
		}
		break;

	  case key_int:
		val = conv2Int(src, false);

		if (max < sizeof(uint64_t) + 2)
			return -1;

		len = store64(buff, 0, val.nval, binaryFlds);
		break;

	  case key_dbl:
		val = conv2Dbl(src, false);

		if (max < sizeof(uint64_t) + 2)
			return -1;

		// store double as int

		len = store64(buff, 0, val.nval, binaryFlds);

		// if sign bit not set (negative), flip all the bits

		if (~buff[off] & 0x80)
			for (idx = off; idx < len; idx++)
				buff[idx] ^= 0xff;

		break;

	  case key_bool:
		val = conv2Bool(src, false);
		len = 1;

		if (len > max)
			return -1;

		if (binaryFlds)
			len = store64(buff, 0, val.boolean ? 1 : 0, binaryFlds);
		else
			buff[0] = val.boolean ? 1 : 0;

		break;

	  default:
		val = conv2Str(src, false, false);
		str = js_addr(val);

		len = str->len;

		if (len > max)
			return -1;

		memcpy(buff + off, str->val, len);

		if (!binaryFlds)
			buff[len++] = 0;
		else {
			buff[0] = (len - 2) >> 8; 
			buff[1] = (len- 2) ;
		}

		break;
	  }

	  break;
	}

	if (spec->fldType & key_reverse)
	  for (idx = off; idx < len; idx++)
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

	if (type_cmp (opt, amt, "string"))
		return key_str;

	return 0;
}

uint32_t key_options(value_t option) {
	string_t *str = js_addr(option);
	uint8_t *opt = str->val;
	uint32_t len = str->len;
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

//	compile keys into permanent spot in the database

DbAddr compileKeys(DbHandle hndl[1], value_t keySpec) {
	dbobject_t *dboval = js_addr(keySpec);
    pair_t *pairs = keySpec.marshaled ? dboval->pairs : keySpec.oval->pairsPtr;
    uint32_t cnt = keySpec.marshaled ? dboval->cnt : vec_cnt(pairs);
	uint32_t idx, off, fld;
	struct Field *field;
	IndexKeySpec *spec;
	Handle *docHndl;
	uint8_t *base;
	string_t *str;
	uint32_t size;
	DbAddr slot;

	slot.bits = 0;

	if ((docHndl = bindHandle(hndl)))
		size = sizeof(uint32_t);
	else
		return slot;

	for( idx = 0; idx < cnt; idx++) {
		size += sizeof(IndexKeySpec) + sizeof(struct Field);
		str = js_addr(pairs[idx].name);

		//  go through field name

		for (fld = 0; fld < str->len; fld++)
		  if (str->val[fld] == '.')
			size += sizeof(struct Field);
		  else
			size++;
	}

	//	allocate space to compile key structure
	//	in the database

	slot.bits = allocBlk(docHndl->map->db, size, true);
	base = getObj(docHndl->map->db, slot);
	off = sizeof(uint32_t);

	//	fill in each compound key spec

	for (idx = 0; idx < cnt; idx++) {
		spec = (IndexKeySpec *)(base + off);
		str = js_addr(pairs[idx].name);
		off += sizeof(*spec);

		memset (spec, 0, sizeof(*spec));
		spec->fldType = key_options(pairs[idx].value);
		spec->numFlds = 1;

		//  go through field name components

		field = (struct Field *)(base + off);
		fld = 0;

		while (fld < str->len) {
			uint8_t ch = str->val[fld++];

		  // end of a field name component?

		  if (ch == '.') {
			field->hash = hashStr(field->name, *field->len);
			off += sizeof(struct Field);
			spec->numFlds++;

			field = (struct Field *)(base + off);
			memset (field, 0, sizeof(struct Field));
			continue;
		  }

		  field->name[field->len[0]++] = ch;
		  off++;
		}

		field->hash = hashStr(field->name, *field->len);
		off += sizeof(struct Field);
	}

	releaseHandle(docHndl, hndl);
	*(uint32_t *)base = off;
	return slot;
}

//  build an array of keys for a document

void buildKeys(Handle **idxHndls, uint16_t keyIdx, value_t rec, DbAddr *keys, ObjId docId, Ver *prevVer, uint32_t idxCnt) {
	bool binaryFlds = idxHndls[keyIdx]->map->arenaDef->params[IdxKeyFlds].boolVal;
	uint16_t depth = 0, off = sizeof(uint32_t);
	KeyStack stack[MAX_array_fields];
	int fldLen, idx, fld, nxt;
	IndexKeyValue keyValue[1];
	uint8_t bytes[MAX_key];
	struct Field *field;
	IndexKeySpec *spec;
	value_t val, name;
	uint32_t keyMax;
	uint8_t *base;
	DbAddr addr;

  base = getObj(idxHndls[keyIdx]->map->db, idxHndls[keyIdx]->map->arenaDef->params[IdxKeyAddr].addr);
  keyMax = *(uint32_t *)base;

  //	create IndexKeyVelue structure in buff

  memset (keyValue, 0, sizeof(IndexKeyValue));

  //	add each key field to the key, or multi-key

  while (true) {
	if (off < keyMax)
	  spec = (IndexKeySpec *)(base + off);
	else {
	  int docIdLen = store64(bytes, keyValue->keyLen, docId.addr, binaryFlds);
	  uint64_t hash = hashStr(bytes, keyValue->keyLen + docIdLen);
	  bool found = false;

	  // try to reuse key from a previous version

	  if (prevVer && prevVer->keys->bits) {
		DbMmbr *mmbr = getObj(idxHndls[0]->map, *prevVer->keys);
		DbAddr *slot = getMmbr(mmbr, hash);

		//  find IndexKeyValue address in the mmbr table

		while (slot && slot->bits) {
		  IndexKeyValue *prior = getObj(idxHndls[0]->map, *slot);

		  if (prior->idxId == idxHndls[keyIdx]->map->arenaDef->id)
		   if (prior->keyLen == keyValue->keyLen)
			if (prior->docIdLen == docIdLen)
			 if (!memcmp(prior + 1, bytes, prior->keyLen + prior->docIdLen)) {
				addr.bits = slot->bits;
				found = true;
				break;
			 }

		  slot = nxtMmbr(mmbr, &slot->bits);
		}
	  }

	  // if not inserted previously, install the new key

	  if (!found) {
		int addrLen, size = sizeof(IndexKeyValue) + keyValue->keyLen;

		addr.bits = allocDocStore(idxHndls[0], size + docIdLen + INT_key, false);
		addrLen = store64(bytes, keyValue->keyLen + docIdLen, addr.addr, binaryFlds);
		keyValue->idxId = idxHndls[keyIdx]->map->arenaDef->id;
		keyValue->docIdLen = docIdLen;
		keyValue->addrLen = addrLen;
		keyValue->keyIdx = keyIdx;


		size += docIdLen + addrLen;

		memcpy (getObj(idxHndls[0]->map, addr), keyValue, sizeof(IndexKeyValue));
		memcpy ((uint8_t *)getObj(idxHndls[0]->map, addr) + sizeof(IndexKeyValue), bytes, size - sizeof(IndexKeyValue));
	  }  

	  //  add to our key membership

	  if (!keys->bits)
		iniMmbr(idxHndls[0]->map, keys, idxCnt);

	  *newMmbr(idxHndls[0]->map, keys, hash) = addr.bits;

	  // are we finished with multi-key?

	  if (!depth--)
		break;

	  // advance to next array element in multi-key

	  off = stack[depth].off;
	  idx = stack[depth].idx++;
	  keyValue->keyLen = stack[depth].keyLen;

	  if (idx < stack[depth].cnt)
	  	val = stack[depth++].values[idx];

	  continue;
	}
		
	//  append next key field

	off += sizeof(*spec);
	val = rec;
	nxt = 0;

	//	find field value

	for (fld = 0; fld < spec->numFlds; fld++) {
		field = (struct Field *)(base + off + nxt);
		nxt += *field->len + sizeof(struct Field);
		name.addr = (string_t *)field->len;
		name.bits = vt_string;

		if ((val = lookup(val, name, false, field->hash)).type == vt_object)
		  if (field->obj)
			continue;

		break;
	}

	//	handle multi-key spec

	if (val.type == vt_array) {
	  dbarray_t *dbaval = js_addr(val);
	  KeyStack *item = &stack[depth];

	  item->values = val.marshaled ? dbaval->valueArray : val.aval->valuePtr;
	  item->cnt = val.marshaled ? dbaval->cnt : vec_cnt(val.aval->valuePtr);
	  item->keyLen = keyValue->keyLen;
	  item->off = off;
	  item->idx = 1;

	  if (item->cnt)
	  	val = item->values[0];
	  else
		val.bits = vt_undef;

	  if (item->cnt > 1)
		depth++;
	}

	fldLen = keyFld(val, spec, keyValue, bytes, binaryFlds);

	if (fldLen < 0)
		break;

	off += (uint16_t)sizeof(IndexKeySpec) + nxt;
	keyValue->keyLen += fldLen;
  }
}

//	bind index DbHandles for document insert batch
//	returns a vector of index handles

Handle **bindDocIndexes(Handle *docHndl) {
	DocStore *docStore = (DocStore *)(docHndl + 1);
	Handle **idxHndls = NULL, *idxHndl;
	DbHandle *hndl;
	int idx;

	vec_push(idxHndls, docHndl);

	lockLatch (docStore->idxHndls->latch);

	//  enumerate all of the index arenas by id
	//	and add handles to the idxHndls vector

	for (idx = arrayFirst(sizeof(DbHandle)); idx < docStore->idxMax; idx++) {
	  if ((hndl = arrayEntry(docHndl->map, docStore->idxHndls, idx)))
		if ((idxHndl = bindHandle(hndl)))
		  vec_push(idxHndls, idxHndl);
	}

	unlockLatch (docStore->idxHndls->latch);
	return idxHndls;
}

value_t propIdxCount(value_t val, bool lVal) {
	DbHandle *hndl = val.addr;
	Handle *idxHndl;
	value_t count;

	count.bits = vt_int;
	count.nval = 0;

	if (val.type == vt_index)
	  if ((idxHndl = bindHandle(hndl))) {
		DbIndex *index = (DbIndex *)(idxHndl->map->arena + 1);
		count.nval = *index->numKeys;
		releaseHandle(idxHndl, hndl);
	  }

	return count;
}

value_t fcnIdxMakeKey(value_t *args, value_t thisVal, environment_t *env) {
	uint16_t off = sizeof(uint32_t);
	IndexKeyValue keyValue[1];
	uint8_t bytes[MAX_key];
	int fldLen, fld, nxt;
	struct Field *field;
	IndexKeySpec *spec;
	value_t val, name;
	bool binaryFlds;
	Handle *idxHndl;
	uint32_t keyMax;
	DbHandle *hndl;
	uint8_t *base;

	hndl = (DbHandle *)baseObject(thisVal)->hndl;
	val.bits = vt_status;

	if (!(idxHndl = bindHandle(hndl)))
		return val.status = DB_ERROR_handleclosed, val;

	binaryFlds = idxHndl->map->arenaDef->params[IdxKeyFlds].boolVal;

	base = getObj(idxHndl->map->db, idxHndl->map->arenaDef->params[IdxKeyAddr].addr);
	keyMax = *(uint32_t *)base;

	memset (keyValue, 0, sizeof(IndexKeyValue));

	if (args[0].type != vt_object)
		return val.status = ERROR_not_object, val;

  //	add each key field to the key (no multi-keys)

  while (off < keyMax) {
	spec = (IndexKeySpec *)(base + off);
	off += sizeof(*spec);
	val = args[0];
	nxt = 0;

	//	find field value

	for (fld = 0; fld < spec->numFlds; fld++) {
		field = (struct Field *)(base + off + nxt);
		nxt += *field->len + sizeof(struct Field);
		name.addr = (string_t *)field->len;
		name.bits = vt_string;

		if ((val = lookup(val, name, false, field->hash)).type == vt_object)
			continue;

		break;
	}

	fldLen = keyFld(val, spec, keyValue, bytes, binaryFlds);

	if (fldLen < 0)
		break;

	off += (uint16_t)sizeof(IndexKeySpec) + nxt;
	keyValue->keyLen += fldLen;
  }

  releaseHandle(idxHndl, hndl);
  val = newString(bytes, keyValue->keyLen);
  val.type = vt_key;
  return val;
}

value_t fcnIdxInsertKey(value_t *args, value_t thisVal, environment_t *env) {
	IndexKeyValue keyValue[1];
	Handle *idxHndl;
	DbHandle *hndl;
	string_t *str;
	value_t val;

	val.bits = vt_status;

	if (args[0].type != vt_key)
		return val.status = ERROR_not_key, val;

	str = js_addr(args[0]);

	memset (keyValue, 0, sizeof(IndexKeyValue));
	keyValue->keyLen = str->len;

	hndl = (DbHandle *)baseObject(thisVal)->hndl;

	if (!(idxHndl = bindHandle(hndl)))
		val.status = DB_ERROR_handleclosed;

	val.status = insertIdxKey(idxHndl, keyValue, str->val);
	return val;
}

value_t fcnIdxDeleteKey(value_t *args, value_t thisVal, environment_t *env) {
	IndexKeyValue keyValue[1];
	Handle *idxHndl;
	DbHandle *hndl;
	string_t *str;
	value_t val;

	memset (keyValue, 0, sizeof(IndexKeyValue));

	hndl = (DbHandle *)baseObject(thisVal)->hndl;
	val.bits = vt_status;

	if (args[0].type != vt_key)
		return val.status = ERROR_not_key, val;

	str = js_addr(args[0]);
	keyValue->keyLen = str->len;

	if ((idxHndl = bindHandle(hndl)))
		val.status = deleteIdxKey (idxHndl, keyValue, str->val);
	else
		val.status = DB_ERROR_handleclosed;

	return val;
}

//	convert key string to object

value_t fcnIdxConvKey(value_t *args, value_t thisVal, environment_t *env) {
	uint16_t off = sizeof(uint32_t);
	IndexKeyValue keyValue[1];
	value_t val, name, obj;
	uint8_t bytes[MAX_key];
	int fldLen, fld, nxt;
	struct Field *field;
	IndexKeySpec *spec;
	bool binaryFlds;
	uint32_t keyMax;
	Handle *idxHndl;
	string_t str[1];
	DbHandle *hndl;
	uint8_t *base;

	hndl = (DbHandle *)baseObject(thisVal)->hndl;
	val.bits = vt_status;

	if (args[0].type != vt_key)
		return val.status = ERROR_not_key, val;

	hndl = (DbHandle *)baseObject(thisVal)->hndl;

	if (!(idxHndl = bindHandle(hndl)))
		return val.status = DB_ERROR_handleclosed, val;

	base = getObj(idxHndl->map->db, idxHndl->map->arenaDef->params[IdxKeyAddr].addr);
	binaryFlds = idxHndl->map->arenaDef->params[IdxKeyFlds].boolVal;
	keyMax = *(uint32_t *)base;
	obj = newObj(vt_object);

  while (off < keyMax) {
	spec = (IndexKeySpec *)(base + off);
	off += sizeof(*spec);
	val = obj;
	nxt = 0;

	//	add next field value to object

	for (fld = 0; fld < spec->numFlds; fld++) {
		field = (struct Field *)(base + off + nxt);
		nxt += *field->len + sizeof(struct Field);
		name.addr = (string_t *)field->len;
		name.bits = vt_string;

		// find existing attribute

		if ((val = lookup(val, name, false, field->hash)).type == vt_object)
			continue;

		break;
	}

	fldLen = getFld(args[0], spec, keyValue, bytes, binaryFlds);

	if (fldLen < 0)
		break;

	off += (uint16_t)sizeof(IndexKeySpec) + nxt;

	val.bits = vt_status;
  }

  releaseHandle(idxHndl, hndl);
  return obj;
}

PropFcn builtinIdxFcns[] = {
	{ fcnIdxMakeKey, "makeKey" },
	{ fcnIdxConvKey, "convKey" },
	{ fcnIdxInsertKey, "insertKey" },
	{ fcnIdxDeleteKey, "deleteKey" },
	{ NULL, NULL}
};

PropVal builtinIdxProp[] = {
	{ propIdxCount, "count" },
	{ NULL, NULL}
};


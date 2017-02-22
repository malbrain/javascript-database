#include "js.h"
#include "js_db.h"
#include "js_dbindex.h"
#include "database/btree1/btree1.h"
#include "database/artree/artree.h"

#ifdef __linux__
#define offsetof(type,member) __builtin_offsetof(type,member)
#endif

#ifdef _WIN32
#define strncasecmp _strnicmp
#endif

extern DbMap memMap[1];

//	insert a key into an index

DbStatus insertIdxKey (Handle *idxHndl, void *keyBytes, uint32_t keyLen, uint32_t suffixLen, bool unique) {
	DbStatus stat;

	switch (*idxHndl->map->arena->type) {
	case Hndl_artIndex:
		stat = artInsertKey(idxHndl, keyBytes, keyLen + suffixLen);
		break;

	case Hndl_btree1Index:
		stat = btree1InsertKey(idxHndl, keyBytes, keyLen + suffixLen, 0, Btree1_indexed);
		break;
	}

	return stat;
}

//	allocate docStore power-of-two memory

uint64_t allocDocStore(Handle *docHndl, uint32_t size, bool zeroit) {
DbAddr *free = listFree(docHndl,0);
DbAddr *wait = listWait(docHndl,0);

	return allocObj(docHndl->map, free, wait, -1, size, zeroit);
}

int keyFld (value_t *field, IndexKeySpec *spec, IndexKeyValue *keyValue, bool binaryFlds) {
	uint8_t *buff = keyValue->bytes + keyValue->keyLen;
	uint32_t max = MAX_key - keyValue->keyLen;
	value_t val, src;
	int len = 0, off;
	string_t *str;

	if (field)
		src = *field;
	else
		src.bits = vt_undef;

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
			for (int idx = off; idx < len; idx++)
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
	  for (int idx = off; idx < len; idx++)
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
	object_t *oval = js_addr(keySpec);
    pair_t *pairs = oval->marshaled ? oval->pairArray : oval->pairsPtr;
    uint32_t cnt = oval->marshaled ? oval->cnt : vec_cnt(pairs);
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
//	and insert them into their index

void buildKeys(Handle *docHndl, Handle *idxHndl, value_t rec, DbAddr *keys, ObjId docId, Ver *prevVer, uint32_t idxCnt) {
	bool uniqueIdx = idxHndl->map->arenaDef->params[IdxKeyUnique].boolVal;
	bool binaryFlds = idxHndl->map->arenaDef->params[IdxKeyFlds].boolVal;
	uint8_t buff[MAX_key + sizeof(IndexKeyValue)];
	uint16_t depth = 0, off = sizeof(uint32_t);
	KeyStack stack[MAX_array_fields];
	object_t *oval = js_addr(rec);
	IndexKeyValue *keyValue;
	struct Field *field;
	value_t *val, name;
	IndexKeySpec *spec;
	uint32_t keyMax;
	object_t *nobj;
	uint8_t *base;
	DbAddr addr;
	int fldLen;
	int idx;
	int nxt;

  base = getObj(idxHndl->map->db, idxHndl->map->arenaDef->params[IdxKeyAddr].addr);
  keyMax = *(uint32_t *)base;

  //	create IndexKeyVelue structure in buff

  keyValue = (IndexKeyValue *)buff;
  memset (keyValue, 0, sizeof(IndexKeyValue));

  //	add each key field to the key, or multi-key

  while (true) {
	if (off < keyMax)
	  spec = (IndexKeySpec *)(base + off);
	else {
	  int docIdLen = store64(keyValue->bytes, keyValue->keyLen, docId.addr, binaryFlds);
	  uint64_t hash = hashStr(keyValue->bytes, keyValue->keyLen + docIdLen);

	  addr.bits = 0;

	  // try to find key in previous version

	  if (prevVer) {
		uint64_t *slot = getMmbr(docHndl->map, prevVer->keys, hash);

		while (slot && (addr.bits = *slot)) {
		  IndexKeyValue *prior = getObj(docHndl->map, addr);

		  if (prior->idxId == idxHndl->map->arenaDef->id)
		   if (prior->keyLen == keyValue->keyLen)
			if (prior->docIdLen == docIdLen)
			 if (!memcmp(prior->bytes, keyValue->bytes, prior->keyLen + prior->docIdLen))
				break;

		  slot = nxtMmbr(getObj(docHndl->map, *prevVer->keys), slot);
		}
	  }

	  // if not inserted previously, install the new key

	  if (!addr.bits) {
		int addrLen, size = sizeof(IndexKeyValue) + keyValue->keyLen;

		addr.bits = allocDocStore(docHndl, size + docIdLen + INT_key, false);
		addrLen = store64(keyValue->bytes, keyValue->keyLen + docIdLen, addr.addr, binaryFlds);
		keyValue->idxId = idxHndl->map->arenaDef->id;
		keyValue->docIdLen = docIdLen;
		keyValue->addrLen = addrLen;
		size += docIdLen + addrLen;

		memcpy (getObj(docHndl->map, addr), keyValue, size);
		insertIdxKey(idxHndl, keyValue->bytes, keyValue->keyLen, docIdLen + addrLen, uniqueIdx);
	  }  

	  //  add to our key membership

	  if (!keys->bits)
		iniMmbr(docHndl->map, keys, idxCnt);

	  *newMmbr(docHndl->map, keys, hash) = addr.bits;

	  // are we finished with multi-key?

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
		
	//  append next key field

	off += sizeof(*spec);
	nobj = oval;
	nxt = 0;

	for (int fld = 0; fld < spec->numFlds; fld++) {
		field = (struct Field *)(base + off + nxt);
		nxt += *field->len + sizeof(struct Field);
		name.addr = (string_t *)field->len;
		name.bits = vt_string;

		if ((val = lookup(nobj, name, false, field->hash))) {
		  if (val->type == vt_object) {
			nobj = js_addr(*val);
			continue;
		  } else
			break;
		} else
			break;
	}

	//	handle multi-key spec

	if (val && val->type == vt_array) {
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

	fldLen = keyFld(val, spec, keyValue, binaryFlds);

	if (fldLen < 0)
		break;

	off += sizeof(IndexKeySpec) + nxt;
	keyValue->keyLen += fldLen;
  }
}

//	bind index DbHandles for document insert batch
//	returns a vector of index handles

Handle **bindDocIndexes(Handle *docHndl) {
	DocStore *docStore = (DocStore *)(docHndl + 1);
	Handle **idxHndls = NULL, *idxHndl;
	DbAddr addr, *next, *slot;
	RedBlack *rbEntry;
	SkipEntry *entry;
	DbMap *map;

	vec_push(idxHndls, docHndl);

	lockLatch (docStore->idxHndls->latch);

	readLock (docHndl->map->arenaDef->idList->lock);
	next = docHndl->map->arenaDef->idList->head;

	//  enumerate all of the index arenas by id
	//	and add handles to the idxHndls vector

	while (next->addr) {
      SkipNode *skipNode = getObj(docHndl->map->db, *next);

	  for (int idx = 0; idx < next->nslot; idx++) {
		entry = skipAdd(memMap, docStore->idxHndls, *skipNode->array[idx].key);

		if (!*entry->val) {
		  addr.bits = skipNode->array[idx].val[0];
		  rbEntry = getObj(docHndl->map->db, addr);
		  map = arenaRbMap(docHndl->map, rbEntry);

		  idxHndl = makeHandle(map, 0, map->arenaDef->arenaType);
		  *entry->val = (uint64_t)idxHndl;
		} else
		  idxHndl = (Handle *)*entry->val;

		slot = fetchIdSlot(memMap, idxHndl->hndlId);

		if (enterHandle(idxHndl, slot))
			vec_push(idxHndls, (Handle *)*entry->val);
      }

      next = skipNode->next;
	}

	readUnlock (docHndl->map->arenaDef->idList->lock);
	unlockLatch (docStore->idxHndls->latch);
	return idxHndls;
}

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
/*
bool compareDups(DbMap *map, DbCursor *dbCursor) {
  Ver *ver;
  DbStatus stat = findCursorVer(dbCursor, map, NULL, &ver);

	return stat ? true : false;
}
*/

//  insert a key into an index

DbStatus insertKeyValue (Handle *idxHndl, KeyValue *keyValue) {
  //uint32_t totLen = keyValue->keyLen + keyValue->suffixLen;
  DbStatus stat = DB_ERROR_indextype;
  DbMap *map = MapAddr(idxHndl);

	switch (*map->arena->type) {
	case Hndl_artIndex:
//		if (keyValue->unique)
//		stat = artInsertUniq(idxHndl, keyValue->bytes, totLen, keyValue->keyLen, compareDups, (bool *)&keyValue->deferred);
//		else
			stat = artInsertKey(idxHndl, keyValue->bytes, keyValue->keyLen, keyValue->suffixLen );
		break;

	case Hndl_btree1Index:
		stat = btree1InsertKey(idxHndl, keyValue->bytes, keyValue->keyLen, keyValue->suffixLen, 0, Btree1_indexed);
		break;
	}

	return stat;
}

int keyFld(value_t src, KeySpec *spec, KeyValue *keyValue, bool binaryFlds) {
  uint8_t *buff = keyValue->bytes + keyValue->keyLen;
  uint32_t max = MAX_key - keyValue->keyLen;
  uint32_t len = 0, off, idx;
  string_t *str;
  value_t val;

  off = binaryFlds ? 2 : 0;

  while (true) {
    switch (spec->fldType & key_mask) {
      case key_undef:
        if (spec->fldType & key_first) switch (src.type) {
            case vt_int:
              spec->fldType |= key_int;
              continue;
            case vt_dbl:
              spec->fldType |= key_dbl;
              continue;
            case vt_string:
              spec->fldType |= key_str;
              continue;
            case vt_bool:
              spec->fldType |= key_bool;
              continue;
            case vt_key:
              spec->fldType |= key_str;
              continue;
            default:
              spec->fldType |= key_str;
              break;
          }
        break;

      case key_int:
        val = conv2Int(src, false);

        if (max < sizeof(uint64_t) + 2) return -1;

        len = store64(buff, 0, val.nval);
        abandonValue(val);
        break;

      case key_dbl:
        val = conv2Dbl(src, false);

        if (max < sizeof(uint64_t) + 2) return -1;

        // store double as int

        len = store64(buff, 0, val.nval);

        // if sign bit not set (negative), flip all the bits

        if (~buff[off] & 0x80)
          for (idx = off; idx < len; idx++) buff[idx] ^= 0xff;

        abandonValue(val);
        break;

      case key_bool:
        val = conv2Bool(src, false);
        len = 1;

        if (len > max) return -1;

        if (binaryFlds)
          len = store64(buff, 0, val.boolean ? 1 : 0);
        else
          buff[0] = val.boolean ? 1 : 0;

        abandonValue(val);
        break;

      default:
        src = conv2Str(src, false, false);

      case key_str:
        str = js_dbaddr(src, NULL);

        len = str->len;

        if (len > max) return -1;

        memcpy(buff + off, str->val, len);

        if (!binaryFlds)
          buff[len++] = 0;
        else {
          buff[0] = (len - 2) >> 8;
          buff[1] = (len - 2);
        }

        break;
    }

    break;
  }

  if (spec->fldType & key_reverse)
    for (idx = off; idx < len; idx++) buff[idx] ^= 0xff;

  return len;
}

bool type_cmp(uint8_t *type, int amt, char *val) {
  if (strlen(val) != amt) return false;
  if (!memcmp(type, val, amt)) return true;
  return false;
}

uint32_t eval_option(uint8_t *opt, int amt) {
  if (type_cmp(opt, amt, "fwd")) return 0;

  if (type_cmp(opt, amt, "rev")) return key_reverse;

  if (type_cmp(opt, amt, "int")) return key_int;

  if (type_cmp(opt, amt, "dbl")) return key_dbl;

  if (type_cmp(opt, amt, "bool")) return key_bool;

  if (type_cmp(opt, amt, "string")) return key_str;

  return 0;
}

uint32_t key_options(value_t option) {
  string_t *str = js_dbaddr(option, NULL);
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

  else if (option.type == vt_string && len)
    do {
      for (amt = 0; amt < len; amt++)
        if (opt[amt] == ':') break;
      val |= eval_option(opt, amt);
      if (amt < len) amt++;
      opt += amt;
    } while ((len -= amt));

  else
    return key_first;

  return val;
}

value_t fcnIdxInsKey(value_t *args, value_t thisVal, environment_t *env) {
  Handle *idxHndl;
  uint8_t buff[MAX_key + sizeof(KeyValue)];
  KeyValue *keyValue = (KeyValue *)buff;
  KeySpec spec[1];
  DbMap *idxMap;
  DbIndex *index;  
  int cnt = vec_cnt(args), len;
  int idx = 0, off = sizeof(KeyValue);
  value_t v, s, docId = args[0];
  
  s.bits = vt_status;

  if(cnt == 0) 
	  return s.status = ERROR_empty_argument_list, s;

  if ((idxHndl = js_handle(thisVal, Hndl_anyIdx)))
      idxMap = MapAddr(idxHndl);
    else
      return s.status = DB_ERROR_handleclosed, s;

  index = (DbIndex *)(idxMap->arena + 1);

  keyValue = (KeyValue *)buff;
  memset(keyValue, 0, sizeof(KeyValue));

  if ( docId.type == vt_docId)
   while (++idx < cnt) {
    value_t val = args[idx];

    switch (val.type) {
      case vt_int:
        spec->fldType = key_int;
        break;
      case vt_dbl:
        spec->fldType = key_dbl;
        break;
      case vt_string:
        spec->fldType = key_str;
        break;
      case vt_bool:
        spec->fldType = key_bool;
        break;
      case vt_key:
        spec->fldType = key_str;
        break;
      default:
        spec->fldType = key_str;
        val = conv2Str(val, false, false);
        break;
    }
    spec->numFlds = 1;
    spec->field.len[0] = 0;

    len = keyFld(val, spec, keyValue, index->binaryFlds);
    keyValue->keyLen += len;
   }
  else
    return s.status = DB_ERROR_badrecid, s;

  keyValue->suffixLen =
      store64(keyValue->bytes, keyValue->keyLen, docId.idBits);

  if((s.status = insertKeyValue(idxHndl, keyValue)))
    return s;

  v = newString(keyValue->bytes, keyValue->keyLen + keyValue->suffixLen);
  v.type = vt_key;
  return v;
}
	
//	delete a key from an index

JsStatus deleteIdxKey (Handle *idxHndl, KeyValue *keyValue) {
	uint32_t totLen = keyValue->keyLen + keyValue->suffixLen;
	JsStatus stat = (JsStatus)OK;
    DbMap *map = MapAddr(idxHndl);

	switch (*map->arena->type) {
	case Hndl_artIndex:
		stat = (JsStatus)artDeleteKey(idxHndl, keyValue->bytes, totLen, keyValue->keyLen);
		break;

	case Hndl_btree1Index:
		stat = (JsStatus)btree1DeleteKey(idxHndl, keyValue->bytes, totLen);
		break;
	}

	return stat;
}

//  un-install version's keys
/*
JsStatus removeKeys(Handle **idxHndls, Ver *ver, DbMmbr *mmbr, DbAddr *slot) {
	Handle *docHndl = idxHndls[0];
	JsStatus stat = (JsStatus)OK;

	if (!mmbr) {
	  if (ver->keys->addr)
		mmbr = getObj(docHndl->map, *ver->keys);
	  else
		return stat;
	}

	while ((slot = revMmbr(mmbr, &slot->bits))) {
	  IndexKeyValue *keyValue = getObj(docHndl->map, *slot);

	  if (!atomicAdd64(keyValue->refCnt, (uint64_t)(-1LL))) {
		if ((stat = deleteIdxKey(idxHndls[keyValue->keyIdx], keyValue)))
		  return stat;
	  }
	}

	return stat;
}
*/

//	compile index key into permanent spot in the index

DbAddr compileKey(Handle *idxHndl, value_t keySpec) {
	dbobject_t *dboval = js_dbaddr(keySpec, NULL);
    pair_t *pairs = keySpec.marshaled ? dboval->pairs : keySpec.oval->pairsPtr;
    uint32_t cnt = keySpec.marshaled ? dboval->cnt : vec_cnt(pairs);
	uint32_t idx, off, fld;
	struct Field *field;
	KeySpec *spec;
    DbMap *idxMap;
	uint8_t *base;
	string_t *str;
	uint32_t size = 0;
	DbAddr slot;

	slot.bits = 0;
	idxMap = MapAddr(idxHndl);

	for( idx = 0; idx < cnt; idx++) {
		size += sizeof(KeySpec) + sizeof(struct Field);
		str = js_dbaddr(pairs[idx].name, NULL);

		//  go through field name

		for (fld = 0; fld < str->len; fld++)
		  if (str->val[fld] == '.')
			size += sizeof(struct Field);
		  else
			size++;
	}

	//	allocate space to compile key structure

	slot.bits = allocBlk(idxMap, size, true);
	base = getObj(idxMap, slot);
	off = sizeof(uint32_t);

	//	fill in each compound key spec

	for (idx = 0; idx < cnt; idx++) {
		spec = (KeySpec *)(base + off);
		str = js_dbaddr(pairs[idx].name, NULL);
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

	*(uint32_t *)base = off;
	return slot;
}

//  build all keys from a document

value_t fcnIdxBldKey(value_t *args, value_t thisVal, environment_t *env) {
  Handle *idxHndl, *docHndl;
  DbMap *idxMap;
  DbIndex *index;
  uint16_t depth = 0, off = sizeof(uint32_t);
  value_t keys[1], *next;
  value_t s;
  uint32_t idx = 0, reps = 0, cnt, arg = 0;
  dbarray_t *dbaval;
  ObjId docId[1];

  *keys = newArray(array_value, 0);
  s.bits = vt_status;
  cnt = vec_cnt(args);

  if (cnt < 2) return s.status = ERROR_empty_argument_list, s;

  if ((idxHndl = js_handle(thisVal, Hndl_anyIdx)))
    idxMap = MapAddr(idxHndl);
  else
    return s.status = ERROR_empty_argument_list, s;

  index = (DbIndex *)(idxMap->arena + 1);
  arg = 0;
  reps = 0;

  while (arg < vec_cnt(args)) {
    next = args + arg;
    do {
      switch (next->type) {
        case vt_array:
          if (idx == 0) {
            dbaval = js_dbaddr(args[arg], NULL);
            next = args[arg].marshaled ? dbaval->valueArray : args[arg].aval->valuePtr;
            reps = args[arg].marshaled ? dbaval->cnt : vec_cnt(next);
          } else
            next++;

          if (idx++ < reps)
            continue;

          next = NULL;
          reps = 0;
          idx = 0;
          break;

        case vt_docId:
          docId->bits = next->idBits;
          docHndl = getDocIdHndl(next->hndlIdx);
          next = NULL;
          break;

        case vt_document:
          next = jsDocAddr(*next)->value;
          break;

        default:
          next = NULL;
          break;
      }

      if (next == NULL) {
        arg++;
        break;
      }

      if ((s.status = idxBldKeyHelper(*next, docId, docHndl, idxHndl, keys)))
        return s;

    } while (next);
  }

  return *keys;
}

//	helper function to fill KeyValue structure in buff
// and insert index keys for the document

DbStatus idxBldKeyHelper(value_t rec, ObjId *docId, Handle *docHndl, 
  Handle *idxHndl, value_t *keys) {
  DbMap *idxMap = MapAddr(idxHndl);
  DbIndex *index = (DbIndex *)(idxMap->arena + 1);
  uint8_t buff[MAX_key + sizeof(KeyValue)];
  uint16_t depth = 0, off = sizeof(uint32_t);
  KeyStack stack[MAX_array_fields];
  int fldLen, nxt;
  KeyValue *keyValue;
  uint8_t *base;
  value_t v, name;
  uint32_t specMax, fld;

  base = getObj(idxMap, index->keySpec);
  specMax = *(uint32_t *)base;

  keyValue = (KeyValue *)buff;
  memset(keyValue, 0, sizeof(KeyValue));

  //	add each key field to the key, or multi-key

  while (off < specMax) {
    KeySpec *spec = (KeySpec *)(base + off);
    value_t val;

    val = rec;
    nxt = sizeof(KeySpec);

    // compound lookup

    for (fld = 0; fld < spec->numFlds; fld++) {
      struct Field *field = (struct Field *)(base + off + nxt);
      nxt += *field->len + sizeof(struct Field);

      name.addr = (string_t *)field->len;  //recast field->len as str->len
      name.bits = vt_string;

      val = lookup(val, name, false, field->hash);
    }

    switch (val.type) {
      case vt_object:
        val = getDocObject(val);
        continue;

        //	handle multi-key spec

      case vt_array: {
          dbarray_t *dbaval = js_dbaddr(val, NULL);
          KeyStack *item = &stack[depth];

          //  new array?

          if (!depth || off > item[-1].off) {
            item->values = val.marshaled ? dbaval->valueArray : val.aval->valuePtr;
            item->cnt = val.marshaled ? dbaval->cnt : vec_cnt(val.aval->valuePtr);
            item->keyLen = keyValue->keyLen;
            item->off = off;
            item->idx = 0;

            if (item->cnt == 0) {
              off += nxt;
              continue;
            }

            depth++;
          }

          // last element?

          val = item->values[item->idx++];

          if (item->idx < item->cnt)
            break;

          depth--;
          break;
        }
    }


    // append next field

    fldLen = keyFld(val, spec, keyValue, index->binaryFlds);

    if (fldLen < 0)
      break;

    off += nxt;
    keyValue->keyLen += fldLen;

    // last key field?

    if (off < specMax)
      continue;

    installKey(keyValue, docId, docHndl, idxHndl);

    v = newString(keyValue->bytes, keyValue->keyLen);
    v.type = vt_key;

    vec_push(keys->aval->valuePtr, v);

    // are we finished with multi-key?

    if (depth) {
      KeyStack *item = &stack[--depth];
      keyValue->keyLen = item->keyLen;
      off = item->off;
      continue;
    }
  }
  return DB_OK;
}

DbStatus installKey(KeyValue * keyValue, ObjId * docId, Handle *docHndl, Handle * idxHndl) {
  int docIdLen = store64(keyValue->bytes, keyValue->keyLen, docId->bits);
  uint64_t hash = hashStr(keyValue->bytes, keyValue->keyLen + docIdLen);
  uint32_t size = sizeof(KeyValue) + keyValue->keyLen + docIdLen;
  DbMap *docMap = MapAddr(docHndl);
  DbAddr addr;
  value_t s;

  // install the new key

  keyValue->keyHash = hash;
  keyValue->suffixLen = docIdLen;

  s.status = insertKeyValue(idxHndl, keyValue);

  addr.bits = allocDocStore(docHndl, size + INT_key, false);
  keyValue->suffixLen = docIdLen;

  memcpy(getObj(docMap, addr), keyValue, size);

  return insertKeyValue(idxHndl, keyValue);
}

value_t propIdxCount(value_t val, bool lVal) {
	DbHandle *hndl = val.addr;
	Handle *idxHndl;
	value_t count;

	count.bits = vt_int;
	count.nval = 0;

	if (val.type == vt_hndl)
	  if ((idxHndl = bindHandle(hndl, Hndl_anyIdx))) {
          DbMap *map = MapAddr(idxHndl);
		  DbIndex *index = (DbIndex *)(map->arena + 1);
		count.nval = *index->numKeys;
		releaseHandle(idxHndl);
	  }

	return count;
}

int toHex(int ch) {
  if (ch < 10) return ch | 0x30;
  return 0x41 - 10 + ch;
}

value_t fcnKeyToString(value_t *args, value_t thisVal, environment_t *env) {
  uint32_t idx = 0, len = thisVal.str->len * 3;
  string_t *str = js_alloc(len + sizeof(string_t), false);
  value_t ans;

  str->len = len - 1;
  ans.bits = vt_string;
  ans.str = str;

  while (idx < thisVal.str->len) {
    int ch = thisVal.str->val[idx];
    str->val[idx * 3] = toHex(ch / 16);
    str->val[idx * 3 + 1] = toHex(ch % 16);
    str->val[idx++ * 3 + 2] = '.';
  }

  str->val[len] = 0;
  return ans;
}

PropFcn builtinIdxFcns[] = {
    { fcnIdxInsKey, "insertKey" },
    { fcnIdxBldKey, "buildKey"},
    { NULL, NULL}};

PropVal builtinIdxProp[] = {
	{ propIdxCount, "count" },
	{ NULL, NULL}
};

PropVal builtinKeyProp[] = {
	{NULL, NULL}
};

PropFcn builtinKeyFcns[] = {
    {fcnKeyToString, "toString"},
	{NULL, NULL}
};
#include "js.h"
#include "js_props.h"

#include "js_db.h"
#include "js_dbindex.h"

#define INT_key 12		// max extra bytes store64 creates

extern CcMethod *cc;

//	see if version has the key

JsStatus findCursorVer(DbCursor *dbCursor, DbMap *map, JsMvcc *jsMvcc) {
	bool binaryFlds = map->arenaDef->params[IdxKeyFlds].boolVal;
	bool found = false;
	DbAddr addr, *idSlot;
	uint64_t hash;
	ObjId docId;
	int suffix;
	Ver *ver;
	Doc *doc;

	//  throw away the KeyValue address

	suffix = get64 (dbCursor->key, dbCursor->keyLen, &addr.bits, binaryFlds);

	//  get the docId from the key

	get64 (dbCursor->key, dbCursor->keyLen - suffix, &docId.bits, binaryFlds);
	hash = hashStr(dbCursor->key, dbCursor->keyLen - suffix);
	idSlot = fetchIdSlot(map->parent, docId);

	if (!idSlot->bits)
		return NULL;

	// first get the mvcc version for the document

	doc = getObj(map->parent, *idSlot);
	ver = findDocVer(map->parent, doc, jsMvcc);

	if (jsError(ver))
		return ver;

	//	now see if this key goes with this version

	if (ver->keys->bits) {
	  DbMmbr *mmbr = getObj(map->parent, *ver->keys);
	  DbAddr *mmbrSlot = getMmbr(mmbr, hash);

	  while (mmbrSlot && mmbrSlot->bits) {
		IndexKeyValue *prior = getObj(map->parent, *mmbrSlot);

	    if (prior->idxId == map->arenaDef->id)
		 if (prior->keyLen + prior->docIdLen == dbCursor->keyLen - suffix)
		  if (!memcmp(prior->bytes, dbCursor->key, prior->keyLen)) {
			found = true;
			break;
		  }

		mmbrSlot = nxtMmbr(mmbr, &mmbrSlot->bits);
	  }
	}

	//  only return a given docId one time

	if (!found)
		return NULL;

	if (dbCursor->deDup) {
	  uint64_t *mmbrSlot = setMmbr(map->parent, jsMvcc->deDup, docId.bits, true);

	  if (*mmbrSlot == 0 || *mmbrSlot == ~0LL)
		*mmbrSlot = docId.bits;
	  else
		ver = NULL;
	}

	return ver;
}

//	move cursor

value_t fcnCursorMove(value_t *args, value_t thisVal, environment_t *env) {
	DbCursor *dbCursor;
	Ver *ver = NULL;
	Handle *idxHndl;
	value_t op, val;
	JsMvcc *jsMvcc;
	DbHandle *hndl;

	hndl = (DbHandle *)baseObject(thisVal)->hndl;
	val.bits = vt_status;

	op = conv2Int(args[0], false);

	if (!(idxHndl = bindHandle(hndl)))
		return val.status = DB_ERROR_handleclosed, val;

	dbCursor = (DbCursor *)(idxHndl + 1);
	jsMvcc = (JsMvcc *)(dbCursor + 1);

	while (!ver || jsError(ver)) {
	  switch (op.nval) {
	  case OpLeft:
		val.status = dbLeftKey(dbCursor, idxHndl->map);
		break;

	  case OpRight:
		val.status = dbRightKey(dbCursor, idxHndl->map);
		break;

	  case OpNext:
		if ((val.status = dbNextKey(dbCursor, idxHndl->map)))
			break;

		ver = findCursorVer(dbCursor, idxHndl->map, jsMvcc);
		continue;

	  case OpPrev:
		if ((val.status = dbPrevKey(dbCursor, idxHndl->map)))
			break;

		ver = findCursorVer(dbCursor, idxHndl->map, jsMvcc);
		continue;

	  default:
		val.status = DB_ERROR_cursorop;
	  }

	  break;
	}

	releaseHandle(idxHndl, hndl);

	if (ver && !jsError(ver)) {
	  if (!(jsMvcc->docHndl = bindHandle(jsMvcc->hndl)))
		val.status = DB_ERROR_handleclosed;
	  else {
		val = makeDocument(ver, jsMvcc->docHndl);
		releaseHandle(jsMvcc->docHndl, jsMvcc->hndl);
	  }
	}

	return val;
}

value_t fcnCursorPos(value_t *args, value_t thisVal, environment_t *env) {
	value_t op, val, key;
	DbCursor *dbCursor;
	Ver *ver = NULL;
	Handle *idxHndl;
	JsMvcc *jsMvcc;
	DbHandle *hndl;
	string_t *str;

	hndl = (DbHandle *)baseObject(thisVal)->hndl;
	val.bits = vt_status;

	if (!(idxHndl = bindHandle(hndl)))
		return val.status = DB_ERROR_handleclosed, val;

	dbCursor = (DbCursor *)(idxHndl + 1);
	jsMvcc = (JsMvcc *)(dbCursor + 1);

	op = conv2Int(args[0], false);
	key = conv2Str(args[1], false, false);
	str = js_addr(key);

	do val.status = dbFindKey(dbCursor, idxHndl->map, str->val, str->len, op.nval);
	while (!val.status && !(ver = findCursorVer(dbCursor, idxHndl->map, jsMvcc)));

	releaseHandle(idxHndl, hndl);

	if (!(jsMvcc->docHndl = bindHandle(jsMvcc->hndl)))
		return val.status = DB_ERROR_handleclosed, val;

	val = makeDocument(ver, jsMvcc->docHndl);
	releaseHandle(jsMvcc->docHndl, jsMvcc->hndl);
	return val;
}

value_t fcnCursorKeyAt(value_t *args, value_t thisVal, environment_t *env) {
	uint32_t keyLen;
	DbHandle *hndl;
	void *keyStr;
	value_t s;

	s.bits = vt_status;
	hndl = (DbHandle *)baseObject(thisVal)->hndl;

	if ((s.status = keyAtCursor(hndl, &keyStr, &keyLen)))
		return s;

	return newString(keyStr, keyLen);
}

value_t fcnCursorDocAt(value_t *args, value_t thisVal, environment_t *env) {
	value_t s;

	s.bits = vt_status;
	return s;
}

//	clear cursor

value_t fcnCursorReset(value_t *args, value_t thisVal, environment_t *env) {
	DbCursor *dbCursor;
	Handle *idxHndl;
	DbHandle *hndl;
	JsMvcc *jsMvcc;
	uint64_t bits;
	DbAddr next;
	value_t s;

	s.bits = vt_status;
	hndl = (DbHandle *)baseObject(thisVal)->hndl;

	if (!(idxHndl = bindHandle(hndl)))
		return s.status = DB_ERROR_handleclosed, s;

	dbCursor = (DbCursor *)(idxHndl + 1);
	jsMvcc = (JsMvcc *)(dbCursor + 1);
	bits = jsMvcc->deDup->bits;

	//	clear deDup hash table

	while ((next.bits = bits)) {
		DbMmbr *mmbr = getObj(idxHndl->map->parent, next);
		bits = mmbr->next.bits;

		freeBlk(idxHndl->map->parent, next);
	}

	jsMvcc->deDup->bits = 0;
	jsMvcc->txnId.bits = *env->txnBits;
	newTs (jsMvcc->reader, env->timestamp, true);

	s.status = dbLeftKey(dbCursor, idxHndl->map);
	releaseHandle(idxHndl, hndl);
	return s;
}

PropFcn builtinCursorFcns[] = {
	{ fcnCursorPos, "pos" },
	{ fcnCursorMove, "move" },
	{ fcnCursorKeyAt, "keyAt" },
	{ fcnCursorDocAt, "docAt" },
	{ fcnCursorReset, "reset" },
	{ NULL, NULL}
};

PropVal builtinCursorProp[] = {
//	{ propIdxOnDisk, "onDisk" },
	{ NULL, NULL}
};


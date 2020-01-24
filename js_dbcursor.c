#include "js.h"
#include "js_props.h"

#include "js_db.h"
#include "js_dbindex.h"

#define INT_key 12		// max extra bytes creates


//	move cursor

value_t fcnCursorMove(value_t *args, value_t thisVal, environment_t *env) {
  DbCursor *dbCursor;
  DbMap *map, *docMap;
  Handle *idxHndl;
  value_t op, val;
  ObjId *docId;

  val.bits = vt_status;

  if (thisVal.hndl->hndlId.bits)
    if ((idxHndl = bindHandle(thisVal.hndl, Hndl_anyIdx)))
      dbCursor = ClntAddr(idxHndl);
    else
      return val.status = DB_ERROR_handleclosed, val;
  else
    return val.status = DB_ERROR_handleclosed, val;

  op = conv2Int(args[0], false);
  map = MapAddr(idxHndl);
  docMap = map->parent;

  switch (op.nval) {
    case OpLeft:
      val.status = dbLeftKey(dbCursor, map);
      break;

    case OpRight:
      val.status = dbRightKey(dbCursor, map);
      break;

    case OpNext:
      val.status = dbNextKey(dbCursor, map);
      break;

    case OpPrev:
      val.status = dbPrevKey(dbCursor, map);
      break;

    default:
      val.status = DB_ERROR_cursorop;
  }

  if (!val.status) {
    docId = dbGetDocId(dbCursor, docMap);
    val = makeDocument(*docId, docMap);
  }

  releaseHandle(idxHndl, thisVal.hndl);
  return val;
}

value_t fcnCursorPos(value_t *args, value_t thisVal, environment_t *env) {
	value_t op, val, key;
	DbCursor *dbCursor;
	Handle *idxHndl;
    DbMap *docMap;
	string_t *str;
    ObjId *docId;

	val.bits = vt_status;

	if ((thisVal.hndl->hndlId.bits))
        if (!(idxHndl = bindHandle(thisVal.hndl, Hndl_anyIdx)))
            return val.status = DB_ERROR_handleclosed, val;
        else
            docMap = MapAddr(idxHndl);
    else
       return val.status = DB_ERROR_handleclosed, val;

	dbCursor = ClntAddr(idxHndl);

	op = conv2Int(args[0], false);
	key = conv2Str(args[1], false, false);
	str = js_addr(key);

	val.status = dbFindKey(dbCursor, docMap, str->val, str->len, op.nval);

    if(!val.status) {
	  docId = dbGetDocId(dbCursor, docMap);
      val = makeDocument(*docId, docMap);
    }

    releaseHandle(idxHndl, thisVal.hndl);
    return val;
}

value_t fcnCursorKeyAt(value_t *args, value_t thisVal, environment_t *env) {
	uint32_t keyLen;
	value_t s, val;
	uint8_t *keyStr;

	s.bits = vt_status;

	if ((s.status = keyAtCursor(thisVal.hndl, &keyStr, &keyLen)))
		return s;

	val = newString(keyStr, keyLen);
	val.type = vt_key;
	return val;
}

value_t fcnCursorDocAt(value_t *args, value_t thisVal, environment_t *env) {
  value_t val;
  DbCursor *dbCursor;
  ObjId *docId;
  Handle *idxHndl;
  DbMap *docMap;

  val.bits = vt_status;

  if ((thisVal.hndl->hndlId.bits))
    if (!(idxHndl = bindHandle(thisVal.hndl, Hndl_anyIdx)))
      return val.status = DB_ERROR_handleclosed, val;
    else
      docMap = MapAddr(idxHndl);
  else
    return val.status = DB_ERROR_handleclosed, val;

  dbCursor = ClntAddr(idxHndl);

  docId = dbGetDocId(dbCursor, docMap);
  return makeDocument(*docId, docMap);
}

//	clear cursor

value_t fcnCursorReset(value_t *args, value_t thisVal, environment_t *env) {
	DbCursor *dbCursor;
	Handle *idxHndl;
	uint64_t bits;
    DbMap *map;
	DbAddr next;
	value_t s;

	s.bits = vt_status;

	if ((thisVal.hndl->hndlId.bits))
	  if (!(idxHndl = bindHandle(thisVal.hndl, Hndl_anyIdx)))
		return s.status = DB_ERROR_handleclosed, s;

	map = MapAddr(idxHndl);
	dbCursor = ClntAddr(idxHndl);
    bits = dbCursor->deDupHash.bits;
	
    //	clear deDup hash table

	while ((next.bits = bits)) {
		DbMmbr *mmbr = getObj(map->parent, next);
		bits = mmbr->next.bits;

		freeBlk(map->parent, next);
	}

	s.status = dbLeftKey(dbCursor, map);
	releaseHandle(idxHndl, thisVal.hndl);
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

#include "js.h"
#include "js_props.h"

#include "js_db.h"
#include "js_dbindex.h"

#define INT_key 12		// max extra bytes creates


//	move cursor

value_t fcnCursorMove (value_t *args, value_t thisVal, environment_t *env) {
  DbCursor *dbCursor;
  DbMap *map, *docMap;
  Handle *idxHndl;
  value_t op, val, s;

  s.bits = vt_status;

  if ((idxHndl = js_handle(thisVal, Hndl_cursor)))
      dbCursor = ClntAddr(idxHndl);
  else
      return s.status = DB_ERROR_handleclosed, s;

  op = conv2Int(args[0], false);
  map = MapAddr(idxHndl);
  docMap = map->parent;

  if ((s.status = moveCursor(idxHndl->hndl, op.nval))) 
      return s;

  val.bits = vt_docId;
  val.idBits = dbGetDocId(dbCursor).bits;
  val = makeDocument(dbGetDocId(dbCursor), docMap);
  releaseHandle(idxHndl);
  return val;
}


value_t fcnCursorPos(value_t *args, value_t thisVal, environment_t *env) {
    value_t op, val, key;
	DbCursor *dbCursor;
	Handle *idxHndl;
    DbMap *docMap;
	string_t *str;
    ObjId docId;

	val.bits = vt_status;

    if (!(idxHndl = js_handle(thisVal, Hndl_cursor)))
        return val.status = DB_ERROR_handleclosed, val;
    else
        docMap = (MapAddr(idxHndl))->parent;

	dbCursor = ClntAddr(idxHndl);

	op = conv2Int(args[0], false);
	key = conv2Str(args[1], false, false);
	str = js_dbaddr(key, NULL);

	val.status = dbFindKey(dbCursor, docMap, str->val, str->len, op.nval);

    if(!val.status) {
	  docId = dbGetDocId(dbCursor);
      val = makeDocument(docId, docMap);
    }

    return val;
}

value_t fcnCursorKeyAt(value_t *args, value_t thisVal, environment_t *env) {
  uint32_t keyLen;
  value_t s, val, cursor;
  uint8_t *keyStr;

    cursor = thisVal;
	s.bits = vt_status;

    while (cursor.type == vt_object) 
        cursor = *cursor.oval->baseVal;

    if ((s.status = keyAtCursor(cursor.hndl, &keyStr, &keyLen)))
        return s;

	val = newString(keyStr, keyLen);
	val.type = vt_key;
    return val;
}

value_t fcnCursorDocAt(value_t *args, value_t thisVal, environment_t *env) {
  value_t val;
  DbCursor *dbCursor;
  ObjId docId;
  Handle *idxHndl;
  DbMap *docMap;

  val.bits = vt_status;

  if (!(idxHndl = js_handle(thisVal, Hndl_cursor)))
      return val.status = DB_ERROR_handleclosed, val;
    else
      docMap = (MapAddr(idxHndl))->parent;

  dbCursor = ClntAddr(idxHndl);

  docId = dbGetDocId(dbCursor);

  releaseHandle(idxHndl);
  return makeDocument(docId, docMap);
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

	if ((idxHndl = js_handle(thisVal, Hndl_cursor)))
        map = MapAddr(idxHndl);
    else
       return s.status = DB_ERROR_handleclosed, s;

	dbCursor = ClntAddr(idxHndl);
    bits = dbCursor->deDupHash.bits;
	
    //	clear deDup hash table

	while ((next.bits = bits)) {
		DbMmbr *mmbr = getObj(map->parent, next);
		bits = mmbr->next.bits;

		freeBlk(map->parent, next);
	}

	s.status = dbLeftKey(dbCursor, map);
	releaseHandle(idxHndl);
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

#include "js.h"
#include "js_props.h"

#include "js_db.h"
#include "js_dbindex.h"

#define INT_key 12		// max extra bytes creates


//	move cursor

value_t fcnCursorMove (value_t *args, value_t thisVal, environment_t *env) {
  value_t cursHndl = js_handle(thisVal, Hndl_cursor), s;
  DbCursor *dbCursor;
  DbMap *map, *docMap;
  Handle *idxHndl;
  value_t op, val;
  ObjId docId;

  s.bits = vt_status;

  if ((cursHndl.type == vt_hndl))
    if ((idxHndl = bindHandle(cursHndl.hndl, Hndl_cursor)))
      dbCursor = ClntAddr(idxHndl);
    else
      return val.status = DB_ERROR_handleclosed, val;
  else
    return val.status = DB_ERROR_handleclosed, val;

  op = conv2Int(args[0], false);
  map = MapAddr(idxHndl);
  docMap = map->parent;

  if ((s.status = moveCursor(cursHndl.hndl, op.nval))) 
      return s;

  docId = dbGetDocId(dbCursor);
  val = makeDocument(docId, docMap);

  releaseHandle(idxHndl, thisVal.hndl);
  return val;
}

value_t fcnCursorPos(value_t *args, value_t thisVal, environment_t *env) {
    value_t cursHndl = js_handle(thisVal, Hndl_cursor);
    value_t op, val, key;
	DbCursor *dbCursor;
	Handle *idxHndl;
    DbMap *docMap;
	string_t *str;
    ObjId docId;

	val.bits = vt_status;

	if ((cursHndl.type == vt_hndl))
          if (!(idxHndl = bindHandle(cursHndl.hndl, Hndl_cursor)))
            return val.status = DB_ERROR_handleclosed, val;
        else
            docMap = (MapAddr(idxHndl))->parent;
    else
       return val.status = DB_ERROR_handleclosed, val;

	dbCursor = ClntAddr(idxHndl);

	op = conv2Int(args[0], false);
	key = conv2Str(args[1], false, false);
	str = js_dbaddr(key, NULL);

	val.status = dbFindKey(dbCursor, docMap, str->val, str->len, op.nval);

    if(!val.status) {
	  docId = dbGetDocId(dbCursor);
      val = makeDocument(docId, docMap);
    }

    releaseHandle(idxHndl, thisVal.hndl);
    return val;
}

value_t fcnCursorKeyAt(value_t *args, value_t thisVal, environment_t *env) {
  value_t cursHndl = js_handle(thisVal, Hndl_cursor);
  uint32_t keyLen;
  value_t s, val;
  uint8_t *keyStr;

	s.bits = vt_status;

	if ((cursHndl.type == vt_hndl))
      if ((s.status = keyAtCursor(cursHndl.hndl, &keyStr, &keyLen)))
        return s;

	val = newString(keyStr, keyLen);
	val.type = vt_key;
    return val;
}

value_t fcnCursorDocAt(value_t *args, value_t thisVal, environment_t *env) {
  value_t cursHndl = js_handle(thisVal, Hndl_cursor);
  value_t val;
  DbCursor *dbCursor;
  ObjId docId;
  Handle *idxHndl;
  DbMap *docMap;

  val.bits = vt_status;

  if ((cursHndl.type == vt_hndl))
    if (!(idxHndl = bindHandle(cursHndl.hndl, Hndl_cursor)))
      return val.status = DB_ERROR_handleclosed, val;
    else
      docMap = (MapAddr(idxHndl))->parent;
  else
    return val.status = DB_ERROR_handleclosed, val;

  dbCursor = ClntAddr(idxHndl);

  docId = dbGetDocId(dbCursor);
  releaseHandle(idxHndl, thisVal.hndl);
  return makeDocument(docId, docMap);
}

//	clear cursor

value_t fcnCursorReset(value_t *args, value_t thisVal, environment_t *env) {
  value_t cursHndl = js_handle(thisVal, Hndl_cursor);
  DbCursor *dbCursor;
	Handle *idxHndl;
	uint64_t bits;
    DbMap *map;
	DbAddr next;
	value_t s;

	s.bits = vt_status;

	if ((cursHndl.type == vt_hndl))
	  if (!(idxHndl = bindHandle(cursHndl.hndl, Hndl_cursor)))
		return s.status = DB_ERROR_handleclosed, s;
          else
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

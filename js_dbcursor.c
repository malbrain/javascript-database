#include "js.h"
#include "js_props.h"
#include "js_dbindex.h"
#include "database/db.h"
#include "database/db_object.h"
#include "database/db_handle.h"
#include "database/db_arena.h"
#include "database/db_index.h"
#include "database/db_txn.h"

#define INT_key 10		// max extra bytes store64 creates

//	see if version has the key

Ver *findCursorVer(DbCursor *cursor, Handle *idxHndl, Txn *txn) {
	uint8_t buff[MAX_key + sizeof(INT_key) * 3];
	IndexKeyValue *key = (IndexKeyValue *)buff;
	int len, prefix, suffix;
	value_t val;
	ObjId docId;
	Ver *ver;

	prefix = store64(key->keyBytes, 0, idxHndl->map->arenaDef->id);
	memcpy (key->keyBytes + prefix, cursor->key, cursor->keyLen);

	suffix = ((uint8_t *)cursor->key)[cursor->keyLen - 1] & 0x7;
	suffix += ((uint8_t *)cursor->key)[cursor->keyLen - suffix - 1] & 0x7;

	get64 (cursor->key, cursor->keyLen - suffix, &docId.bits);
	*key->keyLen = cursor->keyLen - suffix + prefix;

	// first get the mvcc version for the document

	if ((ver = findDocVer(idxHndl->map->parent, docId, txn))) {
		JsVersion *version = (JsVersion *)(ver + 1);

		val.bits = vt_string;
		val.addr = key->keyLen; // cast to a string_t

		if (!lookup(js_addr(*version->keys), val, false, hashStr(key->keyBytes, *key->keyLen)))
			ver = NULL;
	}

	return ver;
}

//	move cursor

value_t fcnCursorMove(value_t *args, value_t *thisVal) {
	object_t *oval = js_addr(*thisVal);
	DbCursor *cursor;
	Ver *ver = NULL;
	Handle *idxHndl;
	value_t op, val;
	Txn *txn = NULL;
	DbHandle *hndl;

	hndl = (DbHandle *)oval->base->handle;
	val.bits = vt_status;

	op = conv2Int(args[0], false);

	if (!(idxHndl = bindHandle(hndl)))
		return val.status = DB_ERROR_handleclosed, val;

	cursor = (DbCursor *)(idxHndl + 1);

	if (cursor->txnId.bits) 
		txn = fetchIdSlot(idxHndl->map->db, cursor->txnId);

	do {
	  switch (op.nval) {
	  case OpLeft:
		val.status = dbLeftKey(cursor, idxHndl->map);
		break;
	  case OpRight:
		val.status = dbRightKey(cursor, idxHndl->map);
		break;
	  case OpNext:
		val.status = dbNextKey(cursor, idxHndl->map);
		break;
	  case OpPrev:
		val.status = dbPrevKey(cursor, idxHndl->map);
		break;
	  default:
		val.status = DB_ERROR_cursorop;
		break;
	  }
	  if (val.status != DB_OK)
		break;
	} while (!(ver = findCursorVer(cursor, idxHndl, txn)));

	if (ver) {
		Doc *doc = (Doc *)((uint8_t *)ver - ver->offset);
		document_t *document = js_alloc(sizeof(document_t), true);
		val.bits = vt_document;
		val.addr = document;
		val.refcount = true;

		*document->handle = hndl->hndlBits;
		*document->addr = doc->addr.bits;
		document->ver = ver;
	}

	releaseHandle(idxHndl, hndl);
	return val;
}

value_t fcnCursorPos(value_t *args, value_t *thisVal) {
	object_t *oval = js_addr(*thisVal);
	value_t op, val, key;
	DbCursor *cursor;
	Txn *txn = NULL;
	Ver *ver = NULL;
	Handle *idxHndl;
	DbHandle *hndl;
	string_t *str;

	hndl = (DbHandle *)oval->base->handle;
	val.bits = vt_status;

	if (!(idxHndl = bindHandle(hndl)))
		return val.status = DB_ERROR_handleclosed, val;

	cursor = (DbCursor *)(idxHndl + 1);

	if (cursor->txnId.bits) 
		txn = fetchIdSlot(idxHndl->map->db, cursor->txnId);

	op = conv2Int(args[0], false);
	key = conv2Str(args[1], false, false);
	str = js_addr(key);

	do val.status = dbFindKey(cursor, idxHndl->map, str->val, str->len, op.nval);
	while (!val.status && !(ver = findCursorVer(cursor, idxHndl, txn)));

	if (ver) {
		Doc *doc = (Doc *)((uint8_t *)ver - ver->offset);
		document_t *document = js_alloc(sizeof(document_t), true);
		val.bits = vt_document;
		val.addr = document;
		val.refcount = true;

		*document->handle = hndl->hndlBits;
		*document->addr = doc->addr.bits;
		document->ver = ver;
	}

	releaseHandle(idxHndl, hndl);
	return val;
}

value_t fcnCursorKeyAt(value_t *args, value_t *thisVal) {
	object_t *oval = js_addr(*thisVal);
	value_t op, s, key;
	uint32_t keyLen;
	DbHandle *hndl;
	char *keyStr;

	s.bits = vt_status;
	hndl = (DbHandle *)oval->base->handle;

	if ((s.status = keyAtCursor(hndl, &keyStr, &keyLen)))
		return s;

	return newString(keyStr, keyLen);
}

value_t fcnCursorDocAt(value_t *args, value_t *thisVal) {
	value_t s;

	s.bits = vt_status;
	return s;
}

PropFcn builtinCursorFcns[] = {
	{ fcnCursorPos, "pos" },
	{ fcnCursorMove, "move" },
	{ fcnCursorKeyAt, "keyAt" },
	{ fcnCursorDocAt, "docAt" },
	{ NULL, NULL}
};

PropVal builtinCursorProp[] = {
//	{ propIdxOnDisk, "onDisk" },
	{ NULL, NULL}
};


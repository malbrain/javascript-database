#include "jsdb.h"
#include "jsdb_db.h"

//  construct a db cursor
//  given the index
//  and a field array

value_t makeCursor(value_t val, DbMap *index, bool reverse, value_t start, value_t limit) {
	value_t s;

	switch (index->arena->type) {
	case Hndl_btreeIndex:
		return btreeCursor(val, index, reverse, start, limit);
		
	case Hndl_artIndex:
		return artCursor(val, index, reverse, start, limit);
	}

	fprintf(stderr, "Error: makeCursor => invalid index type => %d\n", index->arena->type);
	s.bits = vt_status;
	s.status = ERROR_script_internal;
	return s;
}

//	find correct document from snapshot and key value

bool cursorFindDoc(DbMap *index, DbCursor *cursor, KeySuffix *suffix) {
	RedBlack *entry = index->entry;
	uint8_t key[sizeof(ChildMap)];
	KeyVersion *keyVer;
	ChildMap *child;
	DbDoc *doc;

	child = (ChildMap *)(entry->key + entry->keyLen);

	cursor->docId.bits = get64(suffix->docId);
	cursor->keyVer = get64(suffix->keyVer);

	// find document version per timestamp

	if ((cursor->docAddr.bits = findDocVer(index->parent, cursor->docId, cursor->txnId, cursor->timestamp)))
		doc = getObj(index->parent, cursor->docAddr);
	else
		return false;

	//	see if key version was inserted for this document

	store64(key, child->id);
	if ((entry = rbFind (index->parent, doc->keyActv, key, sizeof(key), NULL)))
		keyVer = (KeyVersion *)(entry->key + entry->keyLen);
	else
		return false;

	if (keyVer->ver == cursor->keyVer)
		return true;

	return false;
}

value_t cursorNext(DbCursor *cursor, DbMap *index)
{
	value_t slot;
	DbDoc *doc;

	switch (index->arena->type) {
	case Hndl_artIndex: {
		ArtCursor *artCursor = (ArtCursor *)cursor;

		while (artNextKey(artCursor, index))
		  if (cursorFindDoc(index, cursor, artCursorSuffix(artCursor))) {
			doc = getObj(index->parent, cursor->docAddr);
			slot.bits = vt_document;
			slot.document = (document_t *)(doc + 1);
			return slot;
		  }

		slot.bits = vt_undef;
		return slot;
	}

	case Hndl_btreeIndex: {
		BtreeCursor *btreeCursor = (BtreeCursor *)cursor;

		while ((btreeNextKey(btreeCursor, index)))
		  if (cursorFindDoc(index, cursor, btreeCursorSuffix(btreeCursor))) {
			doc = getObj(index->parent, cursor->docAddr);
			slot.bits = vt_document;
			slot.document = (document_t *)(doc + 1);
			return slot;
		  }

		slot.bits = vt_undef;
		return slot;
	}
	}

	fprintf(stderr, "Error: nextKey => invalid index type: %d\n", index->arena->type);
	slot.bits = vt_status;
	slot.status = ERROR_script_internal;
	return slot;
}

value_t cursorPrev(DbCursor *cursor, DbMap *index)
{
	value_t slot;
	DbDoc *doc;

	switch (index->arena->type) {
	case Hndl_artIndex: {
		ArtCursor *artCursor = (ArtCursor *)cursor;

		while (artPrevKey(artCursor, index))
		  if (cursorFindDoc(index, cursor, artCursorSuffix(artCursor))) {
			doc = getObj(index->parent, cursor->docAddr);
			slot.bits = vt_document;
			slot.document = (document_t *)(doc + 1);
			return slot;
		  }

		slot.bits = vt_undef;
		return slot;
	}

	case Hndl_btreeIndex: {
		BtreeCursor *btreeCursor = (BtreeCursor *)cursor;

		while ((btreePrevKey(btreeCursor, index)))
		  if (cursorFindDoc(index, cursor, btreeCursorSuffix(btreeCursor))) {
			doc = getObj(index->parent, cursor->docAddr);
			slot.bits = vt_document;
			slot.document = (document_t *)(doc + 1);
			return slot;
		  }

		slot.bits = vt_undef;
		return slot;
	}
	}

	fprintf(stderr, "Error: prevKey => invalid index type: %d\n", index->arena->type);
	slot.bits = vt_status;
	slot.status = ERROR_script_internal;
	return slot;
}

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
		break;
		
	case Hndl_artIndex:
		return artCursor(val, index, reverse, start, limit);
		break;
	}

	fprintf(stderr, "Error: makeCursor => invalid index type => %d\n", index->arena->type);
	s.bits = vt_status;
	s.status = ERROR_script_internal;
	return s;
}

// TODO:  examine versions for key

DbDoc *cursorFindDoc(DbMap *index, DocId docId, value_t key) {
	DbAddr *docAddr = fetchIdSlot(index->parent, docId);
	return getObj(index->parent, *docAddr);
}

value_t cursorNext(DbCursor *cursor, DbMap *index)
{
	value_t slot;
	DbDoc *doc;

	switch (index->arena->type) {
	case Hndl_artIndex:
		while ((cursor->docId.bits = artNextKey((ArtCursor *)cursor, index)))
		  if ((doc = cursorFindDoc(index, cursor->docId, artCursorKey((ArtCursor *)cursor)))) {
			slot.bits = vt_document;
			slot.document = (document_t *)(doc + 1);
			return slot;
		  }

		slot.bits = vt_undef;
		return slot;

	  case Hndl_btreeIndex:
		while ((cursor->docId.bits = btreeNextKey((BtreeCursor *)cursor, index)))
		  if ((doc = cursorFindDoc(index, cursor->docId, btreeCursorKey((BtreeCursor *)cursor)))) {
			slot.bits = vt_document;
			slot.document = (document_t *)(doc + 1);
			return slot;
		  }

		slot.bits = vt_undef;
		return slot;
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
	case Hndl_artIndex:
		while ((cursor->docId.bits = artPrevKey((ArtCursor *)cursor, index)))
		  if ((doc = cursorFindDoc(index, cursor->docId, artCursorKey((ArtCursor *)cursor)))) {
			slot.bits = vt_document;
			slot.document = (document_t *)(doc + 1);
			return slot;
		  }

		slot.bits = vt_undef;
		return slot;

	  case Hndl_btreeIndex:
		while ((cursor->docId.bits = btreePrevKey((BtreeCursor *)cursor, index)))
		  if ((doc = cursorFindDoc(index, cursor->docId, btreeCursorKey((BtreeCursor *)cursor)))) {
			slot.bits = vt_document;
			slot.document = (document_t *)(doc + 1);
			return slot;
		  }

		slot.bits = vt_undef;
		return slot;
	}

	fprintf(stderr, "Error: prevKey => invalid index type: %d\n", index->arena->type);
	slot.bits = vt_status;
	slot.status = ERROR_script_internal;
	return slot;
}

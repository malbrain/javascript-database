#include "jsdb.h"
#include "jsdb_db.h"

//	TODO: lock record

value_t artCursor(value_t hndl, DbMap *index, bool reverse, value_t start, value_t limits) {
	uint32_t off = 0, size = 0, strtMax = 0, limMax = 0;
	uint8_t buff[MAX_key];
	value_t val, next, s;
	CursorStack *stack;
	ArtCursor *cursor;
	uint8_t *keys;
	IndexKey *key;
	DbAddr *base;
	int len;

	s.bits = vt_status;

	if (start.type == vt_array)
		strtMax = vec_count(start.aval->values);

	if (limits.type == vt_array)
		limMax = vec_count(limits.aval->values);

	val.bits = vt_handle;
	val.subType = Hndl_artCursor;
	val.handle = jsdb_alloc(sizeof(ArtCursor), true);
	val.refcount = 1;
	incrRefCnt(val);

	cursor = val.handle;
	cursor->hdr->hndl = hndl;
	cursor->hdr->pqAddr.bits = addPQEntry(index, getSet(index), en_reader);
	cursor->hdr->timestamp = getTimestamp(index, cursor->hdr->pqAddr);

	keys = getObj(index, indexAddr(index)->keys);
	base = artIndexAddr(index)->root;
	key = (IndexKey *)keys;

	//	seek for each given key field

	while (key->type != key_end) {
		if (cursor->keyFlds < strtMax)
			next = start.aval->values[cursor->keyFlds];
		else
			break;

		len = keyFld(next, key, buff, MAX_key);

		if (len < 0)
			return s.status = ERROR_keytoolong, s;

		base = artFindNxtFld(index, cursor, base, buff, len);

		if (base->type != FldEnd)
			break;

		off += sizeof(IndexKey) + key->len;
		key = (IndexKey *)(keys + off);
	}

	stack = &cursor->stack[cursor->depth++];
	stack->slot->bits = base->bits;
	stack->off = cursor->keySize;
	stack->addr = base;
	stack->ch = -1;

	key = (IndexKey *)keys;
	size = 0;
	off = 0;

	//	capture limiting key

	while (key->type != key_end) {
		if (cursor->keyFlds < limMax)
			next = limits.aval->values[cursor->limitFlds];
		else
			break;

		len = keyFld(next, key, cursor->limit + size, MAX_key - size);

		if (len < 0)
			return s.status = ERROR_keytoolong, s;

		cursor->limits[cursor->limitFlds].off = size;
		cursor->limits[cursor->limitFlds++].len = len;

		size += len;
		off += sizeof(IndexKey) + key->len;
		key = (IndexKey *)(keys + off);
	}

	return val;
}

value_t artCursorKey(ArtCursor *cursor) {
	value_t val;

	val.bits = vt_string;
	val.aux = cursor->keySize;
	val.str = cursor->key;
	return val;
}

/**
 * note: used by either 4 or 14 slot node
 * returns entry previous to 'ch'
 * algorithm: place each key byte into radix array, scan backwards
 *			  from 'ch' to preceeding entry.
 */
int slotrev4x14(int ch, uint8_t max, uint32_t alloc, volatile uint8_t* keys) {
	uint8_t radix[256];
	uint32_t slot;

	memset(radix, 0xff, sizeof(radix));

	for (slot = 0; slot < max; slot++) {
		if (alloc & (1 << slot))
			radix[keys[slot]] = slot;
	}

	while (--ch >= 0) {
		if (radix[ch] < 0xff)
			return radix[ch];
	}
	return -1;
}

int slot4x14(int ch, uint8_t max, uint32_t alloc, volatile uint8_t* keys) {
	uint8_t radix[256];
	uint32_t slot;

	memset(radix, 0xff, sizeof(radix));

	for (slot = 0; slot < max; slot++) {
		if (alloc & (1 << slot))
			radix[keys[slot]] = slot;
	}

	while (++ch < 256) {
		assert(ch >= 0);
		if (radix[ch] < 0xff)
			return radix[ch];
	}
	return 256;
}

int slotrev64(int ch, uint64_t alloc, volatile uint8_t* keys) {

	while (--ch >= 0) {
		if (keys[ch] < 0xff)
			if (alloc & (1ULL << keys[ch]))
				return ch;
	}
	return -1;
}

int slot64(int ch, uint64_t alloc, volatile uint8_t* keys) {

	while (++ch < 256) {
		assert(ch >= 0);
		if (keys[ch] < 0xff)
			if (alloc & (1ULL << keys[ch]))
				return ch;
	}
	return 256;
}

/**
 * retrieve next key from cursor
 * note:
 *	nextKey sets rightEOF when it cannot advance
 *	prevKey sets leftEOF when it cannot advance
 *
 */

//	compare key with limit stop

bool artLimitChk(ArtCursor *cursor) {
	int ret;

	for (int idx = 0; idx < cursor->limitFlds && idx < cursor->keyFlds; idx++) {
		uint8_t *limit = cursor->limit + cursor->limits[idx].off;
		uint8_t *fld = cursor->key + cursor->fields[idx].off;
		uint32_t limSize = cursor->limits[idx].len;
		uint32_t fldSize = cursor->fields[idx].len;

		if ((ret = memcmp(fld, limit, limSize > fldSize ? fldSize : limSize))) {
		  if (ret > 0)
			return true;
		  else
			return false;
		}

		if (limSize > fldSize)
			return false;

		if (limSize < fldSize)
			return true;
	}

	return false;
}

uint64_t artNextKey(ArtCursor *cursor, DbMap *index) {
  int slot, prev, spanMax;

  if (cursor->atRightEOF)
	return 0;

  while (cursor->depth < MAX_cursor) {
	CursorStack* stack = &cursor->stack[cursor->depth - 1];
	cursor->keySize = stack->off;

	spanMax = SPANLEN(stack->slot->type);

	switch (stack->slot->type < SpanNode ? stack->slot->type : SpanNode) {
		case Suffix:
			if (stack->ch < 0)
				cursor->suffix = cursor->keySize;

		case FldEnd: {
			ARTEnd *endNode = getObj(index, *stack->slot);

			//  continue into our suffix slot

			if (stack->ch < 0) {
			  if (stack->slot->type == FldEnd) {
				if (cursor->keyFlds)
				  prev = cursor->keySize - cursor->fields[cursor->keyFlds - 1].off;
				else
				  prev = 0;

				cursor->fields[cursor->keyFlds].off = prev;
				cursor->fields[cursor->keyFlds++].len = cursor->keySize - prev;

				if (artLimitChk (cursor)) // we reached the limit key value?
					return 0;
			  }

			  cursor->stack[cursor->depth].slot->bits = endNode->next->bits;
			  cursor->stack[cursor->depth].addr = endNode->next;
			  cursor->stack[cursor->depth].ch = -1;
			  cursor->stack[cursor->depth++].off = cursor->keySize;

			  stack->ch = 0;
			  continue;
			}

			if (stack->ch == 0) {
				cursor->stack[cursor->depth].slot->bits = endNode->pass->bits;
				cursor->stack[cursor->depth].addr = endNode->pass;
				cursor->stack[cursor->depth].ch = -1;
				cursor->stack[cursor->depth++].off = cursor->keySize;
				if (stack->slot->type == FldEnd)
					cursor->keyFlds--;
				stack->ch = 1;
				continue;
			}

			break;
		}

		case KeyEnd: {
			if (stack->ch < 0) {
				KeySuffix *suffix = (KeySuffix *)(cursor->key + cursor->suffix);

				stack->ch = 0;
				return get64(suffix->docId);
			}

			break;
		}

		case SpanNode: {
			ARTSpan* spanNode = getObj(index, *stack->slot);
			spanMax += stack->slot->nbyte + 1;

			if (spanNode->timestamp > cursor->hdr->timestamp)
				break;

			//  continue into our next slot

			if (stack->ch < 0) {
				memcpy(cursor->key + cursor->keySize, spanNode->bytes, spanMax);
				cursor->keySize += spanMax;
				cursor->stack[cursor->depth].slot->bits = spanNode->next->bits;
				cursor->stack[cursor->depth].addr = spanNode->next;
				cursor->stack[cursor->depth].ch = -1;
				cursor->stack[cursor->depth++].off = cursor->keySize;
				stack->ch = 0;
				continue;
			}

			break;
		}

		case Array4: {
			ARTNode4* radix4Node = getObj(index, *stack->slot);

			if (radix4Node->timestamp > cursor->hdr->timestamp)
				break;

			slot = slot4x14(stack->ch, 4, radix4Node->alloc, radix4Node->keys);
			if (slot >= 4)
				break;

			stack->ch = radix4Node->keys[slot];
			cursor->key[cursor->keySize++] = radix4Node->keys[slot];

			cursor->stack[cursor->depth].slot->bits = radix4Node->radix[slot].bits;
			cursor->stack[cursor->depth].addr = &radix4Node->radix[slot];
			cursor->stack[cursor->depth].off = cursor->keySize;
			cursor->stack[cursor->depth++].ch = -1;
			continue;
		}

		case Array14: {
			ARTNode14* radix14Node = getObj(index, *stack->slot);

			if (radix14Node->timestamp > cursor->hdr->timestamp)
				break;

			slot = slot4x14(stack->ch, 14, radix14Node->alloc, radix14Node->keys);
			if (slot >= 14)
				break;

			stack->ch = radix14Node->keys[slot];
			cursor->key[cursor->keySize++] = radix14Node->keys[slot];
			cursor->stack[cursor->depth].slot->bits = radix14Node->radix[slot].bits;
			cursor->stack[cursor->depth].addr = &radix14Node->radix[slot];
			cursor->stack[cursor->depth].ch = -1;
			cursor->stack[cursor->depth++].off = cursor->keySize;
			continue;
		}

		case Array64: {
			ARTNode64* radix64Node = getObj(index, *stack->slot);

			if (radix64Node->timestamp > cursor->hdr->timestamp)
				break;

			stack->ch = slot64(stack->ch, radix64Node->alloc, radix64Node->keys);
			if (stack->ch == 256)
				break;

			cursor->key[cursor->keySize++] = stack->ch;
			cursor->stack[cursor->depth].slot->bits = radix64Node->radix[radix64Node->keys[stack->ch]].bits;
			cursor->stack[cursor->depth].addr = &radix64Node->radix[radix64Node->keys[stack->ch]];
			cursor->stack[cursor->depth].ch = -1;
			cursor->stack[cursor->depth++].off = cursor->keySize;
			continue;
		}

		case Array256: {
			ARTNode256* radix256Node = getObj(index, *stack->slot);

			if (radix256Node->timestamp > cursor->hdr->timestamp)
				break;

			while (stack->ch < 256) {
				uint32_t idx = ++stack->ch;
				if (idx < 256 && radix256Node->alloc[idx / 64] & (1ULL << (idx % 64)))
					break;
			}

			if (stack->ch == 256)
				break;

			cursor->key[cursor->keySize++] = stack->ch;
			cursor->stack[cursor->depth].slot->bits = radix256Node->radix[stack->ch].bits;
			cursor->stack[cursor->depth].addr = &radix256Node->radix[stack->ch];
			cursor->stack[cursor->depth].ch = -1;
			cursor->stack[cursor->depth++].off = cursor->keySize;
			continue;
		}
	}  // end switch

	if (--cursor->depth) {
		stack = &cursor->stack[cursor->depth];
		cursor->keySize = stack->off;
		continue;
	}

	cursor->atRightEOF = true;
	break;
  }  // end while
  return 0;
}

/**
 * retrieve previous key from the cursor
 */

uint64_t artPrevKey(ArtCursor *cursor, DbMap *index) {
	int slot, spanMax;

	if (cursor->atLeftEOF)
		return 0;

	if (!cursor->depth || cursor->atRightEOF) {
		CursorStack* stack = &cursor->stack[cursor->depth++];
		cursor->atRightEOF = false;
		cursor->depth = 0;
		stack->slot->bits = artIndexAddr(index)->root->bits;
		stack->addr = artIndexAddr(index)->root;
		stack->off = 0;
		stack->ch = 256;
	}

	while (cursor->depth) {
		CursorStack* stack = &cursor->stack[cursor->depth - 1];
		cursor->keySize = stack->off;

		spanMax = SPANLEN(stack->slot->type);

		switch (stack->slot->type < SpanNode ? stack->slot->type : SpanNode) {
			case UnusedSlot: {
				break;
			}

			case Suffix:
				if (stack->ch == 0)
					cursor->suffix = cursor->keySize;

			case FldEnd: {
				ARTEnd *endNode = getObj(index, *stack->slot);

				//  continue into our suffix slot

				if (stack->ch > 255) {
					cursor->stack[cursor->depth].slot->bits = endNode->pass->bits;
					cursor->stack[cursor->depth].ch = 256;
					cursor->stack[cursor->depth].addr = endNode->pass;
					cursor->stack[cursor->depth++].off = cursor->keySize;
					stack->ch = 0;
					continue;
				}

				if (stack->ch == 0) {
					cursor->stack[cursor->depth].slot->bits = endNode->next->bits;
					cursor->stack[cursor->depth].ch = 256;
					cursor->stack[cursor->depth].addr = endNode->next;
					cursor->stack[cursor->depth++].off = cursor->keySize;
					stack->ch = -1;
					continue;
				}

				break;
			}

			case KeyEnd: {
				if (stack->ch == 0) {
					KeySuffix *suffix = (KeySuffix *)(cursor->key + cursor->suffix);

					stack->ch = -1;
					return get64(suffix->docId);
				}

				break;
			}

			case SpanNode: {
				ARTSpan* spanNode = getObj(index, *stack->slot);
				spanMax += stack->slot->nbyte + 1;

				if (spanNode->timestamp > cursor->hdr->timestamp)
					break;

				// examine next node under slot

				if (stack->ch > 255) {
					memcpy(cursor->key + cursor->keySize, spanNode->bytes, spanMax);
					cursor->keySize += spanMax;
					cursor->stack[cursor->depth].slot->bits = spanNode->next->bits;
					cursor->stack[cursor->depth].addr = spanNode->next;
					cursor->stack[cursor->depth].ch = 256;
					cursor->stack[cursor->depth++].off = cursor->keySize;
					stack->ch = 0;
					continue;
				}
				break;
			}

			case Array4: {
				ARTNode4* radix4Node = getObj(index, *stack->slot);

				if (radix4Node->timestamp > cursor->hdr->timestamp)
					break;

				slot = slotrev4x14(stack->ch, 4, radix4Node->alloc, radix4Node->keys);
				if (slot < 0)
					break;

				stack->ch = radix4Node->keys[slot];
				cursor->key[cursor->keySize++] = stack->ch;

				cursor->stack[cursor->depth].slot->bits = radix4Node->radix[slot].bits;
				cursor->stack[cursor->depth].addr = &radix4Node->radix[slot];
				cursor->stack[cursor->depth].off = cursor->keySize;
				cursor->stack[cursor->depth++].ch = 256;
				continue;
			}

			case Array14: {
				ARTNode14* radix14Node = getObj(index, *stack->slot);

				if (radix14Node->timestamp > cursor->hdr->timestamp)
					break;

				slot = slotrev4x14(stack->ch, 14, radix14Node->alloc, radix14Node->keys);
				if (slot < 0)
					break;

				stack->ch = radix14Node->keys[slot];
				cursor->key[cursor->keySize++] = stack->ch;

				cursor->stack[cursor->depth].slot->bits = radix14Node->radix[slot].bits;
				cursor->stack[cursor->depth].addr = &radix14Node->radix[slot];
				cursor->stack[cursor->depth].off = cursor->keySize;
				cursor->stack[cursor->depth++].ch = 256;
				continue;
			}

			case Array64: {
				ARTNode64* radix64Node = getObj(index, *stack->slot);

				if (radix64Node->timestamp > cursor->hdr->timestamp)
					break;

				stack->ch = slotrev64(stack->ch, radix64Node->alloc, radix64Node->keys);
				if (stack->ch < 0)
					break;

				slot = radix64Node->keys[stack->ch];
				cursor->key[cursor->keySize++] = stack->ch;

				cursor->stack[cursor->depth].slot->bits = radix64Node->radix[slot].bits;
				cursor->stack[cursor->depth].addr = &radix64Node->radix[slot];
				cursor->stack[cursor->depth].off = cursor->keySize;
				cursor->stack[cursor->depth++].ch = 256;
				continue;
			}

			case Array256: {
				ARTNode256* radix256Node = getObj(index, *stack->slot);

				if (radix256Node->timestamp > cursor->hdr->timestamp)
					break;

				while (--stack->ch >= 0) {
					uint32_t idx = stack->ch;
					if (radix256Node->alloc[idx / 64] & (1ULL << (idx % 64)))
						break;
				}

				if (stack->ch < 0)
					break;

				slot = stack->ch;
				cursor->key[cursor->keySize++] = stack->ch;

				cursor->stack[cursor->depth].slot->bits = radix256Node->radix[slot].bits;
				cursor->stack[cursor->depth].addr = &radix256Node->radix[slot];
				cursor->stack[cursor->depth].off = cursor->keySize;
				cursor->stack[cursor->depth++].ch = 256;
				continue;
			}
		}  // end switch

		if (--cursor->depth) {
			cursor->keySize = stack[-1].off;
			continue;
		}

		break;
	}  // end while

	cursor->atLeftEOF = true;
	return 0;
}

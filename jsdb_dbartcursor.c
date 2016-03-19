#include "jsdb.h"
#include "jsdb_db.h"

value_t artCursor(DbMap *index, bool direction) {
	ArtCursor *cursor;
	value_t val;

	val.bits = vt_handle;
	val.aux = hndl_artCursor;
	val.h = jsdb_alloc(sizeof(ArtCursor), true);

	cursor = val.h;
	cursor->index = index;
	cursor->direction = direction;
	cursor->timestamp = allocateTimestamp(index, en_reader);
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
 *              from 'ch' to preceeding entry.
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
 *   nextKey sets rightEOF when it cannot advance
 *   prevKey sets leftEOF when it cannot advance
 *
 */

bool artNextKey(ArtCursor *cursor) {
    int slot;

    if (cursor->atRightEOF)
        return false;

    if (!cursor->depth || cursor->atLeftEOF) {
        cursor->depth = 0;
        cursor->atLeftEOF = false;
        CursorStack* stack = &cursor->stack[cursor->depth++];
        stack->slot->bits = artIndexAddr(cursor->index)->root->bits;
        stack->addr = artIndexAddr(cursor->index)->root;
        stack->off = 0;
        stack->ch = -1;
    }

    while (cursor->depth < MAX_cursor) {
        CursorStack* stack = &cursor->stack[cursor->depth - 1];
        cursor->keySize = stack->off;

        switch (stack->slot->type) {
            case KeyEnd: {
                ARTKeyEnd *endNode = getObj(cursor->index, *stack->slot);

                if (stack->ch < 0) {
                    stack->ch = 0;
                    return true;
                }

                if (stack->ch == 0) {
                    cursor->stack[cursor->depth].slot->bits = endNode->next->bits;
                    cursor->stack[cursor->depth].addr = endNode->next;
                    cursor->stack[cursor->depth].ch = -1;
                    cursor->stack[cursor->depth++].off = cursor->keySize;
                    stack->ch = 1;
                    continue;
                }

                break;
            }

            case SpanNode: {
                ARTSpan* spanNode = getObj(cursor->index, *stack->slot);
                uint32_t max = stack->slot->nbyte;

                if (spanNode->timestamp > cursor->timestamp)
                    break;

                //  continue into our next slot

                if (stack->ch < 0) {
                    memcpy(cursor->key + cursor->keySize, spanNode->bytes, max);
                    cursor->keySize += max;
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
                ARTNode4* radix4Node = getObj(cursor->index, *stack->slot);

                if (radix4Node->timestamp > cursor->timestamp)
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
                ARTNode14* radix14Node = getObj(cursor->index, *stack->slot);

                if (radix14Node->timestamp > cursor->timestamp)
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
                ARTNode64* radix64Node = getObj(cursor->index, *stack->slot);

                if (radix64Node->timestamp > cursor->timestamp)
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
                ARTNode256* radix256Node = getObj(cursor->index, *stack->slot);

                if (radix256Node->timestamp > cursor->timestamp)
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
    return false;
}

/**
 * retrieve previous key from the cursor
 */

bool artPrevKey(ArtCursor *cursor) {
    int slot;

    if (cursor->atLeftEOF)
        return false;

    if (!cursor->depth || cursor->atRightEOF) {
        CursorStack* stack = &cursor->stack[cursor->depth++];
        cursor->atRightEOF = false;
        cursor->depth = 0;
        stack->slot->bits = artIndexAddr(cursor->index)->root->bits;
        stack->addr = artIndexAddr(cursor->index)->root;
        stack->off = 0;
        stack->ch = 256;
    }

    while (cursor->depth) {
        CursorStack* stack = &cursor->stack[cursor->depth - 1];
        cursor->keySize = stack->off;

        switch (stack->slot->type) {
            case UnusedSlot: {
                break;
            }

            case KeyEnd: {
                ARTKeyEnd *endNode = getObj(cursor->index, *stack->slot);

                if (stack->ch > 255) {
                    cursor->stack[cursor->depth].slot->bits = endNode->next->bits;
                    cursor->stack[cursor->depth].addr = endNode->next;
                    cursor->stack[cursor->depth].ch = -1;
                    cursor->stack[cursor->depth++].off = cursor->keySize;
                    stack->ch = 0;
                    continue;
                }

                if (stack->ch == 0) {
                    stack->ch = -1;
                    return true;
                }

                break;
            }

            case SpanNode: {
                ARTSpan* spanNode = getObj(cursor->index, *stack->slot);
                uint32_t max = stack->slot->nbyte;

                if (spanNode->timestamp > cursor->timestamp)
                    break;

                // examine next node under slot

                if (stack->ch > 255) {
                    memcpy(cursor->key + cursor->keySize, spanNode->bytes, max);
                    cursor->keySize += max;
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
                ARTNode4* radix4Node = getObj(cursor->index, *stack->slot);

                if (radix4Node->timestamp > cursor->timestamp)
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
                ARTNode14* radix14Node = getObj(cursor->index, *stack->slot);

                if (radix14Node->timestamp > cursor->timestamp)
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
                ARTNode64* radix64Node = getObj(cursor->index, *stack->slot);

                if (radix64Node->timestamp > cursor->timestamp)
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
                ARTNode256* radix256Node = getObj(cursor->index, *stack->slot);

                if (radix256Node->timestamp > cursor->timestamp)
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
    return false;
}

bool artSeekKey(ArtCursor *cursor, uint8_t *key, uint32_t keylen) {
	DbAddr *slot = artFindKey (cursor->index, cursor, key, keylen);

	if (!slot || slot->type != KeyEnd)
		return false;

	return true;
}

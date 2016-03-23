#include "jsdb.h"
#include "jsdb_db.h"

DbAddr *artFindKey( DbMap *index, ArtCursor *cursor, uint8_t *key, uint32_t keylen) {

	CursorStack* stack = NULL;
	DbAddr newSlot[1], *slot;
	uint32_t idx, offset = 0;
	uint32_t startDepth = 0;
	uint32_t startSize = 0;
	bool restart = true;
	bool pass = false;

	slot = artIndexAddr(index)->root;

	if (cursor) {
		startSize = cursor->keySize;
		startDepth = cursor->depth;
		cursor->atLeftEOF = false;
	}

	do {
		restart = false;

		if (cursor) {
			cursor->keySize = startSize;
			cursor->depth = startDepth;
		}

		//  if we are waiting on a dead bit to clear

		if (pass)
			art_yield();
		else
			pass = true;

		// loop through all the key bytes
		stack = NULL;

		while (offset < keylen) {
			if (slot->dead) {
				restart = true;
				break;
			}

			if (cursor) {
				stack = &cursor->stack[cursor->depth++];
				stack->slot->bits = slot->bits;
				stack->addr = slot;
				stack->off = cursor->keySize;
				stack->ch = -1;
			}

			newSlot->bits = slot->bits;

			switch (slot->type) {
				case KeySuffix: {
					ARTSuffix *suffixNode = getObj(index, *slot);
					slot = suffixNode->next;
					continue;
				}

				case KeyEnd: {
					break;
				}

				case Array4: {
					ARTNode4 *node = getObj(index, *slot);

					// simple loop comparing bytes

					for (idx = 0; idx < 4; idx++) {
						if (node->alloc & (1 << idx))
							if (key[offset] == node->keys[idx])
								break;
					}

					if (idx < 4) {
						slot = node->radix + idx;  // slot points to child node
						offset++;				// update key byte offset

						if (cursor)		  // update cursor key
							cursor->key[cursor->keySize++] = node->keys[idx];

						continue;
					}

					// key byte not found

					if (cursor) {
						idx = slot4x14(stack->ch, 4, node->alloc, node->keys);

						if (idx < 4)
							stack->ch = node->keys[idx];
						else
							stack->ch = 256;
					}
					break;
				}

				case Array14: {
					ARTNode14 *node = getObj(index, *slot);

					// simple loop comparing bytes

					for (idx = 0; idx < 14; idx++) {
						if (node->alloc & (1 << idx))
							if (key[offset] == node->keys[idx])
								break;
					}

					if (idx < 14) {
						slot = node->radix + idx;  // slot points to child node
						offset++;				// update key byte offset

						if (cursor)		  // update cursor key
							cursor->key[cursor->keySize++] = node->keys[idx];

						continue;
					}

					// key byte not found

					if (cursor) {
						idx = slot4x14(stack->ch, 14, node->alloc, node->keys);

						if (idx < 14)
							stack->ch = node->keys[idx];
						else
							stack->ch = 256;
					}
					break;
				}

				case Array64: {
					ARTNode64* node = getObj(index, *slot);
					idx = node->keys[key[offset]];

					if (idx < 0xff && (node->alloc & (1ULL << idx))) {
						slot = node->radix + idx;  // slot points to child node
						if (cursor)		  // update cursor key
							cursor->key[cursor->keySize++] = key[offset];
						offset++;  // update key offset
						continue;
					}

					// key byte not found

					if (cursor) {
						idx = slot64(stack->ch, node->alloc, node->keys);

						if (idx < 256)
							stack->ch = idx;
						else
							stack->ch = 256;
					}
					break;
				}

				case Array256: {
					ARTNode256* node = getObj(index, *slot);
					idx = key[offset];

					if (node->alloc[idx / 64] & (1ULL << (idx % 64))) {
						if (cursor)  // update cursor key
							cursor->key[cursor->keySize++] = idx;

						slot = node->radix + idx;  // slot points to child node
						offset++;			// update key byte offset
						continue;
					}

					// key byte not found

					if (cursor) {
						while (stack->ch < 256) {
							idx = ++stack->ch;
							if (idx < 256 && node->alloc[idx / 64] & (1ULL << (idx % 64)))
								break;
						}
					}
					break;
				}

				case UnusedSlot: {
					if (cursor) {
						cursor->depth--;
						if (!cursor->depth)
							cursor->atRightEOF = true;
					}
					break;
				}
				default: {
					// unreachable
				}

			}  // end switch

			return slot;

		}  // end while (offset < keylen)

	} while (restart);

	if (cursor) {
		stack = &cursor->stack[cursor->depth++];
		stack->slot->bits = slot->bits;
		stack->off = cursor->keySize;
		stack->addr = slot;
		stack->ch = -1;
	}

	return slot;
}



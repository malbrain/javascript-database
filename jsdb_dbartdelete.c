#include "jsdb.h"
#include "jsdb_db.h"

typedef enum {
    ContinueSearch,
    EndSearch,
    RetrySearch,
    RestartSearch,
    ErrorSearch
} ReturnState;

static bool addNodeToWaitList(DbMap *index, uint32_t set, DbAddr *newSlot) {
    DbAddr *head = artIndexAddr(index)->freeLists[set][newSlot->type].head;
    DbAddr *tail = artIndexAddr(index)->freeLists[set][newSlot->type].tail;
    return addNodeToFrame(index, head, tail, *newSlot);
}

Status artDeleteKey(DbMap *index, uint32_t set, uint8_t *key, uint32_t baseLen) {
	DbAddr *slot, *prev, newSlot[1];
	ArtCursor cursor[1];
	CursorStack *stack;
	ReturnState rt;
	uint32_t bit;
	uint8_t ch;

	memset(cursor, 0, sizeof(ArtCursor));
	slot = artFindKey(index, cursor, key, baseLen);

	//	was key not found in the trie?

	if (!slot || slot->type != KeyEnd)
		return ERROR_keynotfound;

	stack = &cursor->stack[cursor->depth - 1];
	stack->dir = true;

	//	now that we have the trie nodes in the cursor stack
	//	we can go through them backwards to remove empties.

	while (cursor->depth) {
		CursorStack *stack = &cursor->stack[--cursor->depth];
		uint32_t pass = 0;
		bool retry = true;

		prev = slot;
		slot = stack->addr;
		ch = key[stack->off];

		//	wait if we run into a dead slot
		do {
			if (pass)
				art_yield();
			else
				pass = 1;

			// obtain write lock on the node
			lockLatch(slot->latch);
			newSlot->bits = stack->addr->bits;

			if ((retry = newSlot->dead))
				unlockLatch(slot->latch);

		} while (retry);

		switch (newSlot->type) {
			case UnusedSlot: {
				rt = EndSearch;
				break;
			}
			case KeyEnd: {
        		rt = EndSearch;
				break;
			}

			case SpanNode: {
    			kill_slot(slot->latch);

    			if (!addNodeToWaitList(index, set, newSlot))
        			rt = ErrorSearch;
				else
    				rt = ContinueSearch;

				break;
			}
			case Array4: {
				ARTNode4 *node = getObj(index, *stack->addr);

				for (bit = 0; bit < 4; bit++) {
					if (node->alloc & (1 << bit))
						if (ch == node->keys[bit])
							break;
				}

				if (bit == 4) {
					rt = EndSearch;  // key byte not found
					break;
				}

				// we are not the last entry in the node?

				node->alloc &= ~(1 << bit);

				if (node->alloc) {
					rt = EndSearch;
					break;
				}

				kill_slot(slot->latch);

    			if (!addNodeToWaitList(index, set, newSlot)) {
					rt = ErrorSearch;
					break;
				}

				rt = ContinueSearch;
				break;
			}
			case Array14: {
				ARTNode14 *node = getObj(index, *stack->addr);

				for (bit = 0; bit < 14; bit++) {
					if (node->alloc & (1 << bit))
						if (ch == node->keys[bit])
							break;
				}

				if (bit == 14) {
					rt = EndSearch;  // key byte not found
					break;
				}

				// we are not the last entry in the node?

				node->alloc &= ~(1 << bit);

				if (node->alloc) {
					rt = EndSearch;
					break;
				}

				kill_slot(slot->latch);

    			if (!addNodeToWaitList(index, set, newSlot))
					rt = ErrorSearch;
				else
					rt = ContinueSearch;

				break;
			}

			case Array64: {
				ARTNode64 *node = getObj(index, *stack->addr);
    			bit = node->keys[ch];

    			if (bit == 0xff) {
        			rt = EndSearch;
					break;
				}

    			node->keys[ch] = 0xff;
    			node->alloc &= ~(1ULL << bit);

    			if (node->alloc) {
        			rt = EndSearch;
					break;
				}

    			kill_slot(slot->latch);

    			if (!addNodeToWaitList(index, set, newSlot))
        			rt = ErrorSearch;
				else
    				rt = ContinueSearch;

				break;
			}

			case Array256: {
				ARTNode256 *node = getObj(index, *stack->addr);
    			bit = ch;

    			if (~node->alloc[bit / 64] & (1ULL << (bit % 64)))
        			return EndSearch;

    			node->alloc[bit / 64] &= ~(1ULL << (bit % 64));

    			if (node->alloc[0] | node->alloc[1] | node->alloc[2] | node->alloc[3])
        			return EndSearch;

    			kill_slot(slot->latch);

    			if (!addNodeToWaitList(index, set, newSlot))
        			rt = ErrorSearch;
				else
    				rt = ContinueSearch;

				break;
			}
		}	// end switch

		unlockLatch(stack->addr->latch);

		if (rt == ContinueSearch)
			continue;

		break;

	}	// end while

	//	zero out root?

	if (!cursor->depth && rt == ContinueSearch)
		slot->bits = 0;

	atomicAdd64(artIndexAddr(index)->numEntries, -1);
	return OK;
}

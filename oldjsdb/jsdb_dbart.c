#include "jsdb.h"
#include "jsdb_db.h"

bool addSlotToWaitList(DbMap *index, uint32_t set, DbAddr slot) {
	DbAddr *head = artIndexAddr(index)->freeLists[set][slot.type].head;
	DbAddr *tail = artIndexAddr(index)->freeLists[set][slot.type].tail;

	if (addSlotToFrame(index, head, slot.bits)) {
	  if (head->addr && !tail->addr)
		tail->addr = head->addr;
	} else
		return false;

	return true;
}


#include <errno.h>

#ifndef __APPLE__
#include <malloc.h>
#endif

#include "js.h"
#include "js_malloc.h"
#include "database/db.h"
#include "database/db_arena.h"
#include "database/db_map.h"

extern bool mallocDebug;

//  allocate reference counted object

void *js_alloc(uint32_t size, bool zeroit) {
rawobj_t *mem;
DbAddr addr;

	addr.bits = db_rawAlloc(size + sizeof(rawobj_t), mallocDebug ? false : zeroit);

	if (mallocDebug) {
		mem = db_memObj(addr);

		if (mem->bits && mem->bits != 0xdeadbeef) {
			fprintf (stderr, "js_alloc: memory address already in use\n");
			exit(0);
		}

		// now that we've checked it, we can zero it

		if (zeroit)
			memset(mem + 1, 0, size);
	} else {
		mem = db_memObj(addr);
	}

	mem->weakCnt[0] = 0;
	mem->refCnt[0] = 0;
	mem->bits = addr.bits;
	return mem + 1;
}

//	free javascript object

void js_free(void *obj) {
rawobj_t *mem = obj;
DbAddr addr;

	addr.bits = mem[-1].bits;

	if (mallocDebug) {
		mem[-1].bits = 0xdeadbeef;

		if (addr.bits == 0xdeadbeef) {
			fprintf (stderr, "js_free: duplicate free!\n");
			exit(0);
		} else
			db_memFree(addr);
	} else
		db_memFree (addr);
}

uint32_t js_size (void *obj) {
rawobj_t *raw = obj;
DbAddr addr;

	addr.bits = raw[-1].bits;

	if (mallocDebug)
	  if (addr.bits == 0xdeadbeef)
		fprintf (stderr, "js_size: memory already free!\n");

	return db_rawSize(addr) - sizeof(rawobj_t);
}

void *js_realloc(void *old, uint32_t *size, bool zeroit) {
rawobj_t *raw = old, *mem;
uint32_t oldSize;
DbAddr addr;

	addr.bits = raw[-1].addr.bits;

//  is the new size within the same power of two?

	if (mallocDebug)
	  if (addr.bits == 0xdeadbeef)
		fprintf (stderr, "js_realloc: memory already free!\n");

	// calc user's size

	oldSize = db_rawSize(addr) - sizeof(rawobj_t);

	//	see if it is still witin the allocation

	if (oldSize >= *size) {
		*size = oldSize;
		return old;
	}

	if ((mem = js_alloc(*size, zeroit)))
		*size = db_rawSize(mem[-1].addr) - sizeof(rawobj_t);
	else {
		fprintf (stderr, "js_realloc: out of memory!\n");
		exit(1);
	}

	//  copy contents and release old allocation

	memcpy(mem, raw, oldSize);

	if (zeroit)
		memset((char *)mem + oldSize, 0, *size - oldSize);

	// copy reference counts

	*mem[-1].weakCnt = *raw[-1].weakCnt;
	*mem[-1].refCnt = *raw[-1].refCnt;

	addr.bits = raw[-1].bits;

	if(mallocDebug) {
	  if(addr.bits == 0xdeadbeef) {
		fprintf (stderr, "js_realloc: already freed!\n");
		exit(1);
	  }
	  raw[-1].bits = 0xdeadbeef;
	}

	db_memFree (addr);
	return mem;
}


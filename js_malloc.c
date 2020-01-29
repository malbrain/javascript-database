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

	mem = db_malloc(size + sizeof(rawobj_t), mallocDebug ? false : zeroit);

	if (mallocDebug) {
          addr = db_memAddr(mem);

          if (addr.bits && addr.bits != 0xdeadbeef) {
            fprintf(stderr, "js_alloc: memory address already in use\n");
            exit(0);
          }
    }

	mem->weakCnt[0] = 0;
	mem->refCnt[0] = 0;
	return mem + 1;
}

//	free javascript object

void js_free(void *obj) {
rawobj_t *mem = obj;

	db_free(mem - 1);
}

uint32_t js_size (void *obj) {
rawobj_t *raw = obj;

	return db_memSize(raw - 1) - sizeof(rawobj_t);
}

void *js_realloc(void *old, uint32_t *size, bool zeroit) {
rawobj_t *raw = old, *mem;
uint32_t oldSize;
DbAddr addr;

	addr = db_memAddr(raw - 1);

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
		*size = db_memSize(mem) - sizeof(rawobj_t);
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

	db_memFree (addr);
	return mem;
}


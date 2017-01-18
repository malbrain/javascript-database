#include <errno.h>

#ifndef __APPLE__
#include <malloc.h>
#endif

#include "js.h"
#include "js_malloc.h"
#include "database/db_map.h"
#include "database/db_malloc.h"

extern bool mallocDebug;

//  allocate reference counted object

void *js_alloc(uint32_t size, bool zeroit) {
rawobj_t *mem;
uint64_t bits;

	if (mallocDebug) {
		bits = db_rawAlloc(size + sizeof(rawobj_t), false);
		mem = db_memObj(bits);

		if (*mem->addr && *mem->addr != 0xdeadbeef) {
			fprintf (stderr, "js_alloc: memory address already in use\n");
			exit(0);
		}

		// now that we've checked it, we can zero it

		if (zeroit)
			memset(mem + 1, 0, size);
	} else {
		bits = db_rawAlloc(size + sizeof(rawobj_t), zeroit);
		mem = db_memObj(bits);
	}

	mem->weakCnt[0] = 0;
	mem->refCnt[0] = 0;
	*mem->addr = bits;
	return mem + 1;
}

//	free javascript object

void js_free(void *obj) {
rawobj_t *mem = obj;

	if (mallocDebug) {
		uint64_t bits = *mem[-1].addr;
		*mem[-1].addr = 0xdeadbeef;

		if (bits == 0xdeadbeef) {
			fprintf (stderr, "js_free: duplicate free!\n");
			exit(0);
		} else
			db_memFree(bits);
	} else
		db_memFree (*mem[-1].addr);
}

uint32_t js_size (void *obj) {
rawobj_t *raw = obj;

	if (mallocDebug)
	  if (*raw[-1].addr == 0xdeadbeef)
		fprintf (stderr, "js_size: memory already free!\n");

	return db_rawSize(*raw[-1].addr) - sizeof(rawobj_t);
}

void *js_realloc(void *old, uint32_t *size, bool zeroit) {
rawobj_t *raw = old, *mem;
uint32_t oldSize;
uint64_t bits;

	//  is the new size within the same power of two?

	if (mallocDebug)
	  if (*raw[-1].addr == 0xdeadbeef)
		fprintf (stderr, "js_realloc: memory already free!\n");

	// calc user's size

	oldSize = db_rawSize(*raw[-1].addr) - sizeof(rawobj_t);

	//	see if it is still witin the allocation

	if (oldSize >= *size) {
		*size = oldSize;
		return old;
	}

	if ((mem = js_alloc(*size, zeroit)))
		*size = db_rawSize(*mem[-1].addr) - sizeof(rawobj_t);
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

	bits = *raw[-1].addr;

	if(mallocDebug) {
	  if(bits == 0xdeadbeef) {
		fprintf (stderr, "js_realloc: already freed!\n");
		exit(1);
	  }
	  *raw[-1].addr = 0xdeadbeef;
	}

	db_memFree (bits);
	return mem;
}


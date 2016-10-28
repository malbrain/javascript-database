#include <errno.h>

#ifndef __APPLE__
#include <malloc.h>
#endif

#include "js.h"
#include "js_malloc.h"
#include "database/db_malloc.h"

//	allocate javascript object

void *js_alloc(uint32_t len, bool zeroit) {
rawobj_t *mem;
uint64_t bits;

	bits = db_rawAlloc(len + sizeof(rawobj_t), zeroit);

	mem = db_memObj(bits);
	return mem + 1;
}

//	free javascript object

void js_free(void *obj) {
rawobj_t *mem = obj;

	db_memFree (mem[-1].addr);
}

//  allocate reference counted object

uint64_t js_rawAlloc(uint32_t len, bool zeroit) {
rawobj_t *mem;
uint64_t bits;

	bits = db_rawAlloc(len + sizeof(rawobj_t), zeroit);

	mem = db_memObj(bits);
	mem->weakCnt[0] = 0;
	mem->refCnt[0] = 0;
	mem->addr = bits;
	return bits;
}

uint32_t js_size (void *obj) {
rawobj_t *raw = obj;

	return db_rawSize(raw[-1].addr) - sizeof(rawobj_t);
}

void *js_realloc(void *old, uint32_t size, bool zeroit) {
uint32_t amt = size + sizeof(rawobj_t);
rawobj_t *raw = old, *mem;
uint32_t oldSize, newSize;
uint64_t bits;

	//  is the new size within the same power of two?

	oldSize = db_rawSize(raw[-1].addr);

	if (oldSize >= amt)
		return old;

	if ((bits = js_rawAlloc(amt, zeroit)))
		mem = db_memObj(bits);
	else {
		fprintf (stderr, "js_realloc: out of memory!\n");
		exit(1);
	}

	//  copy contents and release old allocation

	newSize = db_rawSize(bits);

	memcpy(mem, raw - 1, oldSize);

	if (zeroit)
		memset((char *)mem + oldSize, 0, newSize - oldSize);

	db_memFree (raw[-1].addr);
	mem->addr = bits;
	return mem + 1;
}


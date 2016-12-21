#include <errno.h>

#ifndef __APPLE__
#include <malloc.h>
#endif

#include "js.h"
#include "js_malloc.h"
#include "database/db_malloc.h"

//  allocate reference counted object

uint64_t js_rawAlloc(uint32_t size, bool zeroit) {
rawobj_t *mem;
uint64_t bits;

	if (debug) {
		bits = db_rawAlloc(size + sizeof(rawobj_t), false);
		mem = db_memObj(bits);

		if (mem->addr && mem->addr != 0xdeadbeef) {
			fprintf (stderr, "js_rawAlloc: duplicate memory address\n");
			exit(0);
		}

		if (zeroit)
			memset(mem + 1, 0, size);
	} else {
		bits = db_rawAlloc(size + sizeof(rawobj_t), zeroit);
		mem = db_memObj(bits);
	}

	mem->weakCnt[0] = 0;
	mem->refCnt[0] = 0;
	mem->addr = bits;
	return bits;
}

//	allocate javascript object

void *js_alloc(uint32_t size, bool zeroit) {
rawobj_t *mem;
uint64_t bits;

	bits = js_rawAlloc(size, zeroit);

	mem = db_memObj(bits);
	return mem + 1;
}

//	free javascript object

void js_free(void *obj) {
rawobj_t *mem = obj;

	if (debug) {
		uint64_t bits = mem[-1].addr;
		mem[-1].addr = 0xdeadbeef;

		if (bits == 0xdeadbeef) {
			fprintf (stderr, "js_free: duplicate free!\n");
			exit(0);
		} else
			db_memFree(bits);
	} else
		db_memFree (mem[-1].addr);
}

uint32_t js_size (void *obj) {
rawobj_t *raw = obj;

	if (debug)
	  if (raw[-1].addr == 0xdeadbeef)
		fprintf (stderr, "js_size: memory already free!\n");

	return db_rawSize(raw[-1].addr) - sizeof(rawobj_t);
}

void *js_realloc(void *old, uint32_t size, bool zeroit) {
uint32_t amt = size + sizeof(rawobj_t);
rawobj_t *raw = old, *mem;
uint32_t oldSize, newSize;
uint64_t bits;

	//  is the new size within the same power of two?

	if (debug)
	  if (raw[-1].addr == 0xdeadbeef)
		fprintf (stderr, "js_realloc: memory already free!\n");

	oldSize = db_rawSize(raw[-1].addr);

	if (oldSize >= amt)
		return old;

	if ((bits = js_rawAlloc(size, zeroit)))
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

	mem->addr = bits;

	bits = raw[-1].addr;

	if(debug) {
	  if(bits == 0xdeadbeef) {
		fprintf (stderr, "js_realloc: out of memory!\n");
		exit(1);
	  }
	  raw[-1].addr = 0xdeadbeef;
	}

	db_memFree (bits);
	return mem + 1;
}


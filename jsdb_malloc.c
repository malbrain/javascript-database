#include <errno.h>

#ifndef __APPLE__
#include <malloc.h>
#endif

#include "jsdb.h"
#include "jsdb_db.h"
#include "jsdb_malloc.h"

DbArena memArena[1];
DbMap memMap[1];

void memInit() {
	memMap->arena = memArena;
#ifdef _WIN32
	memMap->hndl[0] = INVALID_HANDLE_VALUE;
#else
	memMap->hndl[0] = -1;
#endif
}

void jsdb_free (void *obj) {
	rawobj_t *raw = obj;

	if (raw[-1].addr->dead) {
		fprintf(stderr, "Duplicate jsdb_free\n");
		exit (1);
	}

	addSlotToFrame(memMap, &memArena->freeBlk[raw[-1].addr->type], raw[-1].addr->bits);
	raw[-1].addr->dead = 1;
}

//	raw memory allocator

uint64_t jsdb_rawalloc(uint32_t amt, bool zeroit) {
	uint32_t bits = 3;
	uint64_t addr;

#ifdef _WIN32
	_BitScanReverse((unsigned long *)&bits, amt - 1);
	bits++;
#else
	bits = 32 - (__builtin_clz (amt - 1));
#endif
	if ((addr = allocObj(memMap, &memArena->freeBlk[bits], bits, 1UL << bits, zeroit)))
		return addr;

	fprintf (stderr, "out of memory!\n");
	exit(1);
}

void *jsdb_rawaddr(uint64_t rawAddr) {
	DbAddr addr;

	addr.bits = rawAddr;
	return getObj(memMap, addr);
}

void jsdb_rawfree(uint64_t rawAddr) {
	DbAddr addr;

	addr.bits = rawAddr;
	addSlotToFrame(memMap, &memArena->freeBlk[addr.type], rawAddr);
}

//	allocate reference counted object

void *jsdb_alloc(uint32_t len, bool zeroit) {
	rawobj_t *mem;
	DbAddr addr;

	addr.bits = jsdb_rawalloc(len + sizeof(rawobj_t), zeroit);
	mem = getObj(memMap, addr);
	mem->addr->bits = addr.bits;
	mem->weakCnt[0] = 0;
	mem->refCnt[0] = 0;
	return mem + 1;
}

uint32_t jsdb_size (void *obj) {
	rawobj_t *raw = obj;

	return (1 << raw[-1].addr->type) - sizeof(rawobj_t);
}

void *jsdb_realloc(void *old, uint32_t size, bool zeroit) {
	uint32_t amt = size + sizeof(rawobj_t), bits;
	rawobj_t *raw = old, *mem;
	uint32_t oldSize, newSize;
	DbAddr addr[1];
	int oldBits;

#ifdef _WIN32
	_BitScanReverse((unsigned long *)&bits, amt - 1);
	bits++;
#else
	bits = 32 - (__builtin_clz (amt - 1));
#endif
	if (raw[-1].addr->dead) {
		fprintf(stderr, "Duplicate jsdb_realloc\n");
		exit (1);
	}

	//  is the new size within the same power of two?

	oldBits = raw[-1].addr->type;
	oldSize = 1UL << oldBits;
	newSize = 1UL << bits;

	if (oldBits == bits)
		return old;

	if ((addr->bits = allocObj(memMap, &memArena->freeBlk[bits], bits, newSize, zeroit)))
		mem = getObj(memMap, *addr);
	else {
		fprintf (stderr, "out of memory!\n");
		exit(1);
	}

	//  copy contents and release old allocation

	memcpy(mem, raw - 1, oldSize);

	if (zeroit)
		memset((char *)mem + oldSize, 0, newSize - oldSize);

	addSlotToFrame(memMap, &memArena->freeBlk[oldBits], raw[-1].addr->bits);
	raw[-1].addr->dead = 1;

	mem->addr->bits = addr->bits;
	return mem + 1;
}

// duplicate the vector

void *vec_dup(void *vector) {
	rawobj_t *raw = vector, *mem;
	uint32_t size, bits;
	DbAddr addr[1];

	if (!vector)
		return NULL;

	bits = raw[-1].addr->type;
	size = 1UL << bits;

	if ((addr->bits = allocObj(memMap, &memArena->freeBlk[bits], bits, size, false)))
		mem = getObj(memMap, *addr);
	else {
		fprintf (stderr, "out of memory!\n");
		exit(1);
	}

	memcpy(mem, raw - 1, size);

	mem->addr->bits = addr->bits;
	return (int *)(mem + 1) + 2;
}

// dynamically grow the vector

void *vec_grow(void *vector, int increment, int itemsize) {
	int dbl_cur = vector ? 2*vec_max(vector) : 0;
	int min_needed = vec_count(vector) + increment;
	int m = dbl_cur > min_needed ? dbl_cur : min_needed;
	int *p;

	if (m < 4)
		m = 4;

	itemsize *= m;
	itemsize += sizeof(int) * 2;

	if (vector)
		p = jsdb_realloc(vec_raw(vector), itemsize, true);
	else
		p = jsdb_alloc(itemsize, true);

	if (p) {
	  if (!vector) p[1] = 0;
	  p[0] = m;
	  return p+2;
	}

	fprintf(stderr, "vector realloc error: %d\n", errno);
	exit(1);
}

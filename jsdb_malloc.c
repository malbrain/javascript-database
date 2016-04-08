#ifdef _WIN32
#include <intrin.h>
#endif
#include <errno.h>
#include "jsdb.h"
#include "jsdb_db.h"
#include "jsdb_malloc.h"

DbArena memArena[1];
DbAddr freeList[24];		// frames of free objects
DbMap memMap[1];

void memInit() {
	memMap->arena = memArena;
#ifdef _WIN32
	memMap->hndl = INVALID_HANDLE_VALUE;
#else
	memMap->hndl = -1;
#endif
}

void jsdb_free (void *obj) {
	rawobj_t *raw = obj;

	if (raw[-1].addr->dead) {
		fprintf(stderr, "Duplicate jsdb_free\n");
		exit (1);
	}

	addNodeToFrame(memMap, &freeList[raw[-1].addr->type], NULL, *raw[-1].addr);
	raw[-1].addr->dead = 1;
}

void *jsdb_alloc(uint32_t len, bool zeroit) {
	uint32_t amt = len + sizeof(rawobj_t), bits = 3;
	DbAddr addr[1];
	rawobj_t *mem;
	Status error;

#ifdef _WIN32
	_BitScanReverse(&bits, amt - 1);
	bits++;
#else
	bits = 32 - (__builtin_clz (amt - 1));
#endif
/*
	while ((1UL << bits) < amt)
		bits++;
*/
	if ((addr->bits = allocObj(memMap, &freeList[bits], NULL, bits, 1UL << bits, zeroit)))
		mem = getObj(memMap, *addr);
	else {
		fprintf (stderr, "out of memory!\n");
		exit(1);
	}

	mem->refCnt[0] = 0;
	mem->weakCnt[0] = 0;
	mem->addr->bits = addr->bits;
	return mem + 1;
}

uint32_t jsdb_size (value_t val) {
	return (1 << val.raw[-1].addr->type) - sizeof(rawobj_t);
}

void *jsdb_realloc(void *old, uint32_t size, bool zeroit) {
	uint32_t amt = size + sizeof(rawobj_t), bits = 3;
	rawobj_t *raw = old, *mem;
	DbAddr addr[1];
	int oldBits;

#ifdef _WIN32
	_BitScanReverse(&bits, amt - 1);
	bits++;
#else
	bits = 32 - (__builtin_clz (amt - 1));
#endif
/*
	while ((1UL << bits) < amt)
		bits++;
*/
	if (raw[-1].addr->dead) {
		fprintf(stderr, "Duplicate jsdb_realloc\n");
		exit (1);
	}

	//  is the new size within the same power of two?

	oldBits = raw[-1].addr->type;

	if (oldBits == bits)
		return old;

	if ((addr->bits = allocObj(memMap, &freeList[bits], NULL, bits, 1UL << bits, zeroit)))
		mem = getObj(memMap, *addr);
	else {
		fprintf (stderr, "out of memory!\n");
		exit(1);
	}

	//  copy contents and return old allocation

	memcpy(mem, raw - 1, (1ULL << oldBits));
	addNodeToFrame(memMap, &freeList[oldBits], NULL, *raw[-1].addr);
	raw[-1].addr->dead = 1;

	mem->addr->bits = addr->bits;
	return mem + 1;
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


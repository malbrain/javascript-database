#include <intrin.h>

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

	addNodeToFrame(memMap, &freeList[raw[-1].addr->type], NULL, *raw[-1].addr);
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
	bits = __builtin_clz (amt - 1) + 1;
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

void *jsdb_realloc(void *old, uint32_t size) {
	uint32_t amt = size + sizeof(rawobj_t), bits = 3;
	rawobj_t *raw = old, *mem;
	DbAddr addr[1];

#ifdef _WIN32
	_BitScanReverse(&bits, amt - 1);
	bits++;
#else
	bits = __builtin_clz (amt - 1) + 1;
#endif
/*
	while ((1UL << bits) < amt)
		bits++;
*/
	//  is the new size within the same power of two?

	if (raw[-1].addr->type == bits)
		return old;

	if ((addr->bits = allocObj(memMap, &freeList[bits], NULL, bits, 1UL << bits, false)))
		mem = getObj(memMap, *addr);
	else {
		fprintf (stderr, "out of memory!\n");
		exit(1);
	}

	//  copy contents and return old allocation

	memcpy(mem + 1, raw, (1ULL << raw[-1].addr->type) - sizeof(rawobj_t));
	addNodeToFrame(memMap, &freeList[raw[-1].addr->type], NULL, *raw[-1].addr);

	mem->refCnt[0] = 0;
	mem->weakCnt[0] = 0;
	mem->addr->bits = addr->bits;
	return mem + 1;
}

// dynamically grow the vector

void *vec_grow(void *vector, int increment, int itemsize) {
	int dbl_cur = vector ? 2*vec_max(vector) : 0;
	int min_needed = vec_count(vector) + increment;
	int m = dbl_cur > min_needed ? dbl_cur : min_needed;
	int *p;

	if (m < 5)
		m = 5;

	itemsize *= m;
	itemsize += sizeof(int) * 2;

	if (vector)
		p = jsdb_realloc(vec_raw(vector), itemsize);
	else
		p = jsdb_alloc(itemsize, false);

	if (p) {
	  if (!vector) p[1] = 0;
	  p[0] = m;
	  return p+2;
	}

	fprintf(stderr, "vector realloc error: %d\n", errno);
	exit(1);
}


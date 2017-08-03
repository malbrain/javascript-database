#include <errno.h>

#ifndef __APPLE__
#include <malloc.h>
#endif

#include "js.h"
#include "js_malloc.h"
#include "database/db_malloc.h"

extern bool mallocDebug;

#define firstCapacity 10

// duplicate the vector

void *vec_dup(void *vector) {
rawobj_t *raw = vector, *mem;
uint32_t size;

	if (!vector)
		return NULL;

	size = js_size(vec_raw(vector));

	if ((mem = js_alloc(size, false)))
		memcpy(mem, raw, size);
	else {
		fprintf (stderr, "vec_dup: out of memory!\n");
		exit(1);
	}

	*mem[-1].refCnt = *raw[-1].refCnt;
	*mem[-1].weakCnt = *raw[-1].weakCnt;
	return (uint32_t *)mem + 2;
}

// dynamically grow the vector

void *vec_grow(void *vector, uint32_t increment, uint32_t itemsize, bool map) {
uint32_t dbl_cur = 2*vec_max(vector);
uint32_t min_needed = vec_cnt(vector) + increment;
uint32_t cap = dbl_cur > min_needed ? dbl_cur : min_needed;
uint32_t off, size, mapSize = 0, *p;
//rawobj_t *raw, *nxt;

//	raw = (rawobj_t *)v;

	if (cap < firstCapacity)
		cap = firstCapacity;

	if (map) {
		if (cap < 255) {
			mapSize = sizeof(uint8_t);
		} else if (cap < 65535) {
			mapSize = sizeof(uint16_t);
		} else {
			mapSize = sizeof(uint32_t);
		}
	}

	size = itemsize * cap;
	size += sizeof(int) * 2;
	size += mapSize * 3 * cap / 2;

	p = js_alloc(size, false);
//	nxt = (rawobj_t *)p;

	if (vector) {
		off = vec_cnt(vector) * itemsize + 2 * sizeof(int);
		memcpy (p, vec_raw(vector), off);
		memset ((uint8_t *)p + off, 0, size - off);
		js_free(vec_raw(vector));
	} else
		memset (p, 0, size);

	p[0] = cap;
	return p+2;
}

// slice slots from beginning of the vector

void *vec_sliceqty(void *vector, uint32_t qty, uint32_t itemsize) {
uint8_t *dest;
uint32_t idx;

	if (!vector)
		return NULL;

	if (vec_size(vector) < qty) {
		vec_size(vector) = 0;
		return vector;
	}

	vec_size(vector) -= qty;
	dest = vector;

	for (idx = 0; idx < vec_size(vector); idx++) {
		memcpy (dest, dest + qty * itemsize, itemsize);
		dest += itemsize;
	}

	return vector;
}

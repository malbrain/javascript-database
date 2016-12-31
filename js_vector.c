#include <errno.h>

#ifndef __APPLE__
#include <malloc.h>
#endif

#include "js.h"
#include "js_malloc.h"
#include "database/db_malloc.h"

// duplicate the vector

void *vec_dup(void *vector) {
rawobj_t *raw = vector, *mem;
uint32_t size;
uint64_t bits;

	if (!vector)
		return NULL;

	size = js_size(vec_raw(vector));

	if ((bits = js_rawAlloc(size, false)))
		mem = db_memObj(bits);
	else {
		fprintf (stderr, "vec_dup: out of memory!\n");
		exit(1);
	}

	memcpy(mem, raw - 1, size);
	return (int *)(mem + 1) + 3;
}

// dynamically grow the vector

void *vec_grow(void *vector, int increment, int itemsize, bool map) {
int dbl_cur = 2*vec_max(vector);
int min_needed = vec_cnt(vector) + increment;
int cap = dbl_cur > min_needed ? dbl_cur : min_needed;
int size, mapSize = 0;
int *p;

	if (cap < 4)
		cap = 4;

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
	size += sizeof(int) * 3;
	size += mapSize * 3 * cap / 2;

	if (vector)
		p = js_realloc(vec_raw(vector), size, true);
	else
		p = js_alloc(size, true);

	if (p) {
	  p[0] = cap;
	  p[1] = mapSize;
	  return p+3;
	}

	fprintf(stderr, "vector realloc error: %d\n", errno);
	exit(1);
}

// slice slots from beginning of the vector

void *vec_sliceqty(void *vector, int qty, int itemsize) {
uint8_t *dest;
int idx;

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

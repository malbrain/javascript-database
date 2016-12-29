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
	return (int *)(mem + 1) + 2;
}

// dynamically grow the vector

void *vec_grow(void *vector, int increment, int itemsize, int itemxtra) {
int dbl_cur = 2*vec_max(vector);
int min_needed = vec_cnt(vector) + increment;
int m = dbl_cur > min_needed ? dbl_cur : min_needed;
int size;
int *p;

	if (m < 4)
		m = 4;

	size = (itemsize + itemxtra) * m;
	size += sizeof(int) * 2;

	if (vector)
		p = js_realloc(vec_raw(vector), size, true);
	else
		p = js_alloc(size, true);

	if (p) {
	  p[0] = m;
	  return p+2;
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

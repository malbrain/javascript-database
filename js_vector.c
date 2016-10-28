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
		p = js_realloc(vec_raw(vector), itemsize, true);
	else
		p = js_alloc(itemsize, true);

	if (p) {
	  if (!vector) p[1] = 0;
	  p[0] = m;
	  return p+2;
	}

	fprintf(stderr, "vector realloc error: %d\n", errno);
	exit(1);
}

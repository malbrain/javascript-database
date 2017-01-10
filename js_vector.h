#pragma once

// API

#define vec_free(a)				((a) ? js_free(vec_raw(a)),0 : 0)
#define vec_push(a,v)			(vec_maybegrow(a,1), (a)[vec_size(a)++] = (v))
#define vec_pop(a)				((a)[--vec_size(a)])
#define vec_cnt(a)				((a) ? vec_size(a) : 0)
#define vec_max(a)				((a) ? vec_cap(a) : 0)
#define vec_add(a,n)			(vec_maybegrow(a,n), vec_size(a)+=(n))
#define vec_last(a)				((a)[vec_size(a)-1])

// access with a[i]

// internal

#define vec_raw(a)				((int *) (a) - 3)
#define vec_cap(a)				vec_raw(a)[0]		// maximum capacity
#define vec_map(a)				vec_raw(a)[1]		// map entry size
#define vec_size(a)				vec_raw(a)[2]		// current size

#define vec_needgrow(a,n)		((a)==0 || vec_size(a)+(n) > vec_cap(a))
#define vec_maybegrow(a,n)		(vec_needgrow(a,(n)) ? ((a) = vec_grow((a), (n), sizeof(*a), 0)) : 0)
#define vec_slice(a,q)			vec_sliceqty(a, q, sizeof(*a))

void *vec_grow(void *vector, int increment, int itemsize, bool map);
void *vec_sliceqty(void *vector, int qty, int itemsize);
void *vec_dup(void *vector);

#define newVector(numitems,itemsize,map) (vec_grow(NULL, numitems, itemsize, map))

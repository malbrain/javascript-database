#pragma once

// API

#define vec_free(a)				((a) ? js_free(vec_raw(a)),0 : 0)
#define vec_push(a,v)			(vec_maybegrow(a,1), (a)[vec_size(a)++] = (v))
#define vec_pop(a,n)			(((a) && vec_size(a)) ? ((a)[--vec_size(a)]) : n)
#define vec_count(a)			((a) ? vec_size(a) : 0)
#define vec_add(a,n)			(vec_maybegrow(a,n), vec_size(a)+=(n))
#define vec_last(a)				((a)[vec_size(a)-1])
// access with a[i]

// internal

#define vec_raw(a)				((int *) (a) - 2)
#define vec_max(a)				vec_raw(a)[0]			// allocated size
#define vec_size(a)				vec_raw(a)[1]			// current size

#define vec_needgrow(a,n)		((a)==0 || vec_size(a)+(n) > vec_max(a))
#define vec_maybegrow(a,n)	(vec_needgrow(a,(n)) ? ((a) = vec_grow((a), (n), sizeof(*a))) : 0)

void *vec_grow(void *vector, int increment, int itemsize);
void *vec_dup(void *vector);

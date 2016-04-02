#pragma once

// API

#define vec_free(a)			((a) ? free(_vector_raw(a)),0 : 0)
#define vec_push(a,v)		(_vector_maybegrow(a,1), (a)[_vector_size(a)++] = (v))
#define vec_count(a)		((a) ? _vector_size(a) : 0)
#define vec_add(a,n)		(_vector_maybegrow(a,n), _vector_size(a)+=(n), &(a)[_vector_size(a)-(n)])
#define vec_last(a)			((a)[_vector_size(a)-1])
// access with a[i]

// internal

#define _vector_raw(a)			((int *) (a) - 2)
#define _vector_max(a)			_vector_raw(a)[0]			// allocated size
#define _vector_size(a)			_vector_raw(a)[1]			// current size

#define _vector_needgrow(a,n)	((a)==0 || _vector_size(a)+(n) > _vector_max(a))
#define _vector_maybegrow(a,n)	(_vector_needgrow(a,(n)) ? ((a) = _vector_grow((a), (n), sizeof(*(a)))) : 0)

void *_vector_grow(void *_vector, int increment, int itemsize);

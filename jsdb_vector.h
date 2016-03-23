#pragma once

#include <stdlib.h>

// API

#define vec_free(a)			((a) ? free(vec__raw(a)),0 : 0)
#define vec_push(a,v)		(vec__maybegrow(a,1), (a)[vec__n(a)++] = (v))
#define vec_count(a)		((a) ? vec__n(a) : 0)
#define vec_add(a,n)		(vec__maybegrow(a,n), vec__n(a)+=(n), &(a)[vec__n(a)-(n)])
#define vec_last(a)			((a)[vec__n(a)-1])
// access with a[i]

// internal

#define vec__raw(a)			((int *) (a) - 2)
#define vec__m(a)			vec__raw(a)[0]			// allocated size
#define vec__n(a)			vec__raw(a)[1]			// current size

#define vec__maybegrow(a,n)	(vec__needgrow(a,(n)) ? vec__grow(a,n) : 0)
#define vec__needgrow(a,n)	((a)==0 || vec__n(a)+(n) >= vec__m(a))
#define vec__grow(a,n)		((a) = vec__growf((a), (n), sizeof(*(a))))

// dynamically grow the buffer

static void * vec__growf(void *arr, int increment, int itemsize) {
	int dbl_cur = arr ? 2*vec__m(arr) : 0;
	int min_needed = vec_count(arr) + increment;
	int m = dbl_cur > min_needed ? dbl_cur : min_needed;
	int *p = (int *)realloc(arr ? vec__raw(arr) : 0, itemsize * m + sizeof(int)*2);
	if (p) {
	  if (!arr) p[1] = 0;
	  p[0] = m;
	  return p+2;
	} else {
	  return (void *) (2*sizeof(int)); // try to force a NULL pointer exception later
	}
}


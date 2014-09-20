#ifndef _LIBALLOC_H
#define _LIBALLOC_H
#include <types.h>

void    *malloc(size_t);				///< The standard function.
void    *realloc(void *, size_t);		///< The standard function.
void    *calloc(size_t, size_t);		///< The standard function.
void     free(void *);					///< The standard function.

#endif


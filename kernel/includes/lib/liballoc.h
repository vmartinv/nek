#ifndef _LIBALLOC_H
#define _LIBALLOC_H
#include <types.h>

#if defined(__cplusplus)
extern "C"{
#endif
void    *malloc(size_t);				///< The standard function.
void    *realloc(void *, size_t);		///< The standard function.
void    *calloc(size_t, size_t);		///< The standard function.
void     free(void *);					///< The standard function.
#if defined(__cplusplus)
}
#endif

#if defined(__cplusplus)
void *operator new(size_t size);
 
void *operator new[](size_t size);
 
void operator delete(void *p);
 
void operator delete[](void *p);
#endif

#endif


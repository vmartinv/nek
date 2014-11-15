#ifndef STDLIB_H
#define STDLIB_H
#include <types.h>
#include <lib/liballoc.h>

#ifdef __cplusplus
extern "C" {
#endif
 
//not really random at all... Plase don't use for something important.
#define RAND_MAX 32767
int rand();
void srand(unsigned int seed);


#ifdef __cplusplus
}
#endif
 
#endif

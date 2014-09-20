#ifndef STDLIB_H
#define STDLIB_H
#include <types.h>
#include <lib/liballoc.h>

//not really random at all... Plase don't use for something important.
#define RAND_MAX 32767
int rand();
void srand(unsigned int seed);


#endif

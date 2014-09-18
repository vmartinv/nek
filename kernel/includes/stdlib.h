#ifndef STDLIB_H
#define STDLIB_H
#include <types.h>
void* malloc(size_t size);
void free(void* ptr);
void* calloc(size_t num_of_elts, size_t elt_size);
void* realloc(void* pointer, size_t size);

//not really random at all... Plase don't use for something important.
#define RAND_MAX 32767
int rand();
void srand(unsigned int seed);


#endif

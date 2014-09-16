#ifndef STDLIB_H
#define STDLIB_H
#include <stddef.h>
#include <stdint.h>
void* malloc(size_t size);
void free(void* ptr);
void* calloc(size_t num_of_elts, size_t elt_size);
void* realloc(void* pointer, size_t size);
#endif
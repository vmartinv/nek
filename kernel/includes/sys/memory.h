#ifndef MEMORY_H
#define MEMORY_H
#include <types.h>

void init_kmalloc(uintptr_t kernelend);
uintptr_t * kmalloc(size_t sz);
uintptr_t kmalloc_get_pre_placementaddr();

void init_pmm(intptr_t kernelstart, size_t upper_b);
uint32_t pmm_get_memory_bytes();

//for profiler
int pmm_bitmap_test(uintptr_t address);


#endif

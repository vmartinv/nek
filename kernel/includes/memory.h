#ifndef CEDILLE_MEM_H
#define CEDILLE_MEM_H
#include <stddef.h>
#include <stdint.h>
#include <arch/x86/paging.h>
void init_malloc(uint32_t status,uint32_t addr_bytes);
uintptr_t * kmalloc(size_t sz);
uintptr_t * kmalloc_aligned(size_t sz);
uintptr_t * kmalloc_aligned_phys(size_t sz, uintptr_t * phys);
#ifdef BOARDx86generic
uintptr_t * sbrk(size_t amount);
#endif
void init_pmm(uint32_t total_kb);
void pmm_bitmap_set(uintptr_t address);
void pmm_bitmap_clear(uintptr_t address);
uint32_t pmm_bitmap_test(uintptr_t address);
uint32_t pmm_bitmap_findfree();
void pmm_alloc_frame(page_t *page, int is_kernel, int is_writeable);
void pmm_dealloc_frame(page_t *page);
page_t * pmm_get_page(page_directory_t *dir,uint32_t address, uint8_t make);

//Large heap
uintptr_t * lheap_alloc_pages(uint32_t pages);
#endif
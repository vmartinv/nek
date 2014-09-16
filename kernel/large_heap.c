#include <stdint.h>
#include <stddef.h>
#include <memory.h>
#include <logging.h>
#define PAGE_SIZE 0x1000
#ifdef BOARDx86generic
extern page_directory_t * kernel_page_directory;

//TODO: Replace with better algorithm, one that DEALLOCS
uintptr_t * lheap_alloc_pages(uint32_t pages)
{
	uint32_t first_free = pmm_bitmap_findfree() * 0x1000;
	for(uint32_t i = 0; i < pages; i++)
	{
		pmm_alloc_frame(pmm_get_page(kernel_page_directory,first_free + (i * 0x1000), 1), 1,1);
	}
	return (uintptr_t *)(first_free);
}

uintptr_t * sbrk(size_t amount)
{
	if(amount == 0)
	{
		return (uintptr_t*)(pmm_bitmap_findfree() * 0x1000);
	}
	uint32_t actual_amount = (amount / 0x1000);
	if(actual_amount == 0)
	{
		actual_amount++;
	}
	//printf("sbrk: allocating %d pages to cover 0x%X bytes\n",actual_amount,amount);
	void *tmp = lheap_alloc_pages(actual_amount);
	return tmp;
}
#else
uintptr_t * _sbrk(size_t amount)
{
	return NULL;
}
#endif

#include <logging.h>
#include <memory.h>
#include <string.h>
#include <arch/x86/paging.h>
#include <cedille.h>
#include <stdio.h>

///Bitmap of every page in the system, and whether its free (0) or used (1).
uintptr_t * bitmap; 
 ///How many frames bitmap holds.
uint32_t bitmap_frames = 0x0;
/**
Sets the page that address is in to be used.
**/
void pmm_bitmap_set(uintptr_t address)
{
	uint32_t frame = address / 0x1000;
	uint32_t idx = INDEX_FROM_BIT(frame);
	uint32_t off = OFFSET_FROM_BIT(frame);
	bitmap[idx] |= (0x1 << off);
}
/**
Sets the page that address is in to be free.
**/
void pmm_bitmap_clear(uintptr_t address)
{
   uint32_t frame = address / 0x1000;
   uint32_t idx = INDEX_FROM_BIT(frame);
   uint32_t off = OFFSET_FROM_BIT(frame);
   bitmap[idx] &= ~(0x1 << off);
}
/**
Tests to see that the page address is in is set.
**/
uint32_t pmm_bitmap_test(uintptr_t address)
{
   uint32_t frame = address / 0x1000;
   uint32_t idx = INDEX_FROM_BIT(frame);
   uint32_t off = OFFSET_FROM_BIT(frame);
   return (bitmap[idx] & (0x1 << off));
}
/**
Finds the first free page, and returns it's address
**/
uint32_t pmm_bitmap_findfree()
{
	uint32_t i, j;
	for (i = 0; i < INDEX_FROM_BIT(bitmap_frames); i++)
	{
		if (bitmap[i] != 0xFFFFFFFF) // nothing free, exit early.
		{
			// at least one bit is free here.
			for (j = 0; j < 32; j++)
			{
				uint32_t toTest = 0x1 << j;
				if ( !(bitmap[i]&toTest) )
				{
					return i*4*8+j;
				}
			}
		}
	}
	return (uint32_t)-1;
}
/**
Allocates a page frame automatically. 
**/
void pmm_alloc_frame(page_t *page, int is_kernel, int is_writeable)
{
	if(page->frame != 0)
	{
		//oops("Attempted to realloc a page frame!");
		return;
	}
	uint32_t index = pmm_bitmap_findfree();
	if(index == (uintptr_t)-1) //If there is no more memory
	{
		//TODO: Call Kernel OOM Manager before panic
		panic("Out of memory");
	}
	pmm_bitmap_set(index * 0x1000); // Make it the size of a page address boundry
	page->present = 1;
	page->rw = (is_writeable)?1:0;
	page->user = (is_kernel)?0:1;
	page->frame = index;
}
/**
Deallocates a frame.
**/
void pmm_dealloc_frame(page_t *page)
{
	if(page->frame != 0)
	{
		pmm_bitmap_clear(page->frame);
		page->frame = 0x0; // Page now doesn't have a frame.
	}

}
/**
Gets a page from dir, corresponding to the address address. If make is non-zero, and the page doesnt exist, it is created.
**/
page_t * pmm_get_page(page_directory_t *dir,uint32_t address, uint8_t make)
{
	uint32_t page_index = address / 0x1000;		//Page index
	uint32_t table_index = page_index / 1024;	//Table index
	if (dir->tables[table_index])
	{
		return &dir->tables[table_index]->pages[page_index%1024];
	}
	else if(make)
	{
		uint32_t temp;
		dir->tables[table_index] = (page_table_t*)kmalloc_aligned_phys(sizeof(page_table_t), &temp);
		memset(dir->tables[table_index], 0, 0x1000);
		dir->tablesPhysical[table_index] = temp | 0x7; // PRESENT, RW, US.
		return &dir->tables[table_index]->pages[page_index%1024];
	}
	return NULL;
}

void init_pmm(uint32_t total_b)
{
	if(total_b == 0)
	{
		printk("fail","PMM:Unknown amount of free memory? Reading Configuration Space...\n");
		return;
	}
	bitmap_frames = total_b / 1000; // KB = 1000, KiB = 1024
	
	bitmap = kmalloc(INDEX_FROM_BIT(bitmap_frames));
	memset(bitmap, 0, INDEX_FROM_BIT(bitmap_frames));
	
}
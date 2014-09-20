#include <sys/logging.h>
#include <sys/memory.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <sys/profiler.h>

///Bitmap of every page in the system, and whether its free (0) or used (1).
uintptr_t * bitmap; 
 ///bitmap size.
size_t bitmap_sz = 0x0;

#define PAGE_SIZE 0x1000
static uint32_t first_free_byte=0;
//~ const uintptr_t offset=1024*1024;//First MB
uintptr_t offset=0;//First MB


/**
Sets the page that address is in to be used.
**/
inline static void bitmap_set(uint32_t a, uint32_t b)
{
	uint32_t aby=a/32, bby=b/32, aoff=a%32, boff=b%32;
	if(aby==bby){
		bitmap[aby]|= ~((1<<aoff)-1) & ((1<<boff)-1);
	}
	else{
		bitmap[aby]|= ~((1<<aoff)-1);
		bitmap[bby]|=  ((1<<boff)-1);
		if(b-a>33)
			memset(&bitmap[aby+1], 0xFF, sizeof(uint32_t)*(bby-aby-1));
	}
	if(aby==first_free_byte && bitmap[aby]==0xFFFFFFFF){
		first_free_byte=bby;
		while(bitmap[first_free_byte]==0xFFFFFFFF) first_free_byte++;
	}
}

/**
Sets the page that address is in to be free.
**/
inline static void bitmap_clear(uint32_t a, uint32_t b)
{
	uint32_t aby=a/32, bby=b/32, aoff=a%32, boff=b%32;
	if(aby==bby){
		bitmap[aby]&= ((1<<aoff)-1) | ~((1<<boff)-1);
	}
	else{
		bitmap[aby]&=  ((1<<aoff)-1);
		bitmap[bby]&= ~((1<<boff)-1);
		if(b-a>33)
			memset(&bitmap[aby+1], 0x00, sizeof(uint32_t)*(bby-aby-1));
	}
	if(aby<first_free_byte) first_free_byte=aby;
}

/**
Tests to see that the page address is in is set.
**/
int pmm_bitmap_test(uintptr_t address)
{
	if(address<offset) return -1;//kernel memory
    uint32_t frame = (address-offset) / PAGE_SIZE;
	return bitmap[frame/32] & (1 << (frame%32)) ? 1 : 0;
}

/**
Finds the first free page, and returns it's address
**/
inline static int fits_inside(uint32_t pages, const uint32_t bits)
{
	uint32_t k=0;
	for(uint32_t i = 0; i < 32; i++){
		if(!((1<<i)&bits)){
			if(++k==pages) return i+1-pages;
		}
		else k=0;
	}
	return -1;
}


/** This function is supposed to lock the memory data structures. It
 * could be as simple as disabling interrupts or acquiring a spinlock.
 * It's up to you to decide. 
 *
 * \return 0 if the lock was acquired successfully. Anything else is
 * failure.
 */
int liballoc_lock(){
	return 0;
}

/** This function unlocks what was previously locked by the liballoc_lock
 * function.  If it disabled interrupts, it enables interrupts. If it
 * had acquiried a spinlock, it releases the spinlock. etc.
 *
 * \return 0 if the lock was successfully released.
 */
int liballoc_unlock(){
	return 0;
}

/** This is the hook into the local system which allocates pages. It
 * accepts an integer parameter which is the number of pages
 * required.  The page size was set up in the liballoc_init function.
 *
 * \return NULL if the pages were not allocated.
 * \return A pointer to the allocated memory.
 */
void* liballoc_alloc(int pages){
	//~ printf("allocating %d pages...\n", pages);
	uintptr_t start=0;
	size_t left=pages;
	for(size_t i = first_free_byte; i < bitmap_sz; i++){
		if(pages<31){
			int f=fits_inside(pages, bitmap[i]);
			if(f!=-1){
				bitmap_set(i*32+f, i*32+f+pages);
				return (void*)((i*32+f)*PAGE_SIZE+offset);
			}
		}
		size_t zerosL=bitmap[i]?__builtin_clz(bitmap[i]):32;
		if(zerosL!=32 && left>zerosL){
			int zerosR=bitmap[i]?__builtin_ctz(bitmap[i]):32;
			left=pages-zerosR;
			start=i*32+32-zerosR;
		}
		else{
			if(left<=zerosL){
				bitmap_set(start, start+pages);
				return (void*)(start*PAGE_SIZE+offset);
			} 
			left-=zerosL;
		}
	}
	return NULL;
}

/** This frees previously allocated memory. The void* parameter passed
 * to the function is the exact same value returned from a previous
 * liballoc_alloc call.
 *
 * The integer value is the number of pages to free.
 *
 * \return 0 if the memory was successfully freed.
 */
int liballoc_free(void*ptr,int pages){
	//~ printf("freeing %d pages...\n", pages);
	uint32_t start=(((uintptr_t)ptr)-offset)/PAGE_SIZE;
	bitmap_clear(start, start+pages);
	return 0;
}


uint32_t pmm_get_memory_bytes(){
	return bitmap_sz*32*PAGE_SIZE;
}

void init_pmm(intptr_t kernelstart, size_t upper_b)
{
	if(upper_b == 0){
		printk("fail","PMM:Unknown amount of free memory.\n");
		return;
	}
	offset=kmalloc_get_pre_placementaddr();//start of the free memory
	size_t space_left=upper_b+kernelstart-offset;
	bitmap_sz =space_left/PAGE_SIZE/32;
	bitmap = kmalloc(bitmap_sz*sizeof(uint32_t));//each element stores 32 pages
	
	offset=kmalloc_get_pre_placementaddr();//since we used kmalloc, recalculate offsets
	space_left=upper_b+kernelstart-offset;
	bitmap_sz =space_left/PAGE_SIZE/32;
	memset(bitmap, 0, sizeof(uint32_t)*bitmap_sz);//set all pages free
}

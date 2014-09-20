#include <stdio.h>
#include <sys/memory.h>
#include <sys/logging.h>
#include <graphics/video.h>
#ifdef BOARDx86generic
#include <arch/x86/textmode_console.h>
#endif
#include <sys/profiler.h>
#include <sys/console.h>

#ifdef BOARDx86generic
extern uint32_t _kernel_start;

void profiler_memory()
{
	console_printdiv();
	//~ printf("Physical Address Space = 0x0 -> 0x%x (heap ptr at 0x%x)\n",memory_bytes_avalable,lheap_alloc_pages(0)); //TODO
	size_t block_size = pmm_get_memory_bytes() / ((PROFILER_BLOCKS) - 1);
	
	if(block_size < 0x1000)
	{
		block_size = 0x1000;
	}
	if(block_size > 0xA0000)
	{
		printf("Woah! Profiling at block size 0x%x is inaccurate!\n",block_size);
	}
	printf("Used Memory (block size is 0x%x):\n",block_size);
	uint32_t col = 0;
	uint32_t total = 0;
	uint32_t used = 0;
	uint32_t free = 0;
	textmode_setforecolor(PROFILER_C_DEBUG);
	printf(" %04d ",0);
	for (uintptr_t i = (uintptr_t)&_kernel_start; i <= pmm_get_memory_bytes(); i += block_size) {
		int count=0;
		bool iskernel=false;
		for(uintptr_t j=i; j<i+block_size; j+=0x1000){
			int ans=pmm_bitmap_test(j);
			if(ans==-1) iskernel=1;
			count+=ans*0x1000;
		}
		if(iskernel){
			textmode_setforecolor(PROFILER_C_KERNEL);
			printf("%c",219);
		}
		else if((count+block_size-1)/block_size>0)
		{
			used++;
			textmode_setforecolor(PROFILER_C_YES);
			printf("%c",219);
		}
		else
		{
			free++;
			textmode_setforecolor(PROFILER_C_NO);
			printf("%c",176);
		}
		if(!iskernel) total++;
		col++;
		if(col == PROFILER_COL)
		{
			col = 0;
			textmode_setforecolor(PROFILER_C_DEBUG);
			printf(" <-0x%08x\n %04d ",i,total);
		}
	}
	textmode_setforecolor(PROFILER_C_DEBUG);
	printf("\b\b\b\b\b     %d/%d blocks used\n",used,total);
	textmode_resetcolor();
	console_printdiv();;
}
#endif

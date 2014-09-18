#include <stdio.h>
#include <memory.h>
#include <logging.h>
#include <config.h>
#include <graphics/video.h>
#ifdef BOARDx86generic
#include <arch/x86/textmode_console.h>
#endif

extern uint32_t memory_bytes_avalable;

void console_printdiv();

void profiler_log()
{
	START_PROFILE
	printk("debug","Profiling enabled~!\n");
}

#ifdef BOARDx86generic

void profiler_memory()
{
	START_PROFILE
	console_printdiv();
	printf("Physical Address Space = 0x0 -> 0x%x (heap ptr at 0x%x)\n",memory_bytes_avalable,lheap_alloc_pages(0));
	uint32_t block_size = memory_bytes_avalable / ((PROFILER_BLOCKS) - 1);
	
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
	textmode_setforecolor(PROFILER_C_DEBUG);
	printf(" %04d ",0);
	for (uint32_t i = 0; i <= memory_bytes_avalable; i += block_size) {
		if(pmm_bitmap_test(i))
		{
			textmode_setforecolor(PROFILER_C_YES);
			printf("%c",219);
		}
		else
		{
			textmode_setforecolor(PROFILER_C_NO);
			//printf("%c",176);
			printf("%c",219);
		}
		total++;
		col++;
		if(col == PROFILER_COL)
		{
			col = 0;
			textmode_setforecolor(PROFILER_C_DEBUG);
			printf(" <-0x%08x\n %04d ",i,total);
		}
	}
	uint32_t used = 0;
	uint32_t free = 0;
	for (uint32_t i = 0; i <= memory_bytes_avalable; i += block_size) {
		if(pmm_bitmap_test(i))
		{
			used += 1;
		}
		else
		{
			free += 1;
		}
		//total += 1;
	}
	textmode_setforecolor(PROFILER_C_DEBUG);
	printf("\b\b\b\b\b     %d/%d blocks used\n",used,total);
	textmode_resetcolor();
	console_printdiv();;
}
#endif

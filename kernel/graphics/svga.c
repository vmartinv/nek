#include <graphics/svga.h>
#include <arch/x86/paging.h>
#include <memory.h>

extern page_directory_t *kernel_page_directory;
/*
 * Requests the physical frame buffer address be mapped at the logical frame
 * buffer address, 0xD0000000.
 *
 * This function will map fb_length bytes.
 *
 * On success, it returns the virtual address where the framebuffer was mapped,
 * or 0 on failure.
 */
#define SCREEN_ADDRESS 0x001e0000
uint32_t svga_map_fb(uint32_t real_addr, uint32_t fb_length) {
	// Align framebuffer length to page boundaries
	fb_length += 0x1000;
	fb_length &= 0x0FFFF000;

	// Map enough framebuffer
	for(uint32_t i = SCREEN_ADDRESS; i < SCREEN_ADDRESS + fb_length; i += 0x1000) {
		page_t* page = pmm_get_page(kernel_page_directory, i, true);

		uint32_t fb_addr = (i-SCREEN_ADDRESS)+real_addr;
		page->present = 1;
		page->rw = 1;
		page->user = 0;
		page->frame = fb_addr >> 12;
	}

	// Convert the kernel directory addresses to physical if needed
	//~ for(i = 0x340; i < 0x340 + (fb_length / 0x400000); i++) {
		//~ uint32_t physAddr = kernel_page_directory->tablesPhysical[i];
//~ 
		//~ if((physAddr & 0xC0000000) == 0xC0000000) {
			//~ physAddr &= 0x0FFFFFFF; // get rid of high nybble
//~ 
			//~ kernel_page_directory->tablesPhysical[i] = physAddr;
		//~ }
	//~ }

	return SCREEN_ADDRESS;
}

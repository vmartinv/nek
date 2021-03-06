#include <types.h>
#include <sys/logging.h>
#include <sys/console.h>
#include <version.h>
#include <stdio.h>
#include <sys/multiboot.h>
#include <sys/memory.h>
#include <graphics/svga.h>
#include <graphics/video.h>
#include <string.h>
extern uint32_t _kernel_start, _kernel_end;

#if defined(__cplusplus)
extern "C"  /* Use C linkage. */
#endif
int x86_init_descriptor_tables();
void pit_install(uint32_t frequency);

void kmain(const multiboot_info_t * multiboot);
void kbd_init();
/// The entry point for the x86 version of the NesOS Microkernel
#if defined(__cplusplus)
extern "C" /* Use C linkage for kernel_main. */
#endif
void kernel_entry(int magic, const multiboot_info_t * multiboot) {
    video_init((svga_mode_info_t *)multiboot->vbe_mode_info);
    console_init();
    //~ printk("ok", (svga_mode_info_t *)multiboot->vbe_mode_info->screenheight);
    console_printdiv();
    printk("ok", NAME_S, VERSION_S);
    #ifdef DEBUG
        printk("debug", "kernel image(ram): 0x%X - 0x%X (", \
            &_kernel_start, &_kernel_end, &_kernel_end - &_kernel_start);
        logging_printbestunit(&_kernel_end - &_kernel_start, 0);
        printf(")\n");
    #endif
	
    if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
        printk("fail", "NesOS was booted improperly by the bootloader\n");
        printk("info", "\\==> Kernel is now halting\n");
        return;
    }

    printk("status", "Initialising the processor...\n");
    x86_init_descriptor_tables();
    printk("cpu", "Starting interrupts...\n");
    printk("device", "Starting (basic) PIT...\n");
    pit_install(1000);
	kbd_init();
    asm("sti");
    
	// Find the location of our initial ramdisk.
	assert(multiboot->mods_count > 0);
	u32 initrd_end = *(u32*)(multiboot->mods_addr+4);
	
	u32 placement=(u32)&_kernel_end;
	if(placement<initrd_end) placement=initrd_end;
	// Don't trample our module with placement accesses, please!
    init_kmalloc((uintptr_t)placement);
    
    printk("ok", "Starting PMM...\n");
    init_pmm((uintptr_t)&_kernel_start, multiboot->mem_upper*1024);
    //~ multiboot_memory_map_t * mmap = (multiboot_memory_map_t*)multiboot->mmap_addr;
    //~ while ((uint32_t)mmap < (uint32_t)multiboot->mmap_addr + (uint32_t)multiboot->mmap_length) {
         //~ printf("0x%08X->0x%08X (type %d)\n",mmap->addr,mmap->len,mmap->type);
        //~ mmap = (multiboot_memory_map_t*) ( (unsigned int)mmap + mmap->size + sizeof(unsigned int) );
    //~ }
    printk("debug", "Exiting Boot\n");
    console_printdiv();
    kmain(multiboot);
    return;
}

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

int x86_init_descriptor_tables();
void pit_install(uint32_t frequency);

void dmain();
void kmain();

/// The entry point for the x86 version of the NesOS Microkernel
void kernel_entry(int magic, multiboot_info_t * multiboot) {
    video_load_info((svga_mode_info_t *)multiboot->vbe_mode_info);
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
    asm("sti");
    printk("device", "Starting (basic) PIT...\n");
    pit_install(1000);
    init_kmalloc((uintptr_t)&_kernel_end);
    printk("ok", "Starting PMM...\n");
    init_pmm((uintptr_t)&_kernel_start, multiboot->mem_upper*1024);
    video_init();//now we can set up buffers and other stuff
    //~ multiboot_memory_map_t * mmap = (multiboot_memory_map_t*)multiboot->mmap_addr;
    //~ while ((uint32_t)mmap < (uint32_t)multiboot->mmap_addr + (uint32_t)multiboot->mmap_length) {
         //~ printf("0x%08X->0x%08X (type %d)\n",mmap->addr,mmap->len,mmap->type);
        //~ mmap = (multiboot_memory_map_t*) ( (unsigned int)mmap + mmap->size + sizeof(unsigned int) );
    //~ }
    printk("debug", "Exiting Boot\n");
    console_printdiv();
    dmain();
    kmain();
    return;
}

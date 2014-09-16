#include <stddef.h>
#include <stdint.h>
#include <logging.h>
#include <console.h>
#include <stdbool.h>
#include <cedille.h>
#include <stdio.h>
#include <multiboot.h>
#include <memory.h>
extern uint32_t _kernel_start, _kernel_end;

int x86_init_descriptor_tables();
void pit_install(uint32_t frequency);
void init_vmm();

void kmain();
/// The entry point for the x86 version of the Cedille Microkernel
void kernel_entry(int magic, multiboot_info_t * multiboot) {
    console_init();
    console_printdiv();
    printk("ok", "The C%cdille Microkernel v.%s. (c) Corwin McKnight 2014\n", \
        130, CEDILLE_VERSION_S);
    printk("ok", "Branch: x86/generic\n");
    #ifdef DEBUG
        printk("debug", "kernel image(ram): 0x%X - 0x%X (", \
            &_kernel_start, &_kernel_end, &_kernel_end - &_kernel_start);
        logging_printbestunit(&_kernel_end - &_kernel_start, 0);
        printf(")\n");
    #endif

    if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
        printk("fail", "Cedille was booted improperly by the bootloader\n");
        printk("info", "\\==> Kernel is now halting\n");
        return;
    }

    printk("status", "Initialising the processor...\n");
    x86_init_descriptor_tables();
    printk("cpu", "Starting interrupts...\n");
    asm("sti");
    printk("device", "Starting (basic) PIT...\n");
    pit_install(1000);
    init_malloc(0, multiboot->mem_upper * 1024);
    printk("ok", "Starting ");
    init_pmm(multiboot->mem_upper * 1024);
    printf("PMM ");
    init_vmm();
    printf("& VMM\n");
    // printf("Memory Map at:0x%X\n",multiboot->mmap_addr);
    multiboot_memory_map_t * mmap = (multiboot_memory_map_t*)multiboot->mmap_addr;
    while ((uint32_t)mmap < (uint32_t)multiboot->mmap_addr + (uint32_t)multiboot->mmap_length) {
        // printf("0x%08X->0x%08X (type %d)\n",mmap->addr,mmap->len,mmap->type);
        mmap = (multiboot_memory_map_t*) ( (unsigned int)mmap + mmap->size + sizeof(unsigned int) );
    }
    printk("debug", "Exiting Boot\n");
    console_printdiv();
    kmain();
    return;
}
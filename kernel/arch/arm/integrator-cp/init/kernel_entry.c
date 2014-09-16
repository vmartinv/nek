#include <logging.h>
#include <cedille.h>
#include <console.h>
#include <stdio.h>
void arm_init_interrupts();
extern uint32_t _kernel_start, _kernel_end;
void kmain();
void kernel_entry() {
    console_init();
    console_printdiv();
    printf("\n");  // Force a new line as the hres might not be 80 cols.
    printk("status", "Kernel initialising...\n");
    printk("ok", "The Cedille Microkernel v.%s. (c) Corwin McKnight 2014\n",
        CEDILLE_VERSION_S);
    printk("ok", "Branch: arm/integrator-cp\n");

    #ifdef DEBUG
    printk("debug", "kernel image(ram): 0x%X - 0x%X (", \
        &_kernel_start, &_kernel_end, &_kernel_end - &_kernel_start);
    logging_printbestunit(&_kernel_end - &_kernel_start, 0);
    printf(")\n");
    #endif

    printk("status", "Initialising the processor...\n");

    printk("cpu", "Starting interrupts....\n");
    arm_init_interrupts();
    kmain();
}

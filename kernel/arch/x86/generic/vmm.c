#include <stdint.h>
#include <stddef.h>
#include <memory.h>
#include <string.h>
#include <logging.h>
#include <cedille.h>
#include <arch/x86/paging.h>
#include <arch/x86/descriptor_tables.h>
#include <stdio.h>
extern uint32_t _kernel_end;
//Its a pointer
page_directory_t * kernel_page_directory = NULL;
page_directory_t * current_page_directory = NULL;

uintptr_t mman_get_placement_address();
///Identity Maps start_addr to end_addr
void vmm_identity_map(uint32_t start_addr, uint32_t end_addr)
{
	if(start_addr > end_addr)
	{
		oops("VMM indentity map OOB");
	}
	uint32_t addr = start_addr;
	addr &= 0xFFFFF000;
	while(addr < end_addr)
	{
		pmm_alloc_frame(pmm_get_page(kernel_page_directory,addr, 1), 1,1);
		addr += 0x1000;
	}
	asm("hlt");
}
///Sets page directory
void vmm_set_page_directory(page_directory_t *d)
{
	current_page_directory = d;
	asm volatile("mov %0, %%cr3":: "r"(&d->tablesPhysical));
}
///Enables paging
void vmm_enable_paging()
{
	uint32_t cr0; //Its the CR0 register, you dumbo
	asm volatile("mov %%cr0, %0": "=r"(cr0));
	cr0 |= 0x80000000; //Sets the page enable bit in cr0
	asm volatile("mov %0, %%cr0":: "r"(cr0));
}
///Called on page fault to get extra information
void vmm_page_fault_exception(struct regs *regs)
{
	uint32_t faulting_address;
	asm volatile("mov %%cr2, %0" : "=r" (faulting_address));
	// The error code gives us details of what happened.
	int present   = !(regs->err_code & 0x1); 	// Page not present
	int rw = regs->err_code & 0x2;           	// Write operation?
	int us = regs->err_code & 0x4;           	// Processor was in user-mode?
	int reserved = regs->err_code & 0x8;     	// Overwritten CPU-reserved bits of page entry?
	int id = regs->err_code & 0x10;          	// Caused by an instruction fetch?
	printk("fault","Page fault at 0x%x ( ",faulting_address);
	if (rw) {printf("a write; ");}
	if (!rw) {printf("a read; ");}
	if (present) {printf("present ");}
	if (us) {printf("user-mode ");}
	if (reserved) {printf("reserved ");}
	if (id) {printf("instruction fetch ");}
	if (faulting_address < (uint32_t)&_kernel_end) {printf("[in kernel] ");}
	printf(")\n");
}
/// 4kb's of swap space
uintptr_t * kernel_debug_zone;
/// Initialises the VMM, on the x86 side.
void init_vmm()
{
	kernel_page_directory = (page_directory_t *)kmalloc_aligned(sizeof(page_directory_t));
	memset(kernel_page_directory, 0, sizeof(page_directory_t));
	current_page_directory = kernel_page_directory;
	vmm_identity_map(0x0,mman_get_placement_address() + 0x1000);
	register_interrupt_handler (14, vmm_page_fault_exception); //Setup Page Fault Handler
	vmm_set_page_directory(kernel_page_directory);
	vmm_enable_paging();
	//Use the new heap to allocate the kernel debug zone. Serves as scrach
	kernel_debug_zone = (uintptr_t *)lheap_alloc_pages(1);
}

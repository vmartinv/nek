#include <stdint.h>
#include <logging.h>
#include <string.h>
#include <arch/x86/descriptor_tables.h>

struct gdt_entry gdt_entries[6];
struct gdt_ptr   gdt_ptr;
struct idt_entry idt_entries[256];
struct idt_ptr   idt_ptr;
tss_entry_t tss_entry;

extern void tss_flush();

///Sets a gate in the GDT
void gdt_set_gate(signed int num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran)
{
	gdt_entries[num].base_low    = (base & 0xFFFF);
	gdt_entries[num].base_middle = (base >> 16) & 0xFF;
	gdt_entries[num].base_high   = (base >> 24) & 0xFF;

	gdt_entries[num].limit_low   = (limit & 0xFFFF);
	gdt_entries[num].granularity = (limit >> 16) & 0x0F;

	gdt_entries[num].granularity |= gran & 0xF0;
	gdt_entries[num].access      = access;
} 
///Sets a gate in the IDT
void idt_set_gate(unsigned char num, unsigned long base, unsigned short sel, unsigned char flags)
{
    idt_entries[num].base_lo = (base & 0xFFFF);
    idt_entries[num].base_hi = (base >> 16) & 0xFFFF;

    idt_entries[num].sel = sel;
    idt_entries[num].always0 = 0;
    idt_entries[num].flags = flags;
}

static void write_tss(int32_t num, uint32_t ss0, uint32_t esp0)
{
	uint32_t base = (uint32_t) &tss_entry;
	uint32_t limit = base + sizeof(tss_entry);
	
	gdt_set_gate(num, base, limit, 0xE9, 0x00);
	
	memset(&tss_entry, 0, sizeof(tss_entry));
	
	tss_entry.ss0  = ss0;
	tss_entry.esp0 = esp0;
	
	tss_entry.cs   = 0x0b;
	tss_entry.ss = tss_entry.ds = tss_entry.es = tss_entry.fs = tss_entry.gs = 0x13;
}

///Inits interrupt services
void idt_init_isrs()
{
	idt_set_gate(0, (unsigned)isr0, 0x08, 0x8E);
	idt_set_gate(1, (unsigned)isr1, 0x08, 0x8E);
	idt_set_gate(2, (unsigned)isr2, 0x08, 0x8E);
	idt_set_gate(3, (unsigned)isr3, 0x08, 0x8E);
	idt_set_gate(4, (unsigned)isr4, 0x08, 0x8E);
	idt_set_gate(5, (unsigned)isr5, 0x08, 0x8E);
	idt_set_gate(6, (unsigned)isr6, 0x08, 0x8E);
	idt_set_gate(7, (unsigned)isr7, 0x08, 0x8E);

	idt_set_gate(8, (unsigned)isr8, 0x08, 0x8E);
	idt_set_gate(9, (unsigned)isr9, 0x08, 0x8E);
	idt_set_gate(10, (unsigned)isr10, 0x08, 0x8E);
	idt_set_gate(11, (unsigned)isr11, 0x08, 0x8E);
	idt_set_gate(12, (unsigned)isr12, 0x08, 0x8E);
	idt_set_gate(13, (unsigned)isr13, 0x08, 0x8E);
	idt_set_gate(14, (unsigned)isr14, 0x08, 0x8E);
	idt_set_gate(15, (unsigned)isr15, 0x08, 0x8E);

	idt_set_gate(16, (unsigned)isr16, 0x08, 0x8E);
	idt_set_gate(17, (unsigned)isr17, 0x08, 0x8E);
	idt_set_gate(18, (unsigned)isr18, 0x08, 0x8E);
	idt_set_gate(19, (unsigned)isr19, 0x08, 0x8E);
	idt_set_gate(20, (unsigned)isr20, 0x08, 0x8E);
	idt_set_gate(21, (unsigned)isr21, 0x08, 0x8E);
	idt_set_gate(22, (unsigned)isr22, 0x08, 0x8E);
	idt_set_gate(23, (unsigned)isr23, 0x08, 0x8E);

	idt_set_gate(24, (unsigned)isr24, 0x08, 0x8E);
	idt_set_gate(25, (unsigned)isr25, 0x08, 0x8E);
	idt_set_gate(26, (unsigned)isr26, 0x08, 0x8E);
	idt_set_gate(27, (unsigned)isr27, 0x08, 0x8E);
	idt_set_gate(28, (unsigned)isr28, 0x08, 0x8E);
	idt_set_gate(29, (unsigned)isr29, 0x08, 0x8E);
	idt_set_gate(30, (unsigned)isr30, 0x08, 0x8E);
	idt_set_gate(31, (unsigned)isr31, 0x08, 0x8E);
	//Syscall
	idt_set_gate(0x64, (unsigned)isr100, 0x08, 0x8E);
}

void set_kernel_stack(uint32_t stack)
{
	tss_entry.esp0 = stack;
}

///Installs all irqs
void irq_install();
///Inits all descriptor tables.
uint32_t x86_init_descriptor_tables() ///Returns how many tables were initialised on error, otherwise 0.
{
	gdt_ptr.limit = (sizeof(struct gdt_entry) * 6) - 1;
	gdt_ptr.base  = (uint32_t)&gdt_entries;

	gdt_set_gate(0, 0, 0, 0, 0);                // Null segment
	gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); // Code segment
	gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF); // Data segment
	gdt_set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF); // User mode code segment
	gdt_set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF); // User mode data segment
	write_tss(5, 0x10, 0x0);
	gdt_flush((uint32_t)&gdt_ptr);
	
	tss_flush();
	idt_ptr.limit = sizeof(struct idt_entry) * 256 -1;
   	idt_ptr.base  = (uint32_t)&idt_entries;

	memset(&idt_entries, 0, sizeof(struct idt_entry)*256);

	idt_init_isrs();
	idt_flush();
	irq_install();
	return 0;
}
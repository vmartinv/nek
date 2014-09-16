#include <arch/x86/descriptor_tables.h>
#include <arch/x86/ports.h>
#include <stdint.h>
#include <logging.h>
#include <cedille.h>
///Table of all exception messages
const char *exception_messages[] =
{
	"Division By Zero",
	"Debug",
	"Non Maskable Interrupt",
	"Breakpoint",
	"Into Detected Overflow",
	"Out of Bounds",
	"Invalid Opcode",
	"No Coprocessor",

	"Double Fault",
	"Coprocessor Segment Overrun",
	"Bad TSS",
	"Segment Not Present",
	"Stack Fault",
	"General Protection Fault",
	"Page Fault",
	"Unknown Interrupt",

	"Coprocessor Fault",
	"Alignment Check",
	"Machine Check",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",

	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved"
};
/// Function pointer array of interrupt handlers
interrupt_handler_t interrupt_handlers [256];

/*
	unsigned int gs, fs, es, ds;      // pushed the segs last
	unsigned int edi, esi, ebp, useless_value, ebx, edx, ecx, eax;  // pushed by pusha. useless value is esp 
	unsigned int int_no, err_code;    // our 'push byte #' and ecodes do this
	unsigned int eip, cs, eflags, useresp, ss; //pushed by the processor automatically
*/
//Handles interrupts
extern void fault_handler(struct regs *r)
{
	if (interrupt_handlers[r->int_no] != 0)
		interrupt_handlers[r->int_no] (r);
	if(r->int_no < 32)
	{
		printk("fault","code     | %d (error %d),(%s)\n",r->int_no,r->err_code,exception_messages[r->int_no]);
		printk("fault","segment  | gs:0x%x fs:0x%x es:0x%x ds:0x%x cs:0x%x ss:0x%x\n",r->gs,r->fs,r->es,r->ds,r->cs,r->ss);
		printk("fault","stack    | esp: 0x%x ebp: 0x%x uesp: 0x%x\n",r->useless_value,r->ebp,r->useresp);
		printk("fault","gp regs  | eax: 0x%x ebx: 0x%x ecx: 0x%x edx: 0x%x\n",r->eax,r->ebx,r->ecx,r->edx);
		printk("fault",".......  | esi: 0x%x edi: 0x%x eip: 0x%x eflags: 0x%x \n",r->esi,r->edi,r->eip,r->eflags);
		panic("Interrupt fault");
	}
}

/** This installs a custom IRQ handler for the given IRQ **/
void register_interrupt_handler (uint8_t n, interrupt_handler_t h)
{
	interrupt_handlers [n] = h;
	
}

/** This clears the handler for a given IRQ **/
void deregister_interrupt_handler (uint8_t n)
{
	interrupt_handlers [n] = 0;
}
///Remaps the irq's in the PIC
void irq_remap(void)
{
	outb(0x20, 0x11);
	outb(0xA0, 0x11);
	outb(0x21, 0x20);
	outb(0xA1, 0x28);
	outb(0x21, 0x04);
	outb(0xA1, 0x02);
	outb(0x21, 0x01);
	outb(0xA1, 0x01);
	outb(0x21, 0x0);
	outb(0xA1, 0x0);
}

void irq_install()
{
	irq_remap();

	idt_set_gate(32, (unsigned)irq0, 0x08, 0x8E);
	idt_set_gate(33, (unsigned)irq1, 0x08, 0x8E);
	idt_set_gate(34, (unsigned)irq2, 0x08, 0x8E);
	idt_set_gate(35, (unsigned)irq3, 0x08, 0x8E);
	idt_set_gate(36, (unsigned)irq4, 0x08, 0x8E);
	idt_set_gate(37, (unsigned)irq5, 0x08, 0x8E);
	idt_set_gate(38, (unsigned)irq6, 0x08, 0x8E);
	idt_set_gate(39, (unsigned)irq7, 0x08, 0x8E);
	idt_set_gate(40, (unsigned)irq8, 0x08, 0x8E);
	idt_set_gate(41, (unsigned)irq9, 0x08, 0x8E);
	idt_set_gate(42, (unsigned)irq10, 0x08, 0x8E);
	idt_set_gate(43, (unsigned)irq11, 0x08, 0x8E);
	idt_set_gate(44, (unsigned)irq12, 0x08, 0x8E);
	idt_set_gate(45, (unsigned)irq13, 0x08, 0x8E);
	idt_set_gate(46, (unsigned)irq14, 0x08, 0x8E);
	idt_set_gate(47, (unsigned)irq15, 0x08, 0x8E);
	return;
}
///Handles IRQ's
void irq_handler(struct regs *r)
{
	/* This is a blank function pointer */
	if (interrupt_handlers[r->int_no] != 0){
		interrupt_handlers[r->int_no] (r);
	}
	else {
		printk("warn","Recieved unhandled interrupt %d\n",r->int_no);
	}
	/* If the IDT entry that was invoked was greater than 40
	*  (meaning IRQ8 - 15), then we need to send an EOI to
	*  the slave controller */
	if (r->int_no >= 40) {
		outb(0xA0, 0x20);
	}

	/* In either case, we need to send an EOI to the master
	*  interrupt controller too */
	outb(0x20, 0x20);
}

#include <stddef.h>
#include <stdint.h>
#include <arch/arm/interrupts.h>
#include <stdio.h>
#include <logging.h>
#include <cedille.h>
void interrupt_handler(uint32_t lr, uint32_t type);
void __attribute__((naked)) k_exphandler_irq_entry() { KEXP_TOP3; interrupt_handler(lr, ARM4_XRQ_IRQ); KEXP_BOT3; }
void __attribute__((naked)) k_exphandler_fiq_entry() { KEXP_TOP3;  interrupt_handler(lr, ARM4_XRQ_FIQ); KEXP_BOT3; }
void __attribute__((naked)) k_exphandler_reset_entry() { KEXP_TOP3; interrupt_handler(lr, ARM4_XRQ_RESET); KEXP_BOT3; }
void __attribute__((naked)) k_exphandler_undef_entry() { KEXP_TOP3; interrupt_handler(lr, ARM4_XRQ_UNDEF); KEXP_BOT3; }	
void __attribute__((naked)) k_exphandler_abrtp_entry() { KEXP_TOP3; interrupt_handler(lr, ARM4_XRQ_ABRTP); KEXP_BOT3; }
void __attribute__((naked)) k_exphandler_abrtd_entry() { KEXP_TOP3; interrupt_handler(lr, ARM4_XRQ_ABRTD); KEXP_BOT3; }
void __attribute__((naked)) k_exphandler_swi_entry() { KEXP_TOPSWI;   interrupt_handler(lr, ARM4_XRQ_SWINT); KEXP_BOT3; }

uint32_t arm4_cpsrget()
{
    uint32_t r;
    
    asm("mrs %[ps], cpsr" : [ps]"=r" (r));
    return r;
}

void arm4_cpsrset(uint32_t r)
{
    asm("msr cpsr, %[ps]" : : [ps]"r" (r));
}

void arm4_xrqenable_fiq()
{
    arm4_cpsrset(arm4_cpsrget() & ~(1 << 6));
}

void arm4_xrqenable_irq()
{
    arm4_cpsrset(arm4_cpsrget() & ~(1 << 7));
}

void arm_xrqinstall(uint32_t ndx, void *addr)
{
	uint32_t *v;
	v = (uint32_t*)0x0;
	v[ndx] = 0xEA000000 | (((uintptr_t)addr - 8 - (4 * ndx)) >> 2);
}

void arm_init_interrupts()
{
	arm_xrqinstall(ARM4_XRQ_RESET, &k_exphandler_reset_entry);
	arm_xrqinstall(ARM4_XRQ_UNDEF, &k_exphandler_undef_entry);
	arm_xrqinstall(ARM4_XRQ_SWINT, &k_exphandler_swi_entry);
	arm_xrqinstall(ARM4_XRQ_ABRTP, &k_exphandler_abrtp_entry);
	arm_xrqinstall(ARM4_XRQ_ABRTD, &k_exphandler_abrtd_entry);
	arm_xrqinstall(ARM4_XRQ_IRQ, &k_exphandler_irq_entry);
	arm_xrqinstall(ARM4_XRQ_FIQ, &k_exphandler_fiq_entry);
	arm4_cpsrset(arm4_cpsrget() & ~(1 << 7)); //Enable interrupts
}
void interrupt_handler(uint32_t lr, uint32_t type)
{
	uint32_t *t0mmio = (uint32_t*)0x13000000;
	uint32_t swi = 0xFFFFFFFF;
	t0mmio[0x03] = 1; //Clear interrupts in timer
	
	switch(type) //Lets handle it
	{
		case ARM4_XRQ_IRQ:
			break;
		case ARM4_XRQ_FIQ:
			break;
		case ARM4_XRQ_SWINT:
			swi = ((uint32_t*)((uintptr_t)lr - 4))[0] & 0xffff;
			printf("SWI:%d\n",swi);
			break;
		case ARM4_XRQ_UNDEF :
		case ARM4_XRQ_ABRTP :
		case ARM4_XRQ_ABRTD :
		case ARM4_XRQ_RESV1 :
			printk("error","Unhandled exception!\n");
			panic("Unhandled exception!");
			break;
		default:
			break;
	}
	return;
}

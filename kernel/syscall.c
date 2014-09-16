#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <logging.h>
#include <stdio.h>
#include <cedille.h>
#include <syscall.h>
#include <arch/x86/descriptor_tables.h>

static void *syscall_handlers[MAX_SYSCALLS];

void register_syscall_handler (uint8_t n, syscall_handler_t h)
{
	syscall_handlers[n] = h;
	
}

void deregister_syscall_handler (uint8_t n)
{
	syscall_handlers[n] = 0;
}

void syscall_handler(registers_t *regs)
{
	if(regs->eax > MAX_SYSCALLS)
	{
		oops("Syscall out of bounds!");
	}
	void *location = syscall_handlers[regs->eax];
	if(location == NULL)
	{
		oops("Called a unregistered syscall!");
		return;
	}
	#ifdef BOARDx86generic
	int ret;
	asm volatile (" \
		push %1; \
		push %2; \
		push %3; \
		push %4; \
		push %5; \
		call *%6; \
		pop %%ebx; \
		pop %%ebx; \
		pop %%ebx; \
		pop %%ebx; \
		pop %%ebx; \
	" : "=a" (ret) : "r" (regs->edi), "r" (regs->esi), "r" (regs->edx), "r" (regs->ecx), "r" (regs->ebx), "r" (location));
	regs->eax = ret;
	#endif
}

int do_syscall(uint32_t no)
{
	#ifdef BOARDx86generic
	int a;
	asm volatile("int $0x64" : "=a" (a) : "0" (no));
	return a;
	#else
	return 0;
	#endif
}
int do_syscall_p1(uint32_t no, void* param)
{
	#ifdef BOARDx86generic
	int a;
	asm volatile("int $0x64" : "=a" (a) : "0" (no), "b" ((uint32_t)param));
	return a;
	#else
	return 0;
	#endif
}
int do_syscall_p2(uint32_t no, void* param,void* param2)
{
	#ifdef BOARDx86generic
	int a;
	asm volatile("int $0x64" : "=a" (a) : "0" (no), "b" ((uint32_t)param), "c" ((uint32_t)param2));
	return a;
	#else
	return 0;
	#endif

}
void init_syscall();
int init_syscalls()
{
	#ifdef BOARDx86generic
	register_interrupt_handler (0x64, &syscall_handler); //Setup Syscall Handler
	#endif
	init_syscall();
	return 0;
}
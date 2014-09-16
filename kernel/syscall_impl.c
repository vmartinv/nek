#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <logging.h>
#include <stdio.h>
#include <cedille.h>
#include <syscall.h>
int syscall_panic(char * s)
{
	panic(s);
	return 0;
}

int syscall_oops(char * s)
{
	oops(s);
	return 0;
}

void init_syscall()
{
	register_syscall_handler (SYSCALL_PANIC, (syscall_handler_t)&syscall_panic);
	register_syscall_handler (SYSCALL_OOPS, (syscall_handler_t)&syscall_oops);
}
#include <stdint.h>
#include <stddef.h>
#include <arch/sparc/openprom.h>
#include <arch/sparc/serial.h>
#include <console.h>
prom_vec_t * prom_vec = NULL;
void printc(unsigned char c)
{
	serial_write(c);
}
char * test = "Hello World!\n";
void kernel_entry(prom_vec_t * pv)
{
	if(pv->pv_magic_cookie != ROMVEC_MAGIC)
	{
		asm("mov 0xFF, %g7");
		return;
	}
	prom_vec = pv;
	printk("ok","Branch: sparc/v8\n");
}
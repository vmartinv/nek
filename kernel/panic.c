#include <stdint.h>
#include <stddef.h>
#include <logging.h>
char * panic_str = NULL;
char * oops_str = NULL;
uint8_t oops_ctr = 0;
void panic(char *s)
{
	panic_str = s;
	printk("panic","Kernel Panic - %s\n",s);
	#ifdef BOARDx86generic
	asm("cli");
	asm("sti");
	#endif
	for(;;)
	{

	}
}

void oops(char *s)
{
	panic_str = s;
	printk("oops","Kernel Oops - %s\n",s);
	oops_ctr++;
	if(oops_ctr == 3)
	{
		panic("Too many oops!\n");
	}
}
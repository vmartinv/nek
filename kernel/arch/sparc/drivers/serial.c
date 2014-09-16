#include <arch/sparc/openprom.h>
extern prom_vec_t * prom_vec;

void serial_write (char c)
{
	prom_vec->pv_nbputchar(c);
}

void serial_writes(const char * s)
{
	while (*s)
	{
		serial_write(*s);
		s++;
	};
}

#define SERIAL_BASE 0x16000000
#define SERIAL_FLAG_REGISTER 0x18
#define SERIAL_BUFFER_FULL (1 << 5)

void serial_write (char c)
{
    /* Wait until the serial buffer is empty */
    while (*(volatile unsigned long*)(SERIAL_BASE + SERIAL_FLAG_REGISTER) 
                                       & (SERIAL_BUFFER_FULL));
    *(volatile unsigned long*)SERIAL_BASE = c;
	if(c == '\n')
	{
		serial_write ('\r');
	}
}

void serial_writes(char * s)
{
	while (*s) {serial_write(*s); s++;};
}

void serial_init()
{
	
}
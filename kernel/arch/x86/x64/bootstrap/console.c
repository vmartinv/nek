#include <stdint.h>
#include <string.h>

volatile uint16_t *video_memory=(uint16_t*)0xB8000;

uint8_t attributeByte = (0 << 4) | (15  & 0x0F);

void textmode_write(int x,int y,uint8_t data)
{
	uint16_t attribute = attributeByte << 8;
	volatile uint16_t *write_to;
	write_to = video_memory + (y * 80) + x;
	*write_to = data | attribute;
}

inline void outb(uint16_t port, uint8_t value)
{
	asm volatile ("outb %1, %0" : : "dN" (port), "a" (value));
}

inline uint8_t inb(uint16_t port)
{
	uint8_t ret;
	asm volatile("inb %1, %0" : "=a" (ret) : "dN" (port));
	return ret;
}

int term_x, term_y;

void bs_init_console()
{
	unsigned short offset;
	outb(0x3D4, 14);
	offset = inb(0x3D5) << 8;
	outb(0x3D4, 15);
	offset |= inb(0x3D5);
	term_x=offset%80;
	term_y=offset/80;
}

void textmode_setcursor(int x,int y)
{
	uint16_t location = y * 80 + x;
	outb(0x3D4, 14);
	outb(0x3D5, location >> 8);
	outb(0x3D4, 15);
	outb(0x3D5, location);
}

void printc(unsigned char c)
{
	if (c == 0x09) //Tab
	{
		term_x = (term_x+8) & ~(8-1);
	}
	else if (c == '\n') //Newline
	{
		term_x = 0;
		term_y++;
	}
	else if(c >= ' ') //Anything else
	{
		textmode_write(term_x,term_y,c);
		term_x++;
	}

	if (term_x >= 80)
	{
		term_x = 0;
		term_y ++;
	}

	// Move the hardware cursor.
	textmode_setcursor(term_x,term_y);
}
void print(const char *c)
{
	int i = 0;
	while (c[i])
	{
		printc(c[i++]);
	}
}
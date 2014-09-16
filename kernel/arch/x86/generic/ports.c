#include <arch/x86/ports.h>
#include <stdint.h>
///Outputs a byte to port
void outb(uint16_t port, uint8_t value)
{
	asm volatile ("outb %1, %0" : : "dN" (port), "a" (value));
}
///Outputs a word to port
void outw(uint16_t port, uint16_t value)
{
	asm volatile ("outw %1, %0" : : "dN" (port), "a" (value));
}
///Outputs a long to port
void outl(uint16_t port, uint32_t value)
{
	asm volatile ("outl %1, %0" : : "dN" (port), "a" (value));
}
///Inputs a byte from port
uint8_t inb(uint16_t port)
{
	uint8_t ret;
	asm volatile("inb %1, %0" : "=a" (ret) : "dN" (port));
	return ret;
}
///Inputs a word from port
uint16_t inw(uint16_t port)
{
	uint16_t ret;
	asm volatile("inw %w1, %0" : "=a" (ret) : "dN" (port));
	return ret;
}
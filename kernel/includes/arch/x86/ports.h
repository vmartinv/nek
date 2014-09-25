#ifndef ARCH_X86_PORTS_H
#define ARCH_X86_PORTS_H
#include <stdint.h>
///Exports a byte
void outb(uint16_t port, uint8_t value);
///Exports a word
void outw(uint16_t port, uint16_t value);
///Exports a long
void outl(uint16_t port, uint32_t value);
///Imports a byte
uint8_t inb(uint16_t port);
///Imports a word
uint16_t inw(uint16_t port);
///Imports signed words
void insw(uint16_t port, void *addr, unsigned long count);
///Imports a long
uint32_t inl(uint16_t port);



#define PIC1		0x20
#define PIC2		0xA0
#define PIC1_COMMAND	PIC1
#define PIC1_DATA	(PIC1+1)
#define PIC2_COMMAND	PIC2
#define PIC2_DATA	(PIC2+1)
#define PIC_EOI		0x20

#define ICW1_ICW4	0x01		/* ICW4 (not) needed */
#define ICW1_SINGLE	0x02		/* Single (cascade) mode */
#define ICW1_INTERVAL4	0x04		/* Call address interval 4 (8) */
#define ICW1_LEVEL	0x08		/* Level triggered (edge) mode */
#define ICW1_INIT	0x10		/* Initialization - required! */

#define ICW4_8086	0x01		/* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO	0x02		/* Auto (normal) EOI */
#define ICW4_BUF_SLAVE	0x08		/* Buffered mode/slave */
#define ICW4_BUF_MASTER	0x0C		/* Buffered mode/master */
#define ICW4_SFNM	0x10		/* Special fully nested (not) */


#endif

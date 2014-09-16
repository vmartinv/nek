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
///Imports a long
uint32_t inl(uint16_t port);
#endif

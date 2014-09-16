#ifndef CEDILLE_LOGGING_H
#define CEDILLE_LOGGING_H
#include <stddef.h>
#include <stdint.h>
int printk(const char *title, const char *fmt, ...);
void logging_printbestunit(uint32_t bytes, uint8_t newline);
#endif
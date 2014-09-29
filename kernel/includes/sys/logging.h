#ifndef LOGGING_H
#define LOGGING_H
#include <types.h>
int printk(const char *title, const char *fmt, ...);
void logging_printbestunit(uint32_t bytes, uint8_t newline);
#endif

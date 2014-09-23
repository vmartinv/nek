#ifndef STDIO_H
#define STDIO_H

#include <types.h>
#include <stdarg.h>

int vsprintf(char *buf, const char *fmt, va_list args);
int sprintf(char *buf, const char *fmt, ...);
int vprintf(const char *fmt, va_list args);
int printf(const char *fmt, ...);
void console_clear();

uint8_t getchar();
bool iskeydown(int scancode);
bool iskeyup(int scancode);

#endif

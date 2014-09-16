#ifndef LIB_STRING_H
#define LIB_STRING_H

#include <stddef.h>
#include <stdint.h>

int strcmp (const char *str1,const char *str2);
size_t strlen(const char *str);
char * strcpy(char * dest, const char * src);

void *memcpy(void *dest,const void *src,size_t n);
void *memset(void *dest,int val,size_t n);
void memmove(void *dest, const void *src, size_t n);

char *strdup(char *s1);
char * strpbrk(const char * str, const char * accept);
size_t lfind(const char * str, const char accept);
size_t strspn(const char * str, const char * accept);
char * strtok_r(char * str, const char * delim, char ** saveptr);

#endif
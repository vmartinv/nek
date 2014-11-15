#ifndef LIB_STRING_H
#define LIB_STRING_H

#include <types.h>
#ifdef __cplusplus
extern "C" {
#endif
char* strtok(char *s, const char *delim);
char* strchr(const char *s, int c);
char* strsep(char **stringp, const char *delim);
int strcasecmp(const char *s1, const char *s2);
int strncasecmp(const char *s1, const char *s2, size_t n);

int memcmp(const void* ptr1, const void* ptr2, size_t num);
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

#ifdef __cplusplus
}
#endif
#endif

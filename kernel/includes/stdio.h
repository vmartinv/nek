#ifndef STDIO_H
#define STDIO_H

#include <types.h>
#include <stdarg.h>
#include <fs/vfs.h>
#ifdef __cplusplus
extern "C" {
#endif
int vsprintf(char *buf, const char *fmt, va_list args);
int sprintf(char *buf, const char *fmt, ...);
int vprintf(const char *fmt, va_list args);
int printf(const char *fmt, ...);
void console_clear();

uint8_t getchar();
int wait_scancode();
#define RETURN_KEY 28
#define DOWN_ARROY_KEY 80
#define UP_ARROW_KEY 72
#define ESC_KEY 1

bool kbd_iskeydown(int scancode);
bool kbd_iskeyup(int scancode);
void kbd_clear_buffer();

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END -1

typedef struct file file_t;
typedef struct dir dir_t;
typedef struct dirent dirent_t;
#define FILE file_t
#define DIR dir_t
dir_t* opendir(const char *path);
dirent_t* readdir(dir_t *d);
void closedir(dir_t*d);

FILE* fopen(const char *path, const char *mode);
void fclose(FILE* f);
long int fread(void *ptr, size_t size, size_t count, FILE *f);
void fseek(FILE *f, long int offset, int whence);
long int ftell(FILE *f);
char fgetc(FILE *f);

#ifdef __cplusplus
}
#endif
#endif

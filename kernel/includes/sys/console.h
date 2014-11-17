#ifndef CONSOLE_H
#define CONSOLE_H

#ifdef __cplusplus
extern "C" {
#endif

void putchar(unsigned char c);
void console_init();
void console_printdiv();
void console_move_write_cursor(int x, int y);


#ifdef __cplusplus
}
#endif

#endif

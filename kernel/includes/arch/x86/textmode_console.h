#ifndef ARCH_X86_TEXTMODE_CONSOLE_H
#define ARCH_X86_TEXTMODE_CONSOLE_H
#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif
void textmode_init(uint16_t *buffer, unsigned alines, unsigned acols);

///Write a char to the screen
void textmode_write(int x,int y,uint8_t data);
///Write a colored char to the screen
void textmode_write_color(int x,int y,uint8_t data, uint8_t attr);
///Read a char from the screen
uint8_t textmode_read(int x,int y);
///Scroll
void textmode_scroll(int from,int to);
///Set cursor position
void textmode_setcursor(int x,int y);
///Set attribute byte
void textmode_setbackcolor(uint8_t back);
void textmode_setforecolor(uint8_t fore);
///Reset attribute byte
void textmode_resetcolor();
void textmode_changedest(uint16_t *newdest);
void textmode_clear();

#ifdef __cplusplus
}
#endif

#endif

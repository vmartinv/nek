#ifndef VIDEO_H
#define VIDEO_H

#include <stdint.h>
void video_init();
///Inits graphics
int video_graphics_init();
///Prints a char
void video_printchar(int x,int y, unsigned char c);
///Prints a char w/ attribute byte
void video_printcoloredchar(int x,int y, unsigned char c, unsigned char attribute);
///Scrolls
void video_scroll(int from,int to);
///Set's cursor to x and y
void video_setcursor(int x,int y);
///Set current attribute to back & fire
void video_setattributetext(unsigned char back, unsigned char fore);
///Resets current color attribute
void video_resetattributetext();
///Can we use graphics?
int video_graphics_capable();
void video_draw_pixel(int x, int y, uint8_t r,uint8_t g,uint8_t b);

void video_printstring(int x,int y, char *c);
void video_printcoloredstring(int x,int y,unsigned char attribute, char *c);
void video_clear();
void video_settextfore(uint8_t color);
void video_reset_attr();
#endif
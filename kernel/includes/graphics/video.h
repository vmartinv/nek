#ifndef VIDEO_H
#define VIDEO_H

#include <types.h>
#define NES_WIDTH 256
#define NES_HEIGHT 240

struct svga_mode_info;
void video_load_info(struct svga_mode_info *svga_mode_info);
void video_init();
void video_updatepixel_raw(int y,int x,u32 c);
int video_getwidth();
int video_getheight();
int video_getbpp();

#define RGB(r,g,b) (((uint8_t)r)<<16|((uint8_t)g)<<8|((uint8_t)b))

//NES Display
void video_updatepixel(int y,int x,u8 s);
void video_show_frame();//Call to draw the buffer

//Consola
u16 *video_get_text_buffer();
unsigned video_get_lines();
unsigned video_get_cols();
void video_show_console();

#endif

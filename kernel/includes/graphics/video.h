#ifndef VIDEO_H
#define VIDEO_H

#include <types.h>
#define NES_WIDTH 256
#define NES_HEIGHT 240

struct svga_mode_info;
void video_init(struct svga_mode_info *svga_mode_info);

//NES Display
void video_updatepixel(int line,int pixel,u8 s);
void video_dump_frame();//Call to draw the buffer

//Consola
u16 *vga_get_text_buffer();
void video_dump_console();


#endif

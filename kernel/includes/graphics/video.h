#ifndef VIDEO_H
#define VIDEO_H

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif
 
 
#define NES_WIDTH 256
#define NES_HEIGHT 240

struct svga_mode_info;
void video_load_info(struct svga_mode_info *svga_mode_info);
void video_init();
void video_updatepixel_raw(int y,int x,u32 c);
int video_getwidth();
int video_getheight();
int video_getbpp();
void video_draw(int px, int py, const char *data, const int width, const int height);

#define RGB(r,g,b) (((uint8_t)r)<<16|((uint8_t)g)<<8|((uint8_t)b))

//NES Display
void video_updatepixel(int x,int y,u32 color);
void video_flush_frame();//Call to draw the buffer
void video_flush_scanline(int y);//Call to draw the buffer

//Consola
u16 *video_get_text_buffer();
unsigned video_get_lines();
unsigned video_get_cols();
void video_flush_console();
void video_flush_char(int x, int y);

#ifdef __cplusplus
}
#endif

#endif

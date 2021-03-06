#ifndef VIDEO_H
#define VIDEO_H

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif
 
 
#define NES_WIDTH 256
#define NES_HEIGHT 240
struct svga_mode_info;
void video_init(struct svga_mode_info *svga_mode_info);
void video_updatepixel_raw(int y,int x,u32 c);
extern u32 video_width, video_height, video_depth, video_bpl;

int video_getwidth();
int video_getheight();
int video_getbpp();
void video_draw(int px, int py, const char *data, const int width, const int height);

#define RGB(r,g,b) (((uint8_t)r)<<16|((uint8_t)g)<<8|((uint8_t)b))

//For NES Emulator
u8 *video_get_ptr();
extern void (*setpixel)(uint8_t *pos, uint32_t color);

//Consola
u16 *video_get_text_buffer();
unsigned video_get_lines();
unsigned video_get_cols();
void video_flush_console();
void video_flush_console2(int start);

#ifdef __cplusplus
}
#endif

#endif

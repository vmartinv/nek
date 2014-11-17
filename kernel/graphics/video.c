#include <graphics/video.h>
#include <types.h>
#include <string.h>
#include <graphics/svga.h>
#include <stdlib.h>
#include <math.h>//min & max
//~ #include "rsrc/ter-i16b.h"
//~ #include "rsrc/ter-i16n.h"
#include "rsrc/nesfont.h"

#ifndef CONSOLE_ONLY

#define CHAR_HEIGHT 16
#define CHAR_WIDTH 16

static bool initialized=false;

// Text compatibility
static u16 *font;
static u16 text_buffer[80*50];
static unsigned lines, cols;

// Video mode info
u32 video_width=0, video_height=0, video_depth=0, video_bpl=0;
static u8 *screenbase=0;

u16 *video_get_text_buffer(){
	return text_buffer;
}
unsigned video_get_lines(){
	return lines;
}
unsigned video_get_cols(){
	return cols;
}

static void setpixel_16(uint8_t *pos, uint32_t color){
	*(uint16_t*)pos = (SVGA_24TO16BPP(color) & 0xFFFF);
}

static void setpixel_24(uint8_t *pos, uint32_t color){
	//~ *(u32*)pos=color;
    *pos++ = color & 255;           // BLUE
    *pos++ = (color >> 8) & 255;   	// GREEN
    *pos = (color >> 16) & 255;  	// RED
}

static void setpixel_32(uint8_t *pos, uint32_t color){
	*(uint32_t*)pos=color;
}

void (*setpixel)(uint8_t *pos, uint32_t color);



inline static void video_putchar(int textx, int texty, u16 c) {
	if((c&0xFF)>126) return;
	static const u32 consolecolors[]={0x000000, 0x0000c0, 0x00c000, 0x00c0c0, 0xc00000, 0xc000c0, 0xc08000, 0xc0c0c0, 0x808080, 0x0000ff, 0x00ff00, 0x00ffff, 0xff0000, 0xff00ff, 0xffff00, 0xffffff};
	u32 forecolor=consolecolors[(c>>8)&15],backcolor=consolecolors[(c>>12)&15];
	// Handle printing of a regular character
	// Characters are 16 px tall, i.e. 0x10 bytes in stored rep
	uint16_t *ch_ptr = font + ((c-33)&0xFF)*CHAR_HEIGHT;
	uint8_t *write_ptr=screenbase + texty*CHAR_HEIGHT*video_bpl + textx*CHAR_WIDTH*video_depth;
	for(uint8_t y = 0; y < CHAR_HEIGHT; y++) {
		for(uint8_t x = 0; x < CHAR_WIDTH; x++)
			if(1<<(CHAR_WIDTH-x-1) & *ch_ptr)
			//~ if(x_to_bitmap[x] & *ch_ptr)
				setpixel(write_ptr+x*video_depth, forecolor);
			else 
				setpixel(write_ptr+x*video_depth, backcolor);
		write_ptr+=video_bpl;
		ch_ptr++;
	}
}

void video_clear(){
	if(!initialized) return;
	memset(screenbase, 0, video_bpl * video_height);
}



inline static u8 *go_offset(u8 *ptr, int x, int y){
	return ptr+x*video_depth+y*video_bpl;
}


void video_updatepixel_raw(int y,int x,u32 c){
	if(!initialized) return;
	setpixel(go_offset(screenbase, x, y), c);
}

void video_draw(int px, int py, const char *data, const int width, const int height){
	if(px==-1) px=video_width/2-width;
	if(py==-1) px=video_height/2-height;
	u8 *write_ptr=go_offset(screenbase, px, py);
	u32 color;
	for(int y=0; y<2*height; y++){
		for(int x=0; x<width; x++){
			color = ((((data[0] - 33) << 2) | ((data[1] - 33) >> 4))			<< 16)
					  | (((((data[1] - 33) & 0xF) << 4) | ((data[2] - 33) >> 2)) << 8)
				      | (((((data[2] - 33) & 0x3) << 6) | ((data[3] - 33))) 		<< 0);
			data += 4;
			setpixel(write_ptr+2*x*video_depth, color);
			setpixel(write_ptr+(2*x+1)*video_depth, color);
		}
		if(!(y%2)) data-=4*width;
		write_ptr+=video_bpl;
	}
}
static int offsetx, offsety;
u8 *video_get_ptr(){
	return screenbase+offsetx*video_depth+offsety*video_bpl;
}

void video_flush_console(){
	if(!initialized) return;
	//~ int offset=video_width/CHAR_WIDTH>80;
	for(unsigned y=0;y<lines;y++)
		for(unsigned x=0;x<cols;x++)
			video_putchar(x, y, text_buffer[(y*cols)+x]);
}

void video_flush_console2(int start){
	if(!initialized) return;
	//~ int offset=video_width/CHAR_WIDTH>80;
	for(unsigned y=start;y<lines;y++)
		for(unsigned x=0;x<cols;x++)
			video_putchar(x, y, text_buffer[(y*cols)+x]);
}


void video_load_info(svga_mode_info_t *svga_mode_info){
	video_bpl = svga_mode_info->pitch;
	video_width = svga_mode_info->screen_width;
	video_height = svga_mode_info->screen_height;
	video_depth = svga_mode_info->bpp / 8;
	screenbase = (u8*)svga_mode_info->physbase;
	lines=min(video_height/CHAR_HEIGHT, 50);
	cols=min(video_width/CHAR_WIDTH, 80);
}

/*
 * Initialises the framebuffer console
 */
void video_init() {
	switch(video_depth){
	case 2: setpixel=setpixel_16; break;
	case 3: setpixel=setpixel_24; break;
	case 4: setpixel=setpixel_32; break;
	}
	//~ font=ter_i16n_raw;
	font=nesfont_raw;
	//~ nesscreenbase= (u8*)malloc(nesscreensize);
	//~ memset(nesscreenbase, 0, nesscreensize);
	offsety=(video_height-NES_HEIGHT*2)/2;
	offsetx=(video_width-NES_WIDTH*2)/2;
	video_clear();
	initialized=true;
}

int video_getwidth()		{	return(video_width);		}
int video_getheight()		{	return(video_height);		}
int video_getbpp()			{	return(video_depth*8);		}
#else
void video_init(){}
void video_load_info(svga_mode_info_t *svga_mode_info){}
void video_show_frame(){}
void video_show_console(){}
void video_updatepixel(int line,int pixel,u8 s){}
void video_clear(){}
u16 *video_get_text_buffer(){return NULL;}
unsigned video_get_lines(){return 0;}
unsigned video_get_cols(){return 0;}
#endif

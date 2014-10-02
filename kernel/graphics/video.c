#include <graphics/video.h>
#include <types.h>
#include <string.h>
#include <graphics/svga.h>
#include <stdlib.h>
#include <math.h>//min & max
#include <graphics/palette.h>
#include "rsrc/ter-i16b.h"
#include "rsrc/ter-i16n.h"

#ifndef CONSOLE_ONLY

#define CHAR_HEIGHT 16
#define CHAR_WIDTH 8

static bool initialized=false;

// Text compatibility
static u8 *font;
static u16 text_buffer[80*50];
static unsigned lines, cols;

// Video mode info
static u16 screenwidth=0, screenheight=0, screendepth=0, screenbytesPerLine=0;
static const size_t nesscreensize=NES_WIDTH*(NES_HEIGHT+16)*sizeof(u32);
static u8 *screenbase=0;
static u32 *nesscreenbase=0;


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
    *pos++ = color & 255;           // BLUE
    *pos++ = (color >> 8) & 255;   	// GREEN
    *pos = (color >> 16) & 255;  	// RED
}

static void setpixel_32(uint8_t *pos, uint32_t color){
	*(uint32_t*)pos=color;
}

static void (*setpixel)(uint8_t *pos, uint32_t color);



inline static void video_putchar(int textx, int texty, u16 c) {
	static const u32 consolecolors[]={0x000000, 0x0000c0, 0x00c000, 0x00c0c0, 0xc00000, 0xc000c0, 0xc08000, 0xc0c0c0, 0x808080, 0x0000ff, 0x00ff00, 0x00ffff, 0xff0000, 0xff00ff, 0xffff00, 0xffffff};
	u32 forecolor=consolecolors[(c>>8)&15],backcolor=consolecolors[(c>>12)&15];
	// Handle printing of a regular character
	// Characters are 16 px tall, i.e. 0x10 bytes in stored rep
	uint8_t *ch_ptr = font + (c&0xFF)*CHAR_HEIGHT;
	uint8_t *write_ptr=screenbase + texty*CHAR_HEIGHT*screenbytesPerLine + textx*CHAR_WIDTH*screendepth;
	const uint8_t x_to_bitmap[CHAR_WIDTH] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};
	for(uint8_t y = 0; y < CHAR_HEIGHT; y++) {
		for(uint8_t x = 0; x < CHAR_WIDTH; x++)
			if(x_to_bitmap[x] & *ch_ptr)
				setpixel(write_ptr+x*screendepth, forecolor);
			else
				setpixel(write_ptr+x*screendepth, backcolor);
		write_ptr+=screenbytesPerLine;
		ch_ptr++;
	}
}

void video_clear(){
	if(!initialized) return;
	memset(screenbase, 0, screenbytesPerLine * screenheight);
	memset(nesscreenbase, 0, nesscreensize);
}


//this handles pixels coming directly from the nes engine
void video_updatepixel(int line,int pixel,u8 s){
	if(!initialized) return;
	int offset = (line * 256) + pixel;
	if(line >= 8 && line < 232) {
		nesscreenbase[offset] = palette_get_value(s);
	}
	else {
		nesscreenbase[offset] = 0;
	}
}

inline static u8 *go_offset(u8 *ptr, int x, int y){
	return ptr+x*screendepth+y*screenbytesPerLine;
}

void video_updatepixel_raw(int y,int x,u32 c){
	if(!initialized) return;
	setpixel(go_offset(screenbase, x, y), c);
}

int offsetx, offsety;
void video_show_frame(){
	if(!initialized) return;
	u8 *write_ptr=go_offset(screenbase, offsetx, offsety);
	u32 *read_ptr=nesscreenbase;
	for(int y=0;y<NES_HEIGHT*2;y++) {
		for(int x=0;x<NES_WIDTH;x++){
			setpixel(write_ptr+2*x*screendepth, *read_ptr);
			setpixel(write_ptr+(2*x+1)*screendepth, *read_ptr++);
		}
		if(!(y%2)) read_ptr-=NES_WIDTH;
		write_ptr+=screenbytesPerLine;
	}
}


void video_show_console(){
	if(!initialized) return;
	int offset=screenwidth/CHAR_WIDTH>80;
	for(unsigned y=0;y<lines;y++)
		for(unsigned x=0;x<cols;x++)
			video_putchar(x+offset, y, text_buffer[(y*80)+x]);
}


void video_load_info(svga_mode_info_t *svga_mode_info){
	screenbytesPerLine = svga_mode_info->pitch;
	screenwidth = svga_mode_info->screen_width;
	screenheight = svga_mode_info->screen_height;
	screendepth = svga_mode_info->bpp / 8;
	screenbase = (u8*)svga_mode_info->physbase;
	lines=min(screenheight/CHAR_HEIGHT, 50);
	cols=min(screenwidth/CHAR_WIDTH, 80);
}

/*
 * Initialises the framebuffer console
 */
void video_init() {
	switch(screendepth){
	case 2: setpixel=setpixel_16; break;
	case 3: setpixel=setpixel_24; break;
	case 4: setpixel=setpixel_32; break;
	}
	font=ter_i16b_raw;
		
	nesscreenbase= (u32*)malloc(nesscreensize);
	memset(nesscreenbase, 0, nesscreensize);
	offsety=(screenheight-NES_HEIGHT*2)/2;
	offsetx=(screenwidth-NES_WIDTH*2)/2;
	video_clear();
	palette_init();
	initialized=true;
}

int video_getwidth()		{	return(screenwidth);		}
int video_getheight()		{	return(screenheight);		}
int video_getbpp()			{	return(screendepth*8);		}
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

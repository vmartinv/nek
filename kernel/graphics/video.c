#ifndef CONSOLE_TEXTMODE

#include <graphics/video.h>
#include <types.h>
#include <string.h>
#include <graphics/svga.h>
#include <stdlib.h>
#include "rsrc/ter-i16b.h"
#include "rsrc/ter-i16n.h"
#include <graphics/palette.h>

#define CHAR_HEIGHT 16
#define CHAR_WIDTH 8

static bool initialized=false;

// Text compatibility
static u8 *font;
u16 text_buffer[80*25];
static u32 fg_colour;

// Video mode info
static u16 screenwidth, screenheight, screendepth, screenbytesPerLine;
static u8* screenbase;
static u32 *nesscreenbase;


u16 *vga_get_text_buffer(){
	return text_buffer;
}

void setpixel_16(uint8_t *pos, uint32_t color){
	*(uint16_t*)pos = (SVGA_24TO16BPP(color) & 0xFFFF);
}

void setpixel_24(uint8_t *pos, uint32_t color){
    *pos++ = color & 255;           // BLUE
    *pos++ = (color >> 8) & 255;   	// GREEN
    *pos = (color >> 16) & 255;  	// RED
}

void setpixel_32(uint8_t *pos, uint32_t color){
	*(uint32_t*)pos=color;
}
void (*setpixel)(uint8_t *pos, uint32_t color);

void putpixel(int x, int y, uint32_t color){
	setpixel(screenbase + y*screenbytesPerLine + x*screendepth, color);
}

void video_printchar(int textx,int texty, unsigned char c) {
	if(!initialized) return;
	// Handle printing of a regular character
	// Characters are 16 px tall, i.e. 0x10 bytes in stored rep
	uint8_t *ch_ptr = font + c*CHAR_HEIGHT;
	uint8_t *write_ptr=screenbase + texty*CHAR_HEIGHT*screenbytesPerLine + textx*CHAR_WIDTH*screendepth;
	const uint8_t x_to_bitmap[CHAR_WIDTH] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};
	for(uint8_t y = 0; y < CHAR_HEIGHT; y++) {
		for(uint8_t x = 0; x < CHAR_WIDTH; x++)
			if(x_to_bitmap[x] & *ch_ptr)
				setpixel(write_ptr+x*screendepth, fg_colour);
			else
				setpixel(write_ptr+x*screendepth, 0);
		write_ptr+=screenbytesPerLine;
		ch_ptr++;
	}
}

void video_clear(){
	memset(screenbase, 0, screenbytesPerLine * screenheight);
}

//this handles pixels coming directly from the nes engine
void video_updatepixel(int line,int pixel,u8 s){
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

int offsetx, offsety;
void video_dump_frame(){
	u8 *write_ptr=go_offset(screenbase, offsetx, offsety);
	u32 *read_ptr=nesscreenbase;
	for(int y=0;y<NES_HEIGHT;y++) {
		for(int x=0;x<NES_WIDTH;x++)
			setpixel(write_ptr+x*screendepth, *read_ptr++);
		write_ptr+=screenbytesPerLine;
	}
}


void video_dump_console(){
	for(int y=0;y<25;y++)
		for(int x=0;x<80;x++)
			video_printchar(x,y,text_buffer[(y*80)+x]&0xFF);
}

/*
 * Initialises the framebuffer console
 */
void video_init(svga_mode_info_t *svga_mode_info) {
	
	screenbytesPerLine = svga_mode_info->pitch;
	screenwidth = svga_mode_info->screen_width;
	screenheight = svga_mode_info->screen_height;
	screendepth = svga_mode_info->bpp / 8;
	screenbase = (u8*)svga_map_fb(svga_mode_info->physbase, screenbytesPerLine *screenheight);
	
	switch(screendepth){
	case 2: setpixel=setpixel_16; break;
	case 3: setpixel=setpixel_24; break;
	case 4: setpixel=setpixel_32; break;
	}
	font=ter_i16b_raw;
	fg_colour = 0xFFFFFF;
		
	nesscreenbase= (u32*)malloc(NES_WIDTH*(NES_HEIGHT+16)*sizeof(u32));
	for(int y=0; y<NES_HEIGHT; y++)	
		for(int x=0; x<NES_WIDTH; x++)
			video_updatepixel(y, x, GETINDEX_COLOR(13, 0));
	offsety=(screenheight-NES_HEIGHT)/2;
	offsetx=(screenwidth-NES_WIDTH)/2;
	video_clear();
	initialized=true;
	video_dump_console();
}

//~ int video_getwidth()		{	return(screenwidth);		}
//~ int video_getheight()		{	return(screenheight);		}
//~ int video_getbpp()			{	return(screendepth*8);		}

#endif

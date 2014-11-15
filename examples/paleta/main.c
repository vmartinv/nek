#include <stdio.h>
#include <stdlib.h>
#include <graphics/video.h>
#include <ntsc/palette.h>


int main(){
	palette_init();
	video_show_console();
	for(int y=0; y<NES_HEIGHT; y++){
		int paleta=(y/(double)NES_HEIGHT)*32;
		for(int x=0; x<NES_WIDTH; x++){
			int indice=(x/(double)NES_WIDTH)*16;
			int color=GETINDEX_COLOR(paleta, indice);
			video_updatepixel(256*y+x, palette_get_value(color));
		}
	}
	video_show_frame();
	int *ram=malloc(32000*sizeof(int));
	
	free(ram);
	//~ while(1)
	return 0;
}

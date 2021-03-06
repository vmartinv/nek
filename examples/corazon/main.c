#include <stdio.h>
#include <graphics/video.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "ntsc/palette.h"

typedef struct {
	int x,y;
}pto;

pto st[50000];
int sz=0;

void corazon(int color, double r, double sep){
	double t;
	for(int i=0; (t=i*sep)<=2*M_PI && sz<50000; i++){
		pto *p=&st[sz++];
		p->x=(16*sin(t)*sin(t)*sin(t))*r, p->y=(13*cos(t)-5*cos(2*t)-2*cos(3*t)-cos(4*t))*r ;
		p->y=-p->y;
		p->x+=NES_WIDTH/2, p->y+=NES_HEIGHT/2;
		video_updatepixel(p->x, p->y, palette_get_value(color));
	}
}

#define CLIP(x,min,max) (((x) > (max)) ? (max) : (((x) < (min)) ? (min) : (x)))
#define N 2000

int main()
{
	palette_init();
	srand(345345);
	while(1){
		double r=0;
		int p=rand()%32;
		while(!palette_get_value(GETINDEX_COLOR((p=rand()%32), 0)));
		double sep2=CLIP((rand()/(double)RAND_MAX)/10, 0.001, 1);
		bool fixedsep=rand()%10;
		for(int i=0; i<=65; i++){
			r=i*0.1;
			double sep=CLIP((rand()/(double)RAND_MAX)/10, 0.001, 1);
			corazon(GETINDEX_COLOR(p, rand()%8), r, fixedsep?sep2:sep);
			video_flush_frame();
		}
		while(sz){
			pto *p=&st[--sz];
			video_updatepixel(p->x, p->y, palette_get_value(GETINDEX_COLOR(13,0)));
			if(!(sz%(1000)) && sz>200)
					video_flush_frame();
		}
	}
	return 0;
}

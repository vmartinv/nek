/***************************************************************************
 *   Copyright (C) 2013 by James Holodnak                                  *
 *   jamesholodnak@gmail.com                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <ntsc/palette.h>
#include <ntsc/generator.h>
#include <string.h>
#include <stdlib.h>


//palette with emphasis applied
static u8 palette[8][64 * 3];

//palette data fed to video system
static u32 palette32[8][256];		//32 bit color

//caches of all available colors
static u32 palettecache32[256];

static int rshift=16,gshift=8,bshift=0;


/* measurement by Quietust */
//~ static const double emphasis_factor[8][3]={
   //~ {1.00, 1.00, 1.00},
   //~ {1.00, 0.80, 0.81},
   //~ {0.78, 0.94, 0.66},
   //~ {0.79, 0.77, 0.63},
   //~ {0.82, 0.83, 1.12},
   //~ {0.81, 0.71, 0.87},
   //~ {0.68, 0.79, 0.79},
   //~ {0.70, 0.70, 0.70}
//~ };
//~ 
//~ static void generate_emphasis(palette_t *p)
//~ {
	//~ int i,j;
//~ 
	//~ for(j=1;j<8;j++) {
		//~ for(i=0;i<64;i++) {
			//~ p->pal[j][i].r = (u8)((double)p->pal[0][i].r * emphasis_factor[j][0]);
			//~ p->pal[j][i].g = (u8)((double)p->pal[0][i].g * emphasis_factor[j][1]);
			//~ p->pal[j][i].b = (u8)((double)p->pal[0][i].b * emphasis_factor[j][2]);
		//~ }
	//~ }
//~ }

palette_t *palette_create()
{
	palette_t *ret = (palette_t*)malloc(sizeof(palette_t));
	memset(ret,0,sizeof(palette_t));
	return ret;
}

void palette_destroy(palette_t *p)
{
	free(p);
}

//must be called AFTER video_init
void palette_set(palette_t *p)
{
	int i,j;
	palentry_t *e;

	for(j=0;j<8;j++) {
		for(i=0;i<64;i++) {
			palette[j][(i * 3) + 0] = p->pal[j][i].r;
			palette[j][(i * 3) + 1] = p->pal[j][i].g;
			palette[j][(i * 3) + 2] = p->pal[j][i].b;
		}
	}

	for(j=0;j<8;j++) {
		for(i=0;i<256;i++) {
			e = &p->pal[j][i & 0x3F];
			palette32[j][i] = (e->r << rshift) | (e->g << gshift) | (e->b << bshift);
		}
	}

	//~ filter_palette_changed();
}

int palette_init()
{
	palette_t *pal = palette_create();
	palette_generate_NTSC(pal, -15, 45);
	palette_set(pal);
	free(pal);
	for(int i=0;i<32;i++)
		palette_update(i,i);
	return 0;
}

//this handles palette changes from the nes engine
void palette_update(u8 addr,u8 data)
{
	palettecache32[addr+0x00] = palette32[0][data];
	palettecache32[addr+0x20] = palette32[1][data];
	palettecache32[addr+0x40] = palette32[2][data];
	palettecache32[addr+0x60] = palette32[3][data];
	palettecache32[addr+0x80] = palette32[4][data];
	palettecache32[addr+0xA0] = palette32[5][data];
	palettecache32[addr+0xC0] = palette32[6][data];
	palettecache32[addr+0xE0] = palette32[7][data];
}

u32 palette_get_value(const u8 idx){
	return palettecache32[idx];
}



u8 *palette_get()		{	return(u8*)(palette);		}

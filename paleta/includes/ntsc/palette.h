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

#ifndef __palette_h__
#define __palette_h__

#include <types.h>

typedef struct palentry_s {
	u8 r,g,b;
} palentry_t;

typedef struct palette_s {
	palentry_t pal[8][64];
} palette_t;

palette_t *palette_create();
void palette_destroy(palette_t *p);
int palette_init();
u8 *palette_get();
void palette_update(u8 addr,u8 data);
void palette_set(palette_t *p);


#define GETINDEX_COLOR(paleta, idx) ((paleta)+(idx)*0x20)
u32 palette_get_value(const u8 idx);

#endif

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

//palette generator borrowed from nintendulator

#include <math.h>
#include <graphics/palette.h>

#ifndef M_PI
#define M_PI 3.14159f
#endif

#define CLIP(x,min,max) (((x) > (max)) ? (max) : (((x) < (min)) ? (min) : (x)))

static	int	getPhase (double *wave)
{
	double max = -999, min = 999;
	double amp, offset;
	double angle = 0, base;
	int i, j, k;
	for (i = 0; i < 12; i++)
	{
		if (wave[i] > max)
			max = wave[i];
		if (wave[i] < min)
			min = wave[i];
	}
	amp = (max - min) / 2;
	offset = (max + min) / 2;

	for (k = 0; k < 3; k++)
	{
		double error[12], curerror = 0;
		double segsize = 360;
		for (i = 0; i <= k; i++)
			segsize /= 12.0;

		for (j = 0; j < 12; j++)
		{
			error[j] = 0;
			for (i = 0; i < 12; i++)
				error[j] += fabs((amp * sin((i * 30 + j * segsize + angle) * M_PI / 180.0) + offset) - wave[i]);
			curerror += error[j];
		}
		base = 0;
		for (j = 0; j < 12; j++)
		{
			if (error[j] < curerror)
			{
				base = j * segsize;
				curerror = error[j];
			}
		}
		angle += base;
	}

	return (int)angle;
}

void palette_generate_NTSC(palette_t *pal,int hue,int sat)
{
	const double black = 0.519;
	const double white = 1.443;
	const double voltage[2][4] = {{1.094,1.506,1.962,1.962},{0.350,0.519,0.962,1.550}};

	const char phases[12][12] = {
		{1,1,1,1,1,1,0,0,0,0,0,0},
		{1,1,1,1,1,0,0,0,0,0,0,1},	// blue
		{1,1,1,1,0,0,0,0,0,0,1,1},
		{1,1,1,0,0,0,0,0,0,1,1,1},	// magenta
		{1,1,0,0,0,0,0,0,1,1,1,1},
		{1,0,0,0,0,0,0,1,1,1,1,1},	// red
		{0,0,0,0,0,0,1,1,1,1,1,1},
		{0,0,0,0,0,1,1,1,1,1,1,0},	// yellow
		{0,0,0,0,1,1,1,1,1,1,0,0},
		{0,0,0,1,1,1,1,1,1,0,0,0},	// green
		{0,0,1,1,1,1,1,1,0,0,0,0},
		{0,1,1,1,1,1,1,0,0,0,0,0},	// cyan
	};
	const char emphasis[8][12] = {
		{0,0,0,0,0,0,0,0,0,0,0,0},	// none
		{0,1,1,1,1,1,1,0,0,0,0,0},	// red
		{1,1,1,0,0,0,0,0,0,1,1,1},	// green
		{1,1,1,1,1,1,1,0,0,1,1,1},	// yellow
		{0,0,0,0,0,1,1,1,1,1,1,0},	// blue
		{0,1,1,1,1,1,1,1,1,1,1,0},	// magenta
		{1,1,1,0,0,1,1,1,1,1,1,1},	// cyan
		{1,1,1,1,1,1,1,1,1,1,1,1}	// all
	};

	int i, x, y, z;
	for (x = 0; x < 8; x++)
	{
		for (y = 0; y < 4; y++)
		{
			for (z = 0; z < 16; z++)
			{
				double wave[12];
				double Y, I, Q;
				double R, G, B;

				double H = 0, S = 0;

				for (i = 0; i < 12; i++)
				{
					if (z == 0)
						wave[i] = voltage[0][y];
					else if (z < 13)
						wave[i] = phases[z-1][i] ? voltage[0][y] : voltage[1][y];
					else if (z == 13)
						wave[i] = voltage[1][y];
					else	wave[i] = black;
					if ((emphasis[x][i]) && (z < 14))
						wave[i] = wave[i] * 0.75;
				}

				Y = 0.0; S = 0;
				for (i = 0; i < 12; i++)
					Y += wave[i] / 12.0;
				for (i = 0; i < 12; i++)
					S += (wave[i] - Y) * (wave[i] - Y);
				Y = (Y - black) / white;
				S = S / white;	// don't remove black offset, since this is already relative
				S = sqrt(S / 12.0) * sat / 50.0;

				H = M_PI * (270 + getPhase(wave) + hue) / 180.0;

				I = S * sin(H);
				Q = S * cos(H);

				R = Y + 0.956 * I + 0.621 * Q;
				G = Y - 0.272 * I - 0.647 * Q;
				B = Y - 1.107 * I + 1.705 * Q;

				R *= 256;
				G *= 256;
				B *= 256;

				pal->pal[x][(y << 4) | z].r = (unsigned char)CLIP(R,0,255);
				pal->pal[x][(y << 4) | z].g = (unsigned char)CLIP(G,0,255);
				pal->pal[x][(y << 4) | z].b = (unsigned char)CLIP(B,0,255);
			}
		}
	}
}

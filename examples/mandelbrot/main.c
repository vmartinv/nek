/*
http://es.wikipedia.org/wiki/Fractal
http://wiki.osdev.org/Text_UI#Colours
*/
#include <stdio.h>
#include <graphics/video.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

void textmode_setforecolor(uint8_t back);

int main()
{
	double width=video_getwidth(), height=video_getheight();
	double xstart=-1,ystart=-1;
	int iter=100;
	double zoom=2;


	/* these are used for calculating the points corresponding to the pixels */
	while(true){
		double xend=xstart+zoom, yend=ystart+zoom;
		double xstep = (xend-xstart)/width;
		double ystep = (yend-ystart)/height;
		double x = xstart, y = ystart;
		for (int i=0; i<height; i++){
			for (int j=0; j<width; j++){
				double z = 0, zi = 0;
				double color;
				bool inset = true;
				for (int k=0; k<iter && inset; k++){
					/* z^2 = (a+bi)(a+bi) = a^2 + 2abi - b^2 */
					double newz = (z*z)-(zi*zi) + x, newzi = 2*z*zi + y;
					z = newz;
					zi = newzi;
					if(((z*z)+(zi*zi)) > 4){
						color=k;
						inset = false;
					}
				}
				if (inset){
					video_updatepixel_raw(i, j, 0);
				}
				else{
					double color2=color / iter * 255;
					video_updatepixel_raw(i, j, RGB(color2, color2/2, color2/2));
				}
				x += xstep;
			}
			y += ystep;
			x = xstart;
		}
		char c=0;
		while(!(c=getchar()));
		while(getchar());//empty the buffer
		switch(c){
			case 'w':
				ystart-=zoom/4;
				break;
			case 's':
				ystart+=zoom/4;
				break;
			case 'd':
				xstart+=zoom/4;
				break;
			case 'a':
				xstart-=zoom/4;
				break;
			case 'q':
				xstart-=zoom*0.1;
				ystart-=zoom*0.2;
				zoom/=0.8;
				break;
			case 'e':
				xstart+=zoom*0.1;
				ystart+=zoom*0.1;
				zoom*=0.8;
				break;
			case 'z':
				if(iter>10) iter-=10;
				break;
			case 'c':
				iter+=10;
				break;
			case 'r':
				iter=10;
				zoom=2;
				xstart=-1, ystart=-1;
				break;
			case 'h':
				console_clear();
				textmode_setforecolor(6);
				printf("wasd");
				textmode_setforecolor(0x7);
				printf(": moverse\n");
				textmode_setforecolor(2);
				printf("q e");
				textmode_setforecolor(0x7);
				printf(": disminuir/aumentar zoom\n");
				textmode_setforecolor(3);
				printf("z c");
				textmode_setforecolor(0x7);
				printf(": disminuir/aumentar iteraciones\n");
				textmode_setforecolor(4);
				printf("h");
				textmode_setforecolor(0x7);
				printf(": esta ayuda\n");
				textmode_setforecolor(4);
				printf("r");
				textmode_setforecolor(0x7);
				printf(": reiniciar\n");
				textmode_setforecolor(0x7);
				printf("Presione ");
				textmode_setforecolor(5);
				printf("cualquier tecla");
				textmode_setforecolor(0x7);
				printf(" para continuar...\n");
				video_flush_console();
				while(!(c=getchar()));
				break;
			default:
				break;
		}
	}
	return 0;
} 

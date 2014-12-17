#include <graphics/video.h>
#include <arch/x86/textmode_console.h>
#include <sys/console.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <vector>
#include <gamepak.h>
#include <cpu.h>
#include <io.h>
#include <ppu.h>
#include "NES_logo.h"


int play_game(const char *game){
	video_flush_console();
    // Open the ROM file
    FILE* fp = fopen(game, "rb");
    if(!fp) return 1;
	static const char nesheader[]={'N','E','S','\32'};
    for(int i=0; i<4; i++) if(fgetc(fp)!=nesheader[i]) return 2;
    
    // Read the ROM file header
    u8 rom16count = fgetc(fp);
    u8 vrom8count = fgetc(fp);
    u8 ctrlbyte   = fgetc(fp);
    u8 mappernum  = fgetc(fp) | (ctrlbyte>>4);
    for(int i=0; i<8; i++) fgetc(fp);
    if(mappernum >= 0x40) mappernum &= 15;
    GamePak::mappernum = mappernum;
    
    // Read the ROM data
    if(rom16count) GamePak::ROM.resize(rom16count * 0x4000);
    if(vrom8count) GamePak::VRAM.resize(vrom8count * 0x2000);
    fread(&GamePak::ROM[0], rom16count, 0x4000, fp);
    fread(&GamePak::VRAM[0], vrom8count, 0x2000, fp);
    fclose(fp);
    //~ printf("%u * 16kB ROM, %u * 8kB VROM, mapper %u, ctrlbyte %02X\n", rom16count, vrom8count, mappernum, ctrlbyte);
	//~ video_flush_console();

    // Prepare emulation
    GamePak::Init();
    IO::Init();
    PPU::Init();
    CPU::Init();
    // Run the CPU until the program is killed.
    while(CPU::running) CPU::Op();
	return 0;
}

std::vector<char*> roms;

int search_roms(){
	roms.clear();
	DIR *dir;
	struct dirent *ent;
	if((dir = opendir ("./")) == NULL) return 1;
	for(int i=0; (ent = readdir (dir)) != NULL; i++) {
		int len=strlen(ent->d_name);
		if(len>4 && !strcmp(ent->d_name+len-4, ".nes"))
		{
			char *buf=new char[len+1];
			strcpy(buf, ent->d_name);
			roms.push_back(buf);
		}
	}
	closedir (dir);
	if(!roms.size()) return 2;
	return 0;
}

void print_header(){
	console_clear();
	textmode_setforecolor(2);
	for(int i=0; i<14; i++) putchar('\n');
	printf("\t\t\tPor Martin Villagra\n\n\n");
	video_flush_console();
	video_draw(-1,20,logo_data,logo_width,logo_height);
	textmode_setforecolor(0xF);
}

const unsigned maxshow=15;

void print_game_list(unsigned pos, unsigned offset){
	const unsigned start_line=17;
	console_clear();
	textmode_setforecolor(7);
	console_move_write_cursor(0, start_line);
	for(unsigned i=offset; i<min(maxshow+offset, roms.size()); i++) {
		unsigned len=strlen(roms[i]);
		roms[i][len-4]=0;
		putchar('\t');
		if(i==pos) {
			textmode_setforecolor(rand()%0xF+1);
			putchar('*');
			textmode_setforecolor(7);
		}
		else putchar(' ');
		printf(" %s\n", roms[i]);
		roms[i][len-4]='.';
	}
	textmode_setforecolor(0xF);
	video_flush_console2(17);

}

int main(){
	if(search_roms()) return 1;
	unsigned pos=0, offset=0;
	int c;
	while(1){
		print_header();
		do{
			textmode_setforecolor(rand()%0xF+1);
			print_game_list(pos, offset);
			c=wait_scancode();
			if(c==31){
				if(pos<roms.size()-1) pos++;
				if(pos>=maxshow+offset) offset++;
			}
			else if(c==17){
				if(pos) pos--;
				if(pos<offset) offset--;
			}
		}while(c!=RETURN_KEY);
		print_header();
		play_game(roms[pos]);
	}
	return 0;
}

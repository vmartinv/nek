#include <graphics/video.h>
#include <arch/x86/textmode_console.h>
#include <sys/console.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <gamepak.h>
#include <cpu.h>
#include <io.h>
#include <ppu.h>
#include "NES_logo.h"

int play_game(const char *game){
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

void print_menu(){
	console_clear();
	textmode_setforecolor(2);
	printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\t\t\tPor Martin Villagra\n\n\n");
	textmode_setforecolor(7);
	for(unsigned i=0; i<roms.size(); i++) {
		int len=strlen(roms[i]);
		roms[i][len-4]=0;
		printf ("\t%s\n", roms[i]);
		roms[i][len-4]='.';
	}
	textmode_setforecolor(0xF);
	video_flush_console();
	video_draw(-1,20,logo_data,logo_width,logo_height);
}

void put_mark(int pos, char c){
	const int start_line=17;
	console_move_write_cursor(5, start_line+pos);
	putchar_now(c);
}

template <class T> const T& min (const T& a, const T& b) {
  return (a>b)?b:a;     // or: return comp(a,b)?b:a; for version (2)
}
template <class T> const T& max (const T& a, const T& b) {
  return (a<b)?b:a;     // or: return comp(a,b)?b:a; for version (2)
}

int main(){
	if(search_roms()) return 1;
	
	print_menu();
	int pos=0, c;
	do{
		textmode_setforecolor(rand()%0xF+1);
		put_mark(pos, '*');
		c=wait_scancode();
		put_mark(pos, ' ');
		if(c==DOWN_ARROY_KEY) pos=min(pos+1, (int)roms.size()-1);
		else if(c==UP_ARROW_KEY) pos=max(pos-1, 0);
		if(c==RETURN_KEY) play_game(roms[pos]), print_menu();
	}while(1);
	return 0;
}

#ifndef GAMEPAK_H
#define GAMEPAK_H

#include <nestypes.h>
#include <vector>

namespace GamePak
{
    extern std::vector<u8> ROM, VRAM;
    extern unsigned mappernum;
    extern const unsigned VROM_Granularity;
    extern const unsigned VROM_Pages;
    extern u8* Vbanks[];
    extern u8* Nta[4];

	void Init();
	u8 Access(unsigned addr, u8 value, bool write);
}

#endif

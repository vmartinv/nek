#ifndef PPU_H
#define PPU_H
#include <nestypes.h>

namespace PPU {
	void Init();
	void tick();
    u8 Access(u16 index, u8 v, bool write);
}

#endif

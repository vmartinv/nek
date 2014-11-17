#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <nestypes.h>
#include <gamepak.h>
#include <graphics/video.h>


namespace GamePak
{
    std::vector<u8> ROM, VRAM;
    unsigned mappernum;
    const unsigned VROM_Granularity = 0x0400, VROM_Pages = 0x2000 / VROM_Granularity;
    const unsigned ROM_Granularity  = 0x2000, ROM_Pages = 0x10000 / ROM_Granularity;
    unsigned char NRAM[0x1000], PRAM[0x2000];
    u8* banks[ROM_Pages]  = {};
    u8* Vbanks[VROM_Pages] = {};
    u8* Nta[4] = { NRAM+0x0000, NRAM+0x0400, NRAM+0x0000, NRAM+0x0400 };

    template<unsigned npages,unsigned char*(&b)[npages], std::vector<u8>& r, unsigned granu>
    static void SetPages(unsigned size, unsigned baseaddr, unsigned index)
    {
			//~ printf("asdasd\n");
			//~ video_flush_console();
        for(unsigned v = r.size() + index * size,
                     p = baseaddr / granu;
                     p < (baseaddr + size) / granu && p < npages;
                     ++p, v += granu)
            b[p] = &r[v % r.size()];
    }
    auto& SetROM  = SetPages< ROM_Pages, banks, ROM, ROM_Granularity>;
    auto& SetVROM = SetPages<VROM_Pages,Vbanks,VRAM,VROM_Granularity>;

    u8 Access(unsigned addr, u8 value, bool write)
    {
        if(write && addr >= 0x8000 && mappernum == 7) // e.g. Rare games
        {
            SetROM(0x8000, 0x8000, (value&7));
            Nta[0] = Nta[1] = Nta[2] = Nta[3] = &NRAM[0x400 * ((value>>4)&1)];
        }
        if(write && addr >= 0x8000 && mappernum == 2) // e.g. Rockman, Castlevania
        {
            SetROM(0x4000, 0x8000, value);
        }
        if(write && addr >= 0x8000 && mappernum == 3) // e.g. Kage, Solomon's Key
        {
            value &= Access(addr,0,false); // Simulate bus conflict
            SetVROM(0x2000, 0x0000, (value&3));
        }
        if(write && addr >= 0x8000 && mappernum == 1) // e.g. Rockman 2, Simon's Quest
        {
			
            static u8 regs[4]={0x0C,0,0,0}, counter=0, cache=0;
            if(value & 0x80) { regs[0]=0x0C; goto configure; }
            cache |= (value&1) << counter;
            if(++counter == 5)
            {
                regs[ (addr>>13) & 3 ] = value = cache;
            configure:
                cache = counter = 0;
                static const u8 sel[4][4] = { {0,0,0,0}, {1,1,1,1}, {0,1,0,1}, {0,0,1,1} };
                for(unsigned m=0; m<4; ++m) Nta[m] = &NRAM[0x400 * sel[regs[0]&3][m]];
                SetVROM(0x1000, 0x0000, ((regs[0]&16) ? regs[1] : ((regs[1]&~1)+0)));
                SetVROM(0x1000, 0x1000, ((regs[0]&16) ? regs[2] : ((regs[1]&~1)+1)));
                switch( (regs[0]>>2)&3 )
                {
                    case 0: case 1:
                        SetROM(0x8000, 0x8000, (regs[3] & 0xE) / 2);
                        break;
                    case 2:
                        SetROM(0x4000, 0x8000, 0);
                        SetROM(0x4000, 0xC000, (regs[3] & 0xF));
                        break;
                    case 3:
                        SetROM(0x4000, 0x8000, (regs[3] & 0xF));
                        SetROM(0x4000, 0xC000, ~0);
                        break;
                }
            }
        }
        if( (addr >> 13) == 3 ) return PRAM[addr & 0x1FFF ];
        return banks[ (addr / ROM_Granularity) % ROM_Pages] [addr % ROM_Granularity];
    }
    void Init(){
        SetVROM(0x2000, 0x0000, 0);
        for(unsigned v=0; v<4; ++v) SetROM(0x4000, v*0x4000, v==3 ? -1 : 0);
    }
}

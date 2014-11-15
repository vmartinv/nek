#ifndef CPU_H
#define CPU_H

namespace CPU /* CPU: Ricoh RP2A03 (based on MOS6502, almost the same as in Commodore 64) */
{
    extern bool reset, nmi, nmi_edge_detected, intr, running;
    u8 RB(u16 addr);
    u8 WB(u16 addr,u8 v);
    void tick();
    void Init();
    void Op();
}

#endif

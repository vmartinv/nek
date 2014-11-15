#include <stdio.h>
#include <stdlib.h>
#include <nestypes.h>
#include <ppu.h>
#include <gamepak.h>
#include <io.h>
#include <apu.h>



namespace CPU /* CPU: Ricoh RP2A03 (based on MOS6502, almost the same as in Commodore 64) */
{
    u8 RAM[0x800];
    bool reset, nmi, nmi_edge_detected, intr, running;

    template<bool write> static u8 MemAccess(u16 addr, u8 v=0);
    u8 RB(u16 addr)      { return MemAccess<0>(addr); }
    u8 WB(u16 addr,u8 v) { return MemAccess<1>(addr, v); }
    static void tick()
    {
        // PPU clock: 3 times the CPU rate
        for(unsigned n=0; n<3; ++n) PPU::tick();
        // APU clock: 1 times the CPU rate
        for(unsigned n=0; n<1; ++n) APU::tick();
    }
    
    template<bool write> static u8 MemAccess(u16 addr, u8 v)
    {
        // Memory writes are turned into reads while reset is being signalled
        if(reset && write) return MemAccess<0>(addr);

        tick();
        // Map the memory from CPU's viewpoint.
        /**/ if(addr < 0x2000) { u8& r = RAM[addr & 0x7FF]; if(!write)return r; r=v; }
        else if(addr < 0x4000) return PPU::Access(addr&7, v, write);
        else if(addr < 0x4018)
            switch(addr & 0x1F)
            {
                case 0x14: // OAM DMA: Copy 256 bytes from RAM into PPU's sprite memory
                    if(write) for(unsigned b=0; b<256; ++b) WB(0x2004, RB((v&7)*0x0100+b));
                    return 0;
                case 0x15: if(!write) return APU::Read();    APU::Write(0x15,v); break;
                case 0x16: if(!write) return IO::JoyRead(0); IO::JoyStrobe(v); break;
                case 0x17: if(!write) return IO::JoyRead(1); // write:passthru
                default: if(!write) break;
                         APU::Write(addr&0x1F, v);
            }
        else return GamePak::Access(addr, v, write);
        return 0;
    }

    // CPU registers:
    static u16 PC=0xC000;
    static u8 A=0,X=0,Y=0,S=0;
    static union{ /* Status flags: */
        u8 raw;
        RegBit<0> C; // carry
        RegBit<1> Z; // zero
        RegBit<2> I; // interrupt enable/disable
        RegBit<3> D; // decimal mode (unsupported on NES, but flag exists)
        // 4,5 (0x10,0x20) don't exist
        RegBit<6> V; // overflow
        RegBit<7> N; // negative
    } P;
    
    void Init(){
		reset=true, nmi=false, nmi_edge_detected=false, intr=false;
		running=true;
		// Pre-initialize RAM the same way as FCEUX does, to improve TAS sync.
		for(unsigned a=0; a<0x800; ++a)
			RAM[a] = (a&4) ? 0xFF : 0x00;
	}

    static u16 wrap(u16 oldaddr, u16 newaddr)  { return (oldaddr & 0xFF00) + u8(newaddr); }
    static void Misfire(u16 old, u16 addr) { u16 q = wrap(old, addr); if(q != addr) RB(q); }
    static u8   Pop()        { return RB(0x100 | u8(++S)); }
    static void Push(u8 v)   { WB(0x100 | u8(S--), v); }

    template<u16 op> // Execute a single CPU instruction, defined by opcode "op".
    void Ins()       // With template magic, the compiler will literally synthesize >256 different functions.
    {
        // Note: op 0x100 means "NMI", 0x101 means "Reset", 0x102 means "IRQ". They are implemented in terms of "BRK".
        // User is responsible for ensuring that WB() will not store into memory while Reset is being processed.
        unsigned addr=0, d=0, t=0xFF, c=0, sb=0, pbits = op<0x100 ? 0x30 : 0x20;

        // Define the opcode decoding matrix, which decides which micro-operations constitute
        // any particular opcode. (Note: The PLA of 6502 works on a slightly different principle.)
        enum { o8 = op/8, o8m = 1 << (op%8) };
        // Fetch op'th item from a bitstring encoded in a data-specific variant of base64,
        // where each character transmits 8 bits of information rather than 6.
        // This peculiar encoding was chosen to reduce the source code size.
        // Enum temporaries are used in order to ensure compile-time evaluation.
        #define t(s,code) { enum { \
            i=o8m & (s[o8]>90 ? (130+" (),-089<>?BCFGHJLSVWZ[^hlmnxy|}"[s[o8]-94]) \
                              : (s[o8]-" (("[s[o8]/39])) }; if(i) { code; } }

        /* Decode address operand */
        t("                                !", addr = 0xFFFA) // NMI vector location
        t("                                *", addr = 0xFFFC) // Reset vector location
        t("!                               ,", addr = 0xFFFE) // Interrupt vector location
        t("zy}z{y}zzy}zzy}zzy}zzy}zzy}zzy}z ", addr = RB(PC++))
        t("2 yy2 yy2 yy2 yy2 XX2 XX2 yy2 yy ", d = X) // register index
        t("  62  62  62  62  om  om  62  62 ", d = Y)
        t("2 y 2 y 2 y 2 y 2 y 2 y 2 y 2 y  ", addr=u8(addr+d); d=0; tick())              // add zeropage-index
        t(" y z!y z y z y z y z y z y z y z ", addr=u8(addr);   addr+=256*RB(PC++))       // absolute address
        t("3 6 2 6 2 6 286 2 6 2 6 2 6 2 6 /", addr=RB(c=addr); addr+=256*RB(wrap(c,c+1)))// indirect w/ page wrap
        t("  *Z  *Z  *Z  *Z      6z  *Z  *Z ", Misfire(addr, addr+d)) // abs. load: extra misread when cross-page
        t("  4k  4k  4k  4k  6z      4k  4k ", RB(wrap(addr, addr+d)))// abs. store: always issue a misread
        /* Load source operand */
        t("aa__ff__ab__,4  ____ -  ____     ", t &= A) // Many operations take A or X as operand. Some try in
        t("                knnn     4  99   ", t &= X) // error to take both; the outcome is an AND operation.
        t("                9989    99       ", t &= Y) // sty,dey,iny,tya,cpy
        t("                       4         ", t &= S) // tsx, las
        t("!!!!  !!  !!  !!  !   !!  !!  !!/", t &= P.raw|pbits; c = t)// php, flag test/set/clear, interrupts
        t("_^__dc___^__            ed__98   ", c = t; t = 0xFF)        // save as second operand
        t("vuwvzywvvuwvvuwv    zy|zzywvzywv ", t &= RB(addr+d)) // memory operand
        t(",2  ,2  ,2  ,2  -2  -2  -2  -2   ", t &= RB(PC++))   // immediate operand
        /* Operations that mogrify memory operands directly */
        t("    88                           ", P.V = t & 0x40; P.N = t & 0x80) // bit
        t("    nink    nnnk                 ", sb = P.C)       // rol,rla, ror,rra,arr
        t("nnnknnnk     0                   ", P.C = t & 0x80) // rol,rla, asl,slo,[arr,anc]
        t("        nnnknink                 ", P.C = t & 0x01) // lsr,sre, ror,rra,asr
        t("ninknink                         ", t = (t << 1) | (sb * 0x01))
        t("        nnnknnnk                 ", t = (t >> 1) | (sb * 0x80))
        t("                 !      kink     ", t = u8(t - 1))  // dec,dex,dey,dcp
        t("                         !  khnk ", t = u8(t + 1))  // inc,inx,iny,isb
        /* Store modified value (memory) */
        t("kgnkkgnkkgnkkgnkzy|J    kgnkkgnk ", WB(addr+d, t))
        t("                   q             ", WB(wrap(addr, addr+d), t &= ((addr+d) >> 8))) // [shx,shy,shs,sha?]
        /* Some operations used up one clock cycle that we did not account for yet */
        t("rpstljstqjstrjst - - - -kjstkjst/", tick()) // nop,flag ops,inc,dec,shifts,stack,transregister,interrupts
        /* Stack operations and unconditional jumps */
        t("     !  !    !                   ", tick(); t = Pop())                        // pla,plp,rti
        t("        !   !                    ", RB(PC++); PC = Pop(); PC |= (Pop() << 8)) // rti,rts
        t("            !                    ", RB(PC++))  // rts
        t("!   !                           /", d=PC+(op?-1:1); Push(d>>8); Push(d))      // jsr, interrupts
        t("!   !    8   8                  /", PC = addr) // jmp, jsr, interrupts
        t("!!       !                      /", Push(t))   // pha, php, interrupts
        /* Bitmasks */
        t("! !!  !!  !!  !!  !   !!  !!  !!/", t = 1)
        t("  !   !                   !!  !! ", t <<= 1)
        t("! !   !   !!  !!       !   !   !/", t <<= 2)
        t("  !   !   !   !        !         ", t <<= 4)
        t("   !       !           !   !____ ", t = u8(~t)) // sbc, isb,      clear flag
        t("`^__   !       !               !/", t = c | t)  // ora, slo,      set flag
        t("  !!dc`_  !!  !   !   !!  !!  !  ", t = c & t)  // and, bit, rla, clear/test flag
        t("        _^__                     ", t = c ^ t)  // eor, sre
        /* Conditional branches */
        t("      !       !       !       !  ", if(t)  { tick(); Misfire(PC, addr = s8(addr) + PC); PC=addr; })
        t("  !       !       !       !      ", if(!t) { tick(); Misfire(PC, addr = s8(addr) + PC); PC=addr; })
        /* Addition and subtraction */
        t("            _^__            ____ ", c = t; t += A + P.C; P.V = (c^t) & (A^t) & 0x80; P.C = t & 0x100)
        t("                        ed__98   ", t = c - t; P.C = ~t & 0x100) // cmp,cpx,cpy, dcp, sbx
        /* Store modified value (register) */
        t("aa__aa__aa__ab__ 4 !____    ____ ", A = t)
        t("                    nnnn 4   !   ", X = t) // ldx, dex, tax, inx, tsx,lax,las,sbx
        t("                 !  9988 !       ", Y = t) // ldy, dey, tay, iny
        t("                   4   0         ", S = t) // txs, las, shs
        t("!  ! ! !!  !   !       !   !   !/", P.raw = t & ~0x30) // plp, rti, flag set/clear
        /* Generic status flag updates */
        t("wwwvwwwvwwwvwxwv 5 !}}||{}wv{{wv ", P.N = t & 0x80)
        t("wwwv||wvwwwvwxwv 5 !}}||{}wv{{wv ", P.Z = u8(t) == 0)
        t("             0                   ", P.V = (((t >> 5)+1)&2))         // [arr]
        /* All implemented opcodes are cycle-accurate and memory-access-accurate.
         * [] means that this particular separate rule exists only to provide the indicated unofficial opcode(s).
         */
    }

    void Op()
    {
        /* Check the state of NMI flag */
        bool nmi_now = nmi;

        unsigned op = RB(PC++);

        if(reset)                              { op=0x101; }
        else if(nmi_now && !nmi_edge_detected) { op=0x100; nmi_edge_detected = true; }
        else if(intr && !P.I)                  { op=0x102; }
        if(!nmi_now) nmi_edge_detected=false;

        // Define function pointers for each opcode (00..FF) and each interrupt (100,101,102)
        #define c(n) Ins<0x##n>,Ins<0x##n+1>,
        #define o(n) c(n)c(n+2)c(n+4)c(n+6)
        static void(*const i[0x108])() =
        {
            o(00)o(08)o(10)o(18)o(20)o(28)o(30)o(38)
            o(40)o(48)o(50)o(58)o(60)o(68)o(70)o(78)
            o(80)o(88)o(90)o(98)o(A0)o(A8)o(B0)o(B8)
            o(C0)o(C8)o(D0)o(D8)o(E0)o(E8)o(F0)o(F8) o(100)
        };
        #undef o
        #undef c
        i[op]();

        reset = false;
    }
}

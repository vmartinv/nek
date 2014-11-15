#include <apu.h>
#include <cpu.h>

namespace APU{
	void tick(){}
	void Write(u8 index, u8 value){}
	u8 Read(){return 0;}
}
//~ 
//~ namespace APU /* Audio Processing Unit */
//~ {
    //~ static const u8 LengthCounters[32] = { 10,254,20, 2,40, 4,80, 6,160, 8,60,10,14,12,26,14,
                                           //~ 12, 16,24,18,48,20,96,22,192,24,72,26,16,28,32,30 };
    //~ static const u16 NoisePeriods[16] = { 2,4,8,16,32,48,64,80,101,127,190,254,381,508,1017,2034 };
    //~ static const u16 DMCperiods[16] = { 428,380,340,320,286,254,226,214,190,160,142,128,106,84,72,54 };
//~ 
    //~ bool FiveCycleDivider = false, IRQdisable = true, ChannelsEnabled[5] = { false };
    //~ bool PeriodicIRQ = false, DMC_IRQ = false;
    //~ bool count(int& v, int reset) { return --v < 0 ? (v=reset),true : false; }
//~ 
    //~ struct channel
    //~ {
        //~ int length_counter, linear_counter, address, envelope;
        //~ int sweep_delay, env_delay, wave_counter, hold, phase, level;
        //~ union // Per-channel register file
        //~ {
            //~ // 4000, 4004, 400C, 4012:            // 4001, 4005, 4013:            // 4002, 4006, 400A, 400E:
            //~ RegBit<0,8,u32> reg0;                 RegBit< 8,8,u32> reg1;          RegBit<16,8,u32> reg2;
            //~ RegBit<6,2,u32> DutyCycle;            RegBit< 8,3,u32> SweepShift;    RegBit<16,4,u32> NoiseFreq;
            //~ RegBit<4,1,u32> EnvDecayDisable;      RegBit<11,1,u32> SweepDecrease; RegBit<23,1,u32> NoiseType;
            //~ RegBit<0,4,u32> EnvDecayRate;         RegBit<12,3,u32> SweepRate;     RegBit<16,11,u32> WaveLength;
            //~ RegBit<5,1,u32> EnvDecayLoopEnable;   RegBit<15,1,u32> SweepEnable;   // 4003, 4007, 400B, 400F, 4010:
            //~ RegBit<0,4,u32> FixedVolume;          RegBit< 8,8,u32> PCMlength;     RegBit<24,8,u32> reg3;
            //~ RegBit<5,1,u32> LengthCounterDisable;                                 RegBit<27,5,u32> LengthCounterInit;
            //~ RegBit<0,7,u32> LinearCounterInit;                                    RegBit<30,1,u32> LoopEnabled;
            //~ RegBit<7,1,u32> LinearCounterDisable;                                 RegBit<31,1,u32> IRQenable;
        //~ } reg;
//~ 
        //~ // Function for updating the wave generators and taking the sample for each channel.
        //~ template<unsigned c>
        //~ int tick()
        //~ {
            //~ channel& ch = *this;
            //~ if(!ChannelsEnabled[c]) return c==4 ? 64 : 8;
            //~ int wl = (ch.reg.WaveLength+1) * (c >= 2 ? 1 : 2);
            //~ if(c == 3) wl = NoisePeriods[ ch.reg.NoiseFreq ];
            //~ int volume = ch.length_counter ? ch.reg.EnvDecayDisable ? ch.reg.FixedVolume : ch.envelope : 0;
            //~ // Sample may change at wavelen intervals.
            //~ auto& S = ch.level;
            //~ if(!count(ch.wave_counter, wl)) return S;
            //~ switch(c)
            //~ {
                //~ default:// Square wave. With four different 8-step binary waveforms (32 bits of data total).
                    //~ if(wl < 8) return S = 8;
                    //~ return S = (0xF33C0C04u & (1u << (++ch.phase % 8 + ch.reg.DutyCycle * 8))) ? volume : 0;
//~ 
                //~ case 2: // Triangle wave
                    //~ if(ch.length_counter && ch.linear_counter && wl >= 3) ++ch.phase;
                    //~ return S = (ch.phase & 15) ^ ((ch.phase & 16) ? 15 : 0);
//~ 
                //~ case 3: // Noise: Linear feedback shift register
                    //~ if(!ch.hold) ch.hold = 1;
                    //~ ch.hold = (ch.hold >> 1)
                          //~ | (((ch.hold ^ (ch.hold >> (ch.reg.NoiseType ? 6 : 1))) & 1) << 14);
                    //~ return S = (ch.hold & 1) ? 0 : volume;
//~ 
                //~ case 4: // Delta modulation channel (DMC)
                    //~ // hold = 8 bit value, phase = number of bits buffered
                    //~ if(ch.phase == 0) // Nothing in sample buffer?
                    //~ {
                        //~ if(!ch.length_counter && ch.reg.LoopEnabled) // Loop?
                        //~ {
                            //~ ch.length_counter = ch.reg.PCMlength*16 + 1;
                            //~ ch.address        = (ch.reg.reg0 | 0x300) << 6;
                        //~ }
                        //~ if(ch.length_counter > 0) // Load next 8 bits if available
                        //~ {
                            //~ // Note: Re-entrant! But not recursive, because even
                            //~ // the shortest wave length is greater than the read time.
                            //~ // TODO: proper clock
                            //~ if(ch.reg.WaveLength>20)
                                //~ for(unsigned t=0; t<3; ++t) CPU::RB(u16(ch.address) | 0x8000); // timing
                            //~ ch.hold  = CPU::RB(u16(ch.address++) | 0x8000); // Fetch byte
                            //~ ch.phase = 8;
                            //~ --ch.length_counter;
                        //~ }
                        //~ else // Otherwise, disable channel or issue IRQ
                            //~ ChannelsEnabled[4] = ch.reg.IRQenable && (CPU::intr = DMC_IRQ = true);
                    //~ }
                    //~ if(ch.phase != 0) // Update the signal if sample buffer nonempty
                    //~ {
                        //~ int v = ch.linear_counter;
                        //~ if(ch.hold & (0x80 >> --ch.phase)) v += 2; else v -= 2;
                        //~ if(v >= 0 && v <= 0x7F) ch.linear_counter = v;
                    //~ }
                    //~ return S = ch.linear_counter;
            //~ }
        //~ }
    //~ } channels[5] = { };
//~ 
    //~ struct { short lo, hi; } hz240counter = { 0,0 };
//~ 
    //~ void Write(u8 index, u8 value)
    //~ {
        //~ channel& ch = channels[(index/4) % 5];
        //~ switch(index<0x10 ? index%4 : index)
        //~ {
            //~ case 0: if(ch.reg.LinearCounterDisable) ch.linear_counter=value&0x7F; ch.reg.reg0 = value; break;
            //~ case 1: ch.reg.reg1 = value; ch.sweep_delay = ch.reg.SweepRate; break;
            //~ case 2: ch.reg.reg2 = value; break;
            //~ case 3:
                //~ ch.reg.reg3 = value;
                //~ if(ChannelsEnabled[index/4])
                    //~ ch.length_counter = LengthCounters[ch.reg.LengthCounterInit];
                //~ ch.linear_counter = ch.reg.LinearCounterInit;
                //~ ch.env_delay      = ch.reg.EnvDecayRate;
                //~ ch.envelope       = 15;
                //~ if(index < 8) ch.phase = 0;
                //~ break;
            //~ case 0x10: ch.reg.reg3 = value; ch.reg.WaveLength = DMCperiods[value&0x0F]; break;
            //~ case 0x12: ch.reg.reg0 = value; ch.address = (ch.reg.reg0 | 0x300) << 6; break;
            //~ case 0x13: ch.reg.reg1 = value; ch.length_counter = ch.reg.PCMlength*16 + 1; break; // sample length
            //~ case 0x11: ch.linear_counter = value & 0x7F; break; // dac value
            //~ case 0x15:
                //~ for(unsigned c=0; c<5; ++c)
                    //~ ChannelsEnabled[c] = value & (1 << c);
                //~ for(unsigned c=0; c<5; ++c)
                    //~ if(!ChannelsEnabled[c])
                        //~ channels[c].length_counter = 0;
                    //~ else if(c == 4 && channels[c].length_counter == 0)
                        //~ channels[c].length_counter = ch.reg.PCMlength*16 + 1;
                //~ break;
            //~ case 0x17:
                //~ IRQdisable       = value & 0x40;
                //~ FiveCycleDivider = value & 0x80;
                //~ hz240counter     = { 0,0 };
                //~ if(IRQdisable) PeriodicIRQ = DMC_IRQ = false;
        //~ }
    //~ }
    //~ u8 Read()
    //~ {
        //~ u8 res = 0;
        //~ for(unsigned c=0; c<5; ++c) res |= (channels[c].length_counter ? 1 << c : 0);
        //~ if(PeriodicIRQ) res |= 0x40; PeriodicIRQ = false;
        //~ if(DMC_IRQ)     res |= 0x80; DMC_IRQ     = false;
        //~ CPU::intr = false;
        //~ return res;
    //~ }
//~ 
    //~ void tick() // Invoked at CPU's rate.
    //~ {
        //~ // Divide CPU clock by 7457.5 to get a 240 Hz, which controls certain events.
        //~ if((hz240counter.lo += 2) >= 14915)
        //~ {
            //~ hz240counter.lo -= 14915;
            //~ if(++hz240counter.hi >= 4+FiveCycleDivider) hz240counter.hi = 0;
//~ 
            //~ // 60 Hz interval: IRQ. IRQ is not invoked in five-cycle mode (48 Hz).
            //~ if(!IRQdisable && !FiveCycleDivider && hz240counter.hi==0)
                //~ CPU::intr = PeriodicIRQ = true;
//~ 
            //~ // Some events are invoked at 96 Hz or 120 Hz rate. Others, 192 Hz or 240 Hz.
            //~ bool HalfTick = (hz240counter.hi&5)==1, FullTick = hz240counter.hi < 4;
            //~ for(unsigned c=0; c<4; ++c)
            //~ {
                //~ channel& ch = channels[c];
                //~ int wl = ch.reg.WaveLength;
//~ 
                //~ // Length tick (all channels except DMC, but different disable bit for triangle wave)
                //~ if(HalfTick && ch.length_counter
                //~ && !(c==2 ? ch.reg.LinearCounterDisable : ch.reg.LengthCounterDisable))
                    //~ ch.length_counter -= 1;
//~ 
                //~ // Sweep tick (square waves only)
                //~ if(HalfTick && c < 2 && count(ch.sweep_delay, ch.reg.SweepRate))
                    //~ if(wl >= 8 && ch.reg.SweepEnable && ch.reg.SweepShift)
                    //~ {
                        //~ int s = wl >> ch.reg.SweepShift, d[4] = {s, s, ~s, -s};
                        //~ wl += d[ch.reg.SweepDecrease*2 + c];
                        //~ if(wl < 0x800) ch.reg.WaveLength = wl;
                    //~ }
//~ 
                //~ // Linear tick (triangle wave only)
                //~ if(FullTick && c == 2)
                    //~ ch.linear_counter = ch.reg.LinearCounterDisable
                    //~ ? ch.reg.LinearCounterInit
                    //~ : (ch.linear_counter > 0 ? ch.linear_counter - 1 : 0);
//~ 
                //~ // Envelope tick (square and noise channels)
                //~ if(FullTick && c != 2 && count(ch.env_delay, ch.reg.EnvDecayRate))
                    //~ if(ch.envelope > 0 || ch.reg.EnvDecayLoopEnable)
                        //~ ch.envelope = (ch.envelope-1) & 15;
            //~ }
        //~ }
//~ 
        //~ // Mix the audio: Get the momentary sample from each channel and mix them.
        //~ #define s(c) channels[c].tick<c==1 ? 0 : c>()
        //~ auto v = [](float m,float n, float d) { return n!=0.f ? m/n : d; };
        //~ short sample = 30000 *
          //~ (v(95.88f,  (100.f + v(8128.f, s(0) + s(1), -100.f)), 0.f)
        //~ +  v(159.79f, (100.f + v(1.0, s(2)/8227.f + s(3)/12241.f + s(4)/22638.f, -100.f)), 0.f)
          //~ - 0.5f
          //~ );
        //~ #undef s
        //~ // I cheat here: I did not bother to learn how to use SDL mixer, let alone use it in <5 lines of code,
        //~ // so I simply use a combination of external programs for outputting the audio.
        //~ // Hooray for Unix principles! A/V sync will be ensured in post-process.
        //~ //return; // Disable sound because already device is in use
        //~ static FILE* fp = popen("resample mr1789800 r48000 | aplay -fdat 2>/dev/null", "w");
        //~ fputc(sample, fp);
        //~ fputc(sample/256, fp);
    //~ }
//~ }

/*
 * the 16x16 NES font.
 */
#include <types.h>

uint16_t nesfont_raw[] = {
0x7e0,0x7e0,0x7e0,0x7e0,0x7e0,0x7e0,0x780,0x780,0x780,0x780,0x0,0x0,0x780,0x780,0x0,0x0,
0x1e78,0x1e78,0x1e78,0x1e78,0x1e78,0x1e78,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x1e78,0x1e78,0x7ffe,0x7ffe,0x1e78,0x1e78,0x1e78,0x1e78,0x1e78,0x1e78,0x7ffe,0x7ffe,0x1e78,0x1e78,0x0,0x0,
0x180,0x180,0x1ff8,0x1ff8,0x7980,0x7980,0x1ff8,0x1ff8,0x19e,0x19e,0x7ff8,0x7ff8,0x180,0x180,0x0,0x0,
0x1e06,0x1e06,0x6618,0x6618,0x7860,0x7860,0x180,0x180,0x61e,0x61e,0x1866,0x1866,0x6078,0x6078,0x0,0x0,
0x1f80,0x1f80,0x79e0,0x79e0,0x79e0,0x79e0,0x1f80,0x1f80,0x79e6,0x79e6,0x7878,0x7878,0x1ffe,0x1ffe,0x0,0x0,
0x780,0x780,0x780,0x780,0x780,0x780,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x78,0x78,0x1e0,0x1e0,0x780,0x780,0x780,0x780,0x780,0x780,0x1e0,0x1e0,0x78,0x78,0x0,0x0,
0x1e00,0x1e00,0x780,0x780,0x1e0,0x1e0,0x1e0,0x1e0,0x1e0,0x1e0,0x780,0x780,0x1e00,0x1e00,0x0,0x0,
0x0,0x0,0x1e78,0x1e78,0x7e0,0x7e0,0x7ffe,0x7ffe,0x7e0,0x7e0,0x1e78,0x1e78,0x0,0x0,0x0,0x0,
0x0,0x0,0x1e0,0x1e0,0x1e0,0x1e0,0x1ffe,0x1ffe,0x1e0,0x1e0,0x1e0,0x1e0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x780,0x780,0x780,0x780,0x1e00,0x1e00,
0x0,0x0,0x0,0x0,0x0,0x0,0x1ffe,0x1ffe,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x780,0x780,0x780,0x780,0x0,0x0,
0x6,0x6,0x18,0x18,0x60,0x60,0x180,0x180,0x600,0x600,0x1800,0x1800,0x6000,0x6000,0x0,0x0,
0x7e0,0x7e0,0x1878,0x1878,0x781e,0x781e,0x781e,0x781e,0x781e,0x781e,0x1e18,0x1e18,0x7e0,0x7e0,0x0,0x0,
0x1e0,0x1e0,0x7e0,0x7e0,0x1e0,0x1e0,0x1e0,0x1e0,0x1e0,0x1e0,0x1e0,0x1e0,0x1ffe,0x1ffe,0x0,0x0,
0x1ff8,0x1ff8,0x781e,0x781e,0x7e,0x7e,0x7f8,0x7f8,0x1fe0,0x1fe0,0x7e00,0x7e00,0x7ffe,0x7ffe,0x0,0x0,
0x1ffe,0x1ffe,0x78,0x78,0x1e0,0x1e0,0x7f8,0x7f8,0x1e,0x1e,0x781e,0x781e,0x1ff8,0x1ff8,0x0,0x0,
0x1f8,0x1f8,0x7f8,0x7f8,0x1e78,0x1e78,0x7878,0x7878,0x7ffe,0x7ffe,0x78,0x78,0x78,0x78,0x0,0x0,
0x7ff8,0x7ff8,0x7800,0x7800,0x7ff8,0x7ff8,0x1e,0x1e,0x1e,0x1e,0x781e,0x781e,0x1ff8,0x1ff8,0x0,0x0,
0x7f8,0x7f8,0x1e00,0x1e00,0x7800,0x7800,0x7ff8,0x7ff8,0x781e,0x781e,0x781e,0x781e,0x1ff8,0x1ff8,0x0,0x0,
0x7ffe,0x7ffe,0x781e,0x781e,0x78,0x78,0x1e0,0x1e0,0x780,0x780,0x780,0x780,0x780,0x780,0x0,0x0,
0x1fe0,0x1fe0,0x7818,0x7818,0x7e18,0x7e18,0x1fe0,0x1fe0,0x61fe,0x61fe,0x601e,0x601e,0x1ff8,0x1ff8,0x0,0x0,
0x1ff8,0x1ff8,0x781e,0x781e,0x781e,0x781e,0x1ffe,0x1ffe,0x1e,0x1e,0x78,0x78,0x1fe0,0x1fe0,0x0,0x0,
0x0,0x0,0x780,0x780,0x780,0x780,0x0,0x0,0x780,0x780,0x780,0x780,0x0,0x0,0x0,0x0,
0x0,0x0,0x780,0x780,0x780,0x780,0x0,0x0,0x780,0x780,0x780,0x780,0x1e00,0x1e00,0x0,0x0,
0x78,0x78,0x1e0,0x1e0,0x780,0x780,0x1e00,0x1e00,0x780,0x780,0x1e0,0x1e0,0x78,0x78,0x0,0x0,
0x0,0x0,0x0,0x0,0x7ffe,0x7ffe,0x0,0x0,0x7ffe,0x7ffe,0x0,0x0,0x0,0x0,0x0,0x0,
0x1e00,0x1e00,0x780,0x780,0x1e0,0x1e0,0x78,0x78,0x1e0,0x1e0,0x780,0x780,0x1e00,0x1e00,0x0,0x0,
0x1ff8,0x1ff8,0x7ffe,0x7ffe,0x781e,0x781e,0x78,0x78,0x7e0,0x7e0,0x0,0x0,0x7e0,0x7e0,0x0,0x0,
0x1ff8,0x1ff8,0x6006,0x6006,0x67e6,0x67e6,0x6666,0x6666,0x67fe,0x67fe,0x6000,0x6000,0x1ff8,0x1ff8,0x0,0x0,
0x7e0,0x7e0,0x1e78,0x1e78,0x781e,0x781e,0x781e,0x781e,0x7ffe,0x7ffe,0x781e,0x781e,0x781e,0x781e,0x0,0x0,
0x7ff8,0x7ff8,0x781e,0x781e,0x781e,0x781e,0x7ff8,0x7ff8,0x781e,0x781e,0x781e,0x781e,0x7ff8,0x7ff8,0x0,0x0,
0x7f8,0x7f8,0x1e1e,0x1e1e,0x7800,0x7800,0x7800,0x7800,0x7800,0x7800,0x1e1e,0x1e1e,0x7f8,0x7f8,0x0,0x0,
0x7fe0,0x7fe0,0x7878,0x7878,0x781e,0x781e,0x781e,0x781e,0x781e,0x781e,0x7878,0x7878,0x7fe0,0x7fe0,0x0,0x0,
0x7ffe,0x7ffe,0x7800,0x7800,0x7800,0x7800,0x7ff8,0x7ff8,0x7800,0x7800,0x7800,0x7800,0x7ffe,0x7ffe,0x0,0x0,
0x7ffe,0x7ffe,0x7800,0x7800,0x7800,0x7800,0x7ff8,0x7ff8,0x7800,0x7800,0x7800,0x7800,0x7800,0x7800,0x0,0x0,
0x7fe,0x7fe,0x1e00,0x1e00,0x7800,0x7800,0x787e,0x787e,0x781e,0x781e,0x1e1e,0x1e1e,0x7fe,0x7fe,0x0,0x0,
0x781e,0x781e,0x781e,0x781e,0x781e,0x781e,0x7ffe,0x7ffe,0x781e,0x781e,0x781e,0x781e,0x781e,0x781e,0x0,0x0,
0x1ffe,0x1ffe,0x1e0,0x1e0,0x1e0,0x1e0,0x1e0,0x1e0,0x1e0,0x1e0,0x1e0,0x1e0,0x1ffe,0x1ffe,0x0,0x0,
0x1e,0x1e,0x1e,0x1e,0x1e,0x1e,0x1e,0x1e,0x1e,0x1e,0x781e,0x781e,0x1ff8,0x1ff8,0x0,0x0,
0x781e,0x781e,0x7878,0x7878,0x79e0,0x79e0,0x7f80,0x7f80,0x7fe0,0x7fe0,0x79f8,0x79f8,0x787e,0x787e,0x0,0x0,
0x1e00,0x1e00,0x1e00,0x1e00,0x1e00,0x1e00,0x1e00,0x1e00,0x1e00,0x1e00,0x1e00,0x1e00,0x1ffe,0x1ffe,0x0,0x0,
0x781e,0x781e,0x7e7e,0x7e7e,0x7ffe,0x7ffe,0x7ffe,0x7ffe,0x799e,0x799e,0x781e,0x781e,0x781e,0x781e,0x0,0x0,
0x781e,0x781e,0x7e1e,0x7e1e,0x7f9e,0x7f9e,0x7ffe,0x7ffe,0x79fe,0x79fe,0x787e,0x787e,0x781e,0x781e,0x0,0x0,
0x1ff8,0x1ff8,0x781e,0x781e,0x781e,0x781e,0x781e,0x781e,0x781e,0x781e,0x781e,0x781e,0x1ff8,0x1ff8,0x0,0x0,
0x7ff8,0x7ff8,0x781e,0x781e,0x781e,0x781e,0x781e,0x781e,0x7ff8,0x7ff8,0x7800,0x7800,0x7800,0x7800,0x0,0x0,
0x1ff8,0x1ff8,0x781e,0x781e,0x781e,0x781e,0x781e,0x781e,0x79fe,0x79fe,0x7878,0x7878,0x1fe6,0x1fe6,0x0,0x0,
0x7ff8,0x7ff8,0x781e,0x781e,0x781e,0x781e,0x787e,0x787e,0x7fe0,0x7fe0,0x79f8,0x79f8,0x787e,0x787e,0x0,0x0,
0x1fe0,0x1fe0,0x7878,0x7878,0x7800,0x7800,0x1ff8,0x1ff8,0x1e,0x1e,0x781e,0x781e,0x1ff8,0x1ff8,0x0,0x0,
0x1ffe,0x1ffe,0x1e0,0x1e0,0x1e0,0x1e0,0x1e0,0x1e0,0x1e0,0x1e0,0x1e0,0x1e0,0x1e0,0x1e0,0x0,0x0,
0x781e,0x781e,0x781e,0x781e,0x781e,0x781e,0x781e,0x781e,0x781e,0x781e,0x781e,0x781e,0x1ff8,0x1ff8,0x0,0x0,
0x781e,0x781e,0x781e,0x781e,0x781e,0x781e,0x7e7e,0x7e7e,0x1ff8,0x1ff8,0x7e0,0x7e0,0x180,0x180,0x0,0x0,
0x781e,0x781e,0x781e,0x781e,0x799e,0x799e,0x7ffe,0x7ffe,0x7ffe,0x7ffe,0x7e7e,0x7e7e,0x781e,0x781e,0x0,0x0,
0x781e,0x781e,0x7e7e,0x7e7e,0x1ff8,0x1ff8,0x7e0,0x7e0,0x1ff8,0x1ff8,0x7e7e,0x7e7e,0x781e,0x781e,0x0,0x0,
0x1e1e,0x1e1e,0x1e1e,0x1e1e,0x1e1e,0x1e1e,0x7f8,0x7f8,0x1e0,0x1e0,0x1e0,0x1e0,0x1e0,0x1e0,0x0,0x0,
0x7ffe,0x7ffe,0x7e,0x7e,0x1f8,0x1f8,0x7e0,0x7e0,0x1f80,0x1f80,0x7e00,0x7e00,0x7ffe,0x7ffe,0x0,0x0,
0x7f8,0x7f8,0x780,0x780,0x780,0x780,0x780,0x780,0x780,0x780,0x780,0x780,0x7f8,0x7f8,0x0,0x0,
0x6000,0x6000,0x1800,0x1800,0x600,0x600,0x180,0x180,0x60,0x60,0x18,0x18,0x6,0x6,0x0,0x0,
0x1fe0,0x1fe0,0x1e0,0x1e0,0x1e0,0x1e0,0x1e0,0x1e0,0x1e0,0x1e0,0x1e0,0x1e0,0x1fe0,0x1fe0,0x0,0x0,
0x7e0,0x7e0,0x1e78,0x1e78,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x7ffe,0x7ffe,
0x180,0x180,0x60,0x60,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x1ff8,0x1ff8,0x1e,0x1e,0x1ffe,0x1ffe,0x781e,0x781e,0x1ffe,0x1ffe,0x0,0x0,
0x7800,0x7800,0x7800,0x7800,0x7ff8,0x7ff8,0x781e,0x781e,0x781e,0x781e,0x781e,0x781e,0x1ff8,0x1ff8,0x0,0x0,
0x0,0x0,0x0,0x0,0x1ffe,0x1ffe,0x7800,0x7800,0x7800,0x7800,0x7800,0x7800,0x1ffe,0x1ffe,0x0,0x0,
0x1e,0x1e,0x1e,0x1e,0x1ffe,0x1ffe,0x781e,0x781e,0x781e,0x781e,0x781e,0x781e,0x1ffe,0x1ffe,0x0,0x0,
0x0,0x0,0x0,0x0,0x1ff8,0x1ff8,0x781e,0x781e,0x7ffe,0x7ffe,0x7800,0x7800,0x1ff8,0x1ff8,0x0,0x0,
0x7e,0x7e,0x1e0,0x1e0,0x1ffe,0x1ffe,0x1e0,0x1e0,0x1e0,0x1e0,0x1e0,0x1e0,0x1e0,0x1e0,0x0,0x0,
0x0,0x0,0x0,0x0,0x1ffe,0x1ffe,0x781e,0x781e,0x781e,0x781e,0x1ffe,0x1ffe,0x1e,0x1e,0x1ff8,0x1ff8,
0x7800,0x7800,0x7800,0x7800,0x7ff8,0x7ff8,0x781e,0x781e,0x781e,0x781e,0x781e,0x781e,0x781e,0x781e,0x0,0x0,
0x1e0,0x1e0,0x0,0x0,0x7e0,0x7e0,0x1e0,0x1e0,0x1e0,0x1e0,0x1e0,0x1e0,0x1ffe,0x1ffe,0x0,0x0,
0x78,0x78,0x0,0x0,0x1f8,0x1f8,0x78,0x78,0x78,0x78,0x78,0x78,0x78,0x78,0x1fe0,0x1fe0,
0x7800,0x7800,0x7800,0x7800,0x787e,0x787e,0x7ff8,0x7ff8,0x7fe0,0x7fe0,0x79f8,0x79f8,0x787e,0x787e,0x0,0x0,
0x7e0,0x7e0,0x1e0,0x1e0,0x1e0,0x1e0,0x1e0,0x1e0,0x1e0,0x1e0,0x1e0,0x1e0,0x1ffe,0x1ffe,0x0,0x0,
0x0,0x0,0x0,0x0,0x7ff8,0x7ff8,0x679e,0x679e,0x679e,0x679e,0x679e,0x679e,0x679e,0x679e,0x0,0x0,
0x0,0x0,0x0,0x0,0x7ff8,0x7ff8,0x781e,0x781e,0x781e,0x781e,0x781e,0x781e,0x781e,0x781e,0x0,0x0,
0x0,0x0,0x0,0x0,0x1ff8,0x1ff8,0x781e,0x781e,0x781e,0x781e,0x781e,0x781e,0x1ff8,0x1ff8,0x0,0x0,
0x0,0x0,0x0,0x0,0x7ff8,0x7ff8,0x781e,0x781e,0x781e,0x781e,0x7ff8,0x7ff8,0x7800,0x7800,0x7800,0x7800,
0x0,0x0,0x0,0x0,0x1ffe,0x1ffe,0x781e,0x781e,0x781e,0x781e,0x1ffe,0x1ffe,0x1e,0x1e,0x1e,0x1e,
0x0,0x0,0x0,0x0,0x1e7e,0x1e7e,0x1f80,0x1f80,0x1e00,0x1e00,0x1e00,0x1e00,0x1e00,0x1e00,0x0,0x0,
0x0,0x0,0x0,0x0,0x1ff8,0x1ff8,0x7800,0x7800,0x1ff8,0x1ff8,0x1e,0x1e,0x7ff8,0x7ff8,0x0,0x0,
0x1e0,0x1e0,0x1e0,0x1e0,0x1ffe,0x1ffe,0x1e0,0x1e0,0x1e0,0x1e0,0x1e0,0x1e0,0x1e0,0x1e0,0x0,0x0,
0x0,0x0,0x0,0x0,0x781e,0x781e,0x781e,0x781e,0x781e,0x781e,0x781e,0x781e,0x1ffe,0x1ffe,0x0,0x0,
0x0,0x0,0x0,0x0,0x1e1e,0x1e1e,0x1e1e,0x1e1e,0x1e1e,0x1e1e,0x7f8,0x7f8,0x1e0,0x1e0,0x0,0x0,
0x0,0x0,0x0,0x0,0x679e,0x679e,0x679e,0x679e,0x679e,0x679e,0x679e,0x679e,0x1ffe,0x1ffe,0x0,0x0,
0x0,0x0,0x0,0x0,0x781e,0x781e,0x7ffe,0x7ffe,0x7e0,0x7e0,0x7ffe,0x7ffe,0x781e,0x781e,0x0,0x0,
0x0,0x0,0x0,0x0,0x781e,0x781e,0x781e,0x781e,0x781e,0x781e,0x1ffe,0x1ffe,0x1e,0x1e,0x1ff8,0x1ff8,
0x0,0x0,0x0,0x0,0x7ffe,0x7ffe,0x1f8,0x1f8,0x7e0,0x7e0,0x1f80,0x1f80,0x7ffe,0x7ffe,0x0,0x0,
0x78,0x78,0x1e0,0x1e0,0x1e0,0x1e0,0x780,0x780,0x1e0,0x1e0,0x1e0,0x1e0,0x78,0x78,0x0,0x0,
0x1e0,0x1e0,0x1e0,0x1e0,0x1e0,0x1e0,0x1e0,0x1e0,0x1e0,0x1e0,0x1e0,0x1e0,0x1e0,0x1e0,0x0,0x0,
0x1e00,0x1e00,0x780,0x780,0x780,0x780,0x1e0,0x1e0,0x780,0x780,0x780,0x780,0x1e00,0x1e00,0x0,0x0,
0x0,0x0,0x0,0x0,0x1f80,0x1f80,0x67e6,0x67e6,0x1f8,0x1f8,0x0,0x0,0x0,0x0,0x0,0x0};








const unsigned int nesfont_raw_len = sizeof(nesfont_raw)/sizeof(nesfont_raw[0]);

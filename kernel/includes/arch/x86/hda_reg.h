
#ifndef _INCLUDE_DEV_SND_HDAREG_H_91290A0AF1D311E287DF74E50BEE6214
#define _INCLUDE_DEV_SND_HDAREG_H_91290A0AF1D311E287DF74E50BEE6214

#define HDA_REG_GCAP      0x00    // Global Capabilities (2)
#define HDA_REG_VMIN      0x02    // Minor Version (1)
#define HDA_REG_VMAJ      0x03    // Major Version (1)
#define HDA_REG_GCTL      0x08    // Global Control (4)
#define HDA_REG_INTCTL    0x20    // Interrupt Control (4)

#define HDA_REG_CORBLBASE 0x40    // CORB Lower Base Address (4)
#define HDA_REG_CORBUBASE 0x44    // CORB Upper Base Address (4)
#define HDA_REG_CORBWP    0x48    // CORB Write Pointer (2)
#define HDA_REG_CORBRP    0x4A    // CORB Read Pointer (2)
#define HDA_REG_CORBCTL   0x4C    // CORB Control (1)
#define HDA_REG_CORBSTS   0x4D    // CORB Status (1)
#define HDA_REG_CORBSIZE  0x4E    // CORB Size (1)

#define HDA_REG_RIRBLBASE 0x50    // RIRB Lower Base Address (4)
#define HDA_REG_RIRBUBASE 0x54    // RIRB Upper Base Address (4)
#define HDA_REG_RIRBWP    0x58    // RIRB Write Pointer (2)
#define HDA_REG_RIRBCNT   0x5A    // Response Interrupt Count (2)
#define HDA_REG_RIRBCTL   0x5C    // RIRB Control (1)
#define HDA_REG_RIRBSTS   0x5D    // RIRB Status (1)
#define HDA_REG_RIRBSIZE  0x5E    // RIRB Size (1)

#define HDA_REG_IOCI      0x60    // Immediate Command Output Interface (4)
#define HDA_REG_ICII      0x64    // Immediate Command Input Interface (4)
#define HDA_REG_ICIS      0x68    // Immediate Command Status (2)

#define HDA_REG_DPIBLBASE 0x70    // DMA Position Buffer Lower Base (4)
#define HDA_REG_DPIBUBASE 0x74    // DMA Position Buffer Lower Base (4)

// Base offset for Nth iss
#define HDA_REG_ISDBASE(N)  (0x80 + (N) * 0x20)
// Base offset for Nth oss
#define HDA_REG_OSDBASE(N)  (0x80 + hda.cap.niss * 0x20 + (N) * 0x20)

// OSS offsets
#define HDA_REG_OSDCTL(N)   (HDA_REG_OSDBASE(N)+0x0)  // Output Stream Descriptor (3)
#define HDA_REG_OSDSTS(N)   (HDA_REG_OSDBASE(N)+0x3)  // OSD(N) Status (1)
#define HDA_REG_OSDLPIB(N)  (HDA_REG_OSDBASE(N)+0x4)  // OSD(N) Link Position in Current Buffer (4)
#define HDA_REG_OSDCBL(N)   (HDA_REG_OSDBASE(N)+0x8)  // OSD(N) Cyclic Buffer Length (4)
#define HDA_REG_OSDLVI(N)   (HDA_REG_OSDBASE(N)+0xC)  // OSD(N) Last Valid Index (2)
#define HDA_REG_OSDFIFOS(N) (HDA_REG_OSDBASE(N)+0x10) // OSD(N) FIFO Size (2)
#define HDA_REG_OSDFMT(N)   (HDA_REG_OSDBASE(N)+0x12) // OSD(N) Format (2)
#define HDA_REG_OSDBDPL(N)  (HDA_REG_OSDBASE(N)+0x18) // OSD(N) Buffer Descriptor List Pointer [Lower] (4)
#define HDA_REG_OSDBDPU(N)  (HDA_REG_OSDBASE(N)+0x1C) // OSD(N) Buffer Descriptor List Pointer [Upper] (4)

// Stream link position offsets.
#define HDA_REG_SD1LPIBA    0x20A4

#endif // _INCLUDE_DEV_SND_HDAREG_H_91290A0AF1D311E287DF74E50BEE6214

// @file: kern/dev/snd/snd_hda.c
// Copyright (c) 2013 Korepwx. All rights reserved.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Contributors:
//   Korepwx  <public@korepwx.com>  2013-07-20
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#include <types.h>
#include <arch/x86/barrier.h>
#include <arch/x86/hda_reg.h>
#include <arch/x86/pci/registry.h>
#include <arch/x86/pci/pci.h>
#include <arch/x86/pci/driver.h>
#include <arch/x86/ports.h>
#include <sys/logging.h>
#include <sys/timer.h>
#include <sys/memory.h>
#include <stdlib.h>
#include <string.h>
#include <graphics/video.h>
// ---- HDA device database ----
typedef struct {
	uint16_t vendor;
	uint16_t device;
} HDADeviceModel;

static HDADeviceModel hda_device_model[] = {
	{ 0x8086, 0x2668 },
	{ 0x8086, 0x27D8 },
	{ 0x8086, 0x1c20 },
	{ 0     , 0      }
};

// ---- HDA device interface ----
typedef struct {
	// Actually address is 64bit in little endian.
	uint32_t addr_low;
	uint32_t addr_high;
	
	// Buffer size.
	uint32_t size;
	
	// Buffer control (bit 1 is IOC flag).
	uint32_t flag;
} BufferDescriptor;

typedef struct {
	// Basic PCI Device on bus.
	PciDeviceInfo *pci;
	
	// IO mapped control base address.
	volatile void*   ctrl_addr;
	size_t  ctrl_len;
	
	// Query about the device capacity.
	struct {
		uint8_t noss;
		uint8_t niss;
		uint8_t nbss;
		uint8_t nsdo;
		uint8_t f64b;
	} cap;
	
	// Buffers allocated.
	void*     cmd_buf;    // FOR CORB & RIRB.
	
	// CORB buffer.
	uint32_t* corb_addr;
	uint16_t  corb_size;
	
	// RIRB buffer.
	uint64_t* rirb_addr;
	uint16_t  rirb_size;
	uint8_t   rirb_readptr;
	
	// OSS flags.
	struct {
		// Buffer descriptors.
		BufferDescriptor *buf;
		size_t buflen;
	} stm[1];
	
} HDADevice;
static HDADevice hda = {0};

// ---- hda register IO ----

#define SET_BIT(VAL, NBIT)    { *(VAL) |= (1 << (NBIT)); }
#define UNSET_BIT(VAL, NBIT)  { *(VAL) &= ~(1 << (NBIT)); }
#define GET_BIT(VAL, NBIT) ( !!((VAL) & (1 << (NBIT))) )

#define HDA_REG_READ(OFFSET, TYPE)  \
	(*((volatile TYPE *)(hda.ctrl_addr + (HDA_REG_ ## OFFSET))))
	
#define HDA_REG_WRITE(OFFSET, TYPE, VAL) do { \
	*((volatile TYPE*)(hda.ctrl_addr + (HDA_REG_ ## OFFSET))) = (VAL); \
	} while (0)

#define HDA_REG_READ_U8(OFFSET)         HDA_REG_READ(OFFSET, uint8_t)
#define HDA_REG_READ_U16(OFFSET)        HDA_REG_READ(OFFSET, uint16_t)
#define HDA_REG_READ_U32(OFFSET)        HDA_REG_READ(OFFSET, uint32_t)
#define HDA_REG_WRITE_U8(OFFSET, VAL)   HDA_REG_WRITE(OFFSET, uint8_t, (VAL))
#define HDA_REG_WRITE_U16(OFFSET, VAL)  HDA_REG_WRITE(OFFSET, uint16_t, (VAL))
#define HDA_REG_WRITE_U32(OFFSET, VAL)  HDA_REG_WRITE(OFFSET, uint32_t, VAL)

void io_delay(){
	//TODO
	sleep(10);
}
#define HDA_WAIT_UNTIL_WRITTEN(OFFSET, TYPE, VAL) do { \
	int repeat; \
	HDA_REG_WRITE(OFFSET, TYPE, (VAL)); \
	printk("hda", "%x -> %x\n", HDA_REG_READ(OFFSET, TYPE), VAL); \
	video_flush_console();\
	for (repeat=0; repeat<10; ++repeat) { \
		if (HDA_REG_READ(OFFSET, TYPE) != (VAL)) \
			io_delay(); \
	} \
	assert(HDA_REG_READ(OFFSET, TYPE) == (VAL)); \
} while (0);
#define HDA_WAIT_UNTIL_WRITTEN_U8(OFFSET, VAL) \
	HDA_WAIT_UNTIL_WRITTEN(OFFSET, uint8_t, (VAL))
#define HDA_WAIT_UNTIL_WRITTEN_U16(OFFSET, VAL) \
	HDA_WAIT_UNTIL_WRITTEN(OFFSET, uint16_t, (VAL))
#define HDA_WAIT_UNTIL_WRITTEN_U32(OFFSET, VAL) \
	HDA_WAIT_UNTIL_WRITTEN(OFFSET, uint32_t, (VAL))

// ---- Create a default output stream with two buffer, 44.1khz, 2channels ----
static void hda_create_stream(void* audio, size_t size)
{
	uint32_t u32;
	uint16_t u16;
	//~ uint8_t u8;
	int i;
	
	// Set stream count.
	u32 = HDA_REG_READ_U32(OSDCTL(0));  // 3 bytes, little endian.
	u32 |= (0x1 << 20);                 // stream id.
	UNSET_BIT(&u32, 1);                 // set this stream not running.
	HDA_WAIT_UNTIL_WRITTEN_U32(OSDCTL(0), u32);
	
	// Set stream format
	u16 = HDA_REG_READ_U16(OSDFMT(0));
	u16 &= 0x80; // Only one bit should be preserved, other bits clear zero.
	SET_BIT(&u16, 14); // base sample rate. 0=48khz, 1=44.1khz.
	u16 |= (0x1 << 4); // Bits per sample. 000=8, 001=16, 010=20, 011=24, 100=32.
	u16 |= 0x0; // channels. 0000=1, 0001=2, ..., 1111=16.
	HDA_WAIT_UNTIL_WRITTEN_U16(OSDFMT(0), u16); // This is very important!
	
	// Allocate stream buffers.
	assert(sizeof(BufferDescriptor) == 16);
	hda.stm[0].buf = (BufferDescriptor*)liballoc_alloc(1);
	hda.stm[0].buflen = 256; // Maximum 256 descriptors; also this is one page.
	memset(hda.stm[0].buf, 0, sizeof(hda.stm[0].buf[0]) * hda.stm[0].buflen);
	
	// Assign these buffers to stream.
	HDA_REG_WRITE_U32(OSDBDPU(0), 0x0);
	HDA_WAIT_UNTIL_WRITTEN_U32(OSDBDPL(0), (uintptr_t)hda.stm[0].buf);
	
	// Two buffers are required for the DMA engine.
	HDA_REG_WRITE_U16(OSDLVI(0), 0x1);
	assert(((size_t)audio % 128) == 0);
	assert((size % 128) == 0);
	
	for (i=0; i<2; ++i) {
		hda.stm[0].buf[i].addr_low = (uintptr_t)audio & 0xFFFFFFFF;
		hda.stm[0].buf[i].addr_high = 0;
		hda.stm[0].buf[i].size = size;
	}
	
	// Set the packet count of buffers.
	HDA_WAIT_UNTIL_WRITTEN_U32(OSDCBL(0), 
		size / 2 /* 16-bit */ );
	
	// Turn on the stream.
	u32 = HDA_REG_READ_U32(OSDCTL(0));
	SET_BIT(&u32, 1);
	HDA_WAIT_UNTIL_WRITTEN_U32(OSDCTL(0), u32);
	barrier();
	
	//~ while(1){
		//~ u32 = HDA_REG_READ_U32(OSDLPIB(0));  // 3 bytes, little endian.
		//~ uint32_t u322 = HDA_REG_READ_U32(OSDCTL(0));
		//~ u8 = HDA_REG_READ_U8(OSDSTS(0));  // 3 bytes, little endian.
		//~ printk("hda", "%x 0x%x\n", u32, (int)u322 &((1<<25)-1));
		//~ printk("hda", "%d\n", u32);
	//~ }
}


static void hda_test_audio(void)
{
	//TODO
//~ #define SECTSIZE 512
//~ #define BADAPPLE_AUDIO_DATA_SIZE   75502
	//~ badapple_detect_disk();
	//~ int page_count = BADAPPLE_AUDIO_DATA_SIZE * SECTSIZE / PGSIZE;
	//~ uint8_t *audio_data = page2kva(kalloc_pages(page_count));
	//~ kdisk_read_secs(ideno, audio_data, BADAPPLE_AUDIO_DATA_SIZE / 32, ideoff + 1);

	const int sz=999936;
	//~ uint8_t *audio_data =malloc(sz);
	uint8_t *audio_data = (uint8_t*)liballoc_alloc((sz+4095)/4096);
	for(int i=0; i<sz; i++) audio_data[i]=rand()%256;
	printk("hda",  "read sample input.\n");
	video_flush_console();
	hda_create_stream(audio_data, sz);
	printk("hda",  "sended wav into output.\n");
	video_flush_console();
}

// ---- Initialize the snd_hda device ----
static void hda_setup(void)
{
	int i;
	uint8_t u8;
	uint16_t u16;
	uint32_t u32;
	
	// Check the version.
	assert(HDA_REG_READ_U8(VMIN) == 0);
	assert(HDA_REG_READ_U8(VMAJ) == 1);
	
	// Turn on CRST bit.
	u32 = HDA_REG_READ_U32(GCTL);
	if(GET_BIT(u32, 0) == 0) {
		SET_BIT(&u32, 0);
		barrier();
		HDA_REG_WRITE_U32(GCTL, u32);
		barrier();
	}
	
	for (i=0; i<10; ++i) {
		if (GET_BIT(HDA_REG_READ_U32(GCTL), 0) == 1)
			break;
		io_delay();
	}
	
	if (GET_BIT(HDA_REG_READ_U32(GCTL), 0) == 0)
		panic("[hda] Cannot turn on HDA device.");
	
	// Wait for 521 us (that's how long?).
	io_delay();
	
	// ---- Detect Capability ----
	u16 = HDA_REG_READ_U16(GCAP);
	hda.cap.noss = (u16 >> 12)  & 0xf;
	hda.cap.niss = (u16 >> 8)   & 0xf;
	hda.cap.nbss = (u16 >> 3)   & 0x1f;
	hda.cap.nsdo = (u16 >> 1)   & 0x3;
	hda.cap.f64b = (u16)        & 0x1;
	hda.cap.nsdo = (1 << hda.cap.nsdo);
	
	printk("hda", "HDA GCAP oss(%d) iss(%d) bss(%d) sdo(%d) f64(%d).\n",
				 hda.cap.noss, hda.cap.niss, hda.cap.nbss, hda.cap.nsdo,
				 hda.cap.f64b);
	
	// ---- Setup CORB ----
	// Stop CORB & RIRB DMA if it is running
	u8 = HDA_REG_READ_U8(CORBCTL);
	if (GET_BIT(u8, 1) == 1) {
		UNSET_BIT(&u8, 1);
		HDA_REG_WRITE_U8(CORBCTL, u8);  // Stop CORB DMA
		io_delay();
	}
	barrier();
	u8 = HDA_REG_READ_U8(RIRBCTL);
	if (GET_BIT(u8, 1) == 1) {
		UNSET_BIT(&u8, 1);
		HDA_REG_WRITE_U8(RIRBCTL, u8);  // Stop RIRB DMA
		io_delay();
	}
	
	// Buffer for CORB & RIRB
	hda.cmd_buf = (void *)liballoc_alloc(1);
	hda.corb_addr = hda.cmd_buf + 0x0;
	hda.rirb_addr = hda.cmd_buf + 0x400;
	hda.corb_size = 256;
	hda.rirb_size = 256;
	hda.rirb_readptr = 0;
	
	// Setup CORB.
	barrier();
	HDA_REG_WRITE_U8(CORBSIZE, (HDA_REG_READ_U8(CORBSIZE) & 0xfc) | 0x2);  // Set CORB to use 1KB memory.
	HDA_REG_WRITE_U32(CORBLBASE, (uintptr_t)hda.corb_addr);
	HDA_REG_WRITE_U32(CORBUBASE, 0x0);
	
	barrier();
	u16 = HDA_REG_READ_U16(CORBRP);
	SET_BIT(&u16, 15);
	HDA_REG_WRITE_U16(CORBRP, u16);
	HDA_REG_WRITE_U32(CORBWP, 0x0);
	
	// Setup RIRB.
	barrier();
	HDA_REG_WRITE_U8(RIRBSIZE, (HDA_REG_READ_U8(RIRBSIZE) & 0xfc) | 0x2);
	HDA_REG_WRITE_U32(RIRBLBASE, (uintptr_t)(hda.rirb_addr));
	HDA_REG_WRITE_U32(RIRBUBASE, 0x0);
	
	barrier();
	u16 = HDA_REG_READ_U16(RIRBWP);
	SET_BIT(&u16, 15);
	HDA_REG_WRITE_U16(RIRBWP, u16);
	
	
	barrier();
	u8 = HDA_REG_READ_U8(RIRBCTL);
	UNSET_BIT(&u8, 0);
	HDA_REG_WRITE_U8(RIRBCTL, u8);
	
	// Verify the device settings.
	barrier();
	//~ return;
	assert(GET_BIT(HDA_REG_READ_U8(CORBCTL), 1) == 0); // CORB DMA still stopped.
	assert(GET_BIT(HDA_REG_READ_U8(RIRBCTL), 1) == 0); // RIRB DMA still stopped.
	assert((HDA_REG_READ_U8(CORBSIZE) & 0x3) == 0x2); // CORB size is set to 256
	assert((HDA_REG_READ_U8(RIRBSIZE) & 0x3) == 0x2); // RIRB size is set to 256
	 // CORB buffer
	assert(HDA_REG_READ_U32(CORBLBASE) == (uintptr_t)(hda.corb_addr)); // CORB buffer
	assert(HDA_REG_READ_U32(RIRBLBASE) == (uintptr_t)(hda.rirb_addr)); // RIRB buffer
	
//~ #if 0
	// Now it's time to enable CORB and RIRB
	u8 = HDA_REG_READ_U8(CORBCTL);
	SET_BIT(&u8, 1);
	HDA_REG_WRITE_U8(CORBCTL, u8);
	barrier();
	u8 = HDA_REG_READ_U8(RIRBCTL);
	SET_BIT(&u8, 1);
	HDA_REG_WRITE_U8(RIRBCTL, u8);
	barrier();
//~ #endif
	
	// Test audio output.
	hda_test_audio();
}

void HdaInit(unsigned id, PciDeviceInfo *info){
	hda.pci=0;
	for(HDADeviceModel* m = hda_device_model; m->vendor; m++){
		if (m->vendor == info->vendorId && m->device == info->deviceId) {
			hda.pci = info;
			break;
		}
	}
	// If no sound device discovered exit.
	if (!hda.pci) {
		return;
	}
	
	memset(&hda, 0, sizeof(hda));
	
	printk("hda", "Picked something as HDA sound device.\n");
	
	
    // Base I/O Address
    PciBar bar;
    PciGetBar(&bar, id, 0);
    if (bar.flags & PCI_BAR_IO){
		printk("hda", "Error: Only Memory Mapped I/O supported\n");
		return;
    }
	
	volatile u8 *bar0 = bar.u.address;
	// assert(bar0 == ctrl_addr);
	//~ barrier();
	//~ pci_write_config(hda.pci, 0x10, 0xFFFFFFFFU);
	//~ siz0 = (~pci_read_config(hda.pci, 0x10)) + 1;
	//~ assert(siz0 != 1);
	//~ barrier();
	//~ pci_write_config(hda.pci, 0x10, bar0);
	
	// Setup global page table.
	// Some note on this: BIOS set a mapped memory for PCI device  
	//   (generally at in-available memory).  Change the base address register 
	//   may not work.  So I "hack" the pcore page table (which is the same as 
	//   ucore) to directly translate these high memories into physical address.
	hda.ctrl_addr = bar0;
	hda.ctrl_len  = bar.size;
	printk("hda", "control base 0x%08x, size %u\n", (size_t)hda.ctrl_addr, bar.size);
	
	// Initialize the HDA device.
	hda_setup();
}

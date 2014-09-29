#include <types.h>
#include <string.h>
#include <sys/logging.h>
#include <fs/disk.h>
#include <stdlib.h>
#include <arch/x86/descriptor_tables.h>
#include <arch/x86/ata.h>
#include <arch/x86/ports.h>

// D2203
static int ata_drivers_loaded = 0;

#define	ATA_WAIT_TIMEOUT		200

// Command/status port bit masks
#define ATA_SR_BSY				0x80
#define ATA_SR_DRDY				0x40
#define ATA_SR_DF				0x20
#define ATA_SR_DSC				0x10
#define ATA_SR_DRQ				0x08
#define ATA_SR_CORR				0x04
#define ATA_SR_IDX				0x02
#define ATA_SR_ERR				0x01

// Features/error port bit masks
#define ATA_ER_BBK				0x80
#define ATA_ER_UNC				0x40
#define ATA_ER_MC				0x20
#define ATA_ER_IDNF				0x10
#define ATA_ER_MCR				0x08
#define ATA_ER_ABRT				0x04
#define ATA_ER_TK0NF			0x02
#define ATA_ER_AMNF				0x01

// Commands for ATA devices
#define ATA_CMD_READ_PIO		0x20
#define ATA_CMD_READ_PIO_EXT	0x24
#define ATA_CMD_READ_DMA		0xC8
#define ATA_CMD_READ_DMA_EXT	0x25
#define ATA_CMD_WRITE_PIO		0x30
#define ATA_CMD_WRITE_PIO_EXT	0x34
#define ATA_CMD_WRITE_DMA		0xCA
#define ATA_CMD_WRITE_DMA_EXT	0x35
#define ATA_CMD_CACHE_FLUSH		0xE7
#define ATA_CMD_CACHE_FLUSH_EXT	0xEA
#define ATA_CMD_PACKET			0xA0
#define ATA_CMD_IDENTIFY_PACKET	0xA1
#define ATA_CMD_IDENTIFY		0xEC

// Offsets into the ATA IDENTIFY chunk (in bytes)
#define ATA_IDENT_DEVICETYPE	0
#define ATA_IDENT_CYLINDERS		2
#define ATA_IDENT_HEADS			6
#define ATA_IDENT_SECTORS		12
#define ATA_IDENT_SERIAL		20
#define ATA_IDENT_MODEL			54
#define ATA_IDENT_CAPABILITIES	98
#define ATA_IDENT_FIELDVALID	106
#define ATA_IDENT_MAX_LBA		120
#define ATA_IDENT_SWDMASUPPORT	124
#define ATA_IDENT_MWDMASUPPORT	126
#define ATA_IDENT_PIOSUPPORT	128
#define ATA_IDENT_PIOCYC		134
#define ATA_IDENT_PIOCYCIORDY	136
#define ATA_IDENT_COMMANDSETS	164
#define ATA_IDENT_UDMASUPPORT	176
#define ATA_IDENT_MAX_LBA_EXT	200

// Registers accepted by the ata_reg_write and ata_reg_read functions
#define ATA_REG_DATA			0x00
#define ATA_REG_ERROR			0x01
#define ATA_REG_FEATURES		0x01
#define ATA_REG_SECCOUNT0		0x02
#define ATA_REG_LBA0			0x03
#define ATA_REG_LBA1			0x04
#define ATA_REG_LBA2			0x05
#define ATA_REG_HDDEVSEL		0x06
#define ATA_REG_COMMAND			0x07
#define ATA_REG_STATUS			0x07
#define ATA_REG_SECCOUNT1		0x08
#define ATA_REG_LBA3			0x09
#define ATA_REG_LBA4			0x0A
#define ATA_REG_LBA5			0x0B
#define ATA_REG_CONTROL			0x0C
#define ATA_REG_ALTSTATUS		0x0C
#define ATA_REG_DEVADDRESS		0x0D

// Private functions
static uint8_t ata_reg_read(ata_driver_t *drv, uint8_t channel, uint8_t reg);
static void ata_reg_write(ata_driver_t *drv, unsigned char channel, unsigned char reg, unsigned char data);
static void ata_do_pio_read(ata_driver_t *drv, void* dest, uint32_t num_words, uint8_t channel);
static int ata_access_pio(ata_driver_t *drv, uint8_t rw, uint8_t drive, uint32_t lba, uint8_t numsects, void *buf);

static ata_driver_t *ata;

void ata_driver_init(disk_t* disk) {
	disk->reset = ata_reset;
	disk->init = ata_init;
	disk->read = ata_read;
	disk->write = ata_write;
	disk->flush_cache = ata_flush_cache;
	disk->erase = ata_erase;
}

/*
 * Initialises the driver, based on the base addresses from the PCI controller
 */
ata_driver_t* ata_init_pci(uint32_t BAR0, uint32_t BAR1, uint32_t BAR2, uint32_t BAR3, uint32_t BAR4) {
	int k, count = 0;
	int channel, device;

	// Set up memory for the driver
	ata_driver_t* driver = (ata_driver_t *) malloc(sizeof(ata_driver_t));

	// Initialise memory if needed
	if(driver) {
		memset(driver, 0, sizeof(ata_driver_t));
	} else {
		return NULL;
	}

	// Allocate some buffer memory
	uint8_t *ide_buf = (uint8_t *) malloc(2048);
	memset(ide_buf, 0, 2048);

	// Copy the BAR addresses
	driver->BAR0 = BAR0;
	driver->BAR1 = BAR1;
	driver->BAR2 = BAR2;
	driver->BAR3 = BAR3;
	driver->BAR4 = BAR4;

	// Set up IO ports to control the IDE controller with
	driver->channels[ATA_PRIMARY].base = (BAR0 & 0xFFFFFFFC) + 0x1F0 * (!BAR0);
	driver->channels[ATA_PRIMARY].ctrl = (BAR1 & 0xFFFFFFFC) + 0x3F6 * (!BAR1);
	driver->channels[ATA_PRIMARY].bmide = (BAR4 & 0xFFFFFFFC) + 0; // Bus Master IDE
	driver->channels[ATA_SECONDARY].base = (BAR2 & 0xFFFFFFFC) + 0x170 * (!BAR2);
	driver->channels[ATA_SECONDARY].ctrl = (BAR3 & 0xFFFFFFFC) + 0x376 * (!BAR3);
	driver->channels[ATA_SECONDARY].bmide = (BAR4 & 0xFFFFFFFC) + 8; // Bus Master IDE

	// Disable IRQs for both channels
	ata_reg_write(driver, ATA_PRIMARY, ATA_REG_CONTROL, driver->channels[ATA_PRIMARY].nIEN = 0x02);
	ata_reg_write(driver, ATA_SECONDARY, ATA_REG_CONTROL, driver->channels[ATA_SECONDARY].nIEN = 0x02);

	// Probe for devices
	for(channel = 0; channel < 2; channel++) {
		// Disenable IRQs for the channel
		driver->channels[channel].nIEN = 2;

		// Check each device on the channel
		for(device = 0; device < 2; device++) {
			uint8_t type = ATA_DEVICE_TYPE_ATA, status;
			bool isATAPI = false;

			// Set up some initial state
			driver->devices[count].drive_exists = false;
			driver->devices[count].dma_enabled = false;
 
 			// Select drive
			ata_reg_write(driver, channel, ATA_REG_HDDEVSEL, 0xA0 | (device << 4));
 
			// Send IDENTIFY command
			ata_reg_write(driver, channel, ATA_REG_COMMAND, ATA_CMD_IDENTIFY);

			// Wait a while by writing to a bogus IO port
 			io_wait();
 			io_wait();

 			// If the status register is 0, there is no device
			if(ata_reg_read(driver, channel, ATA_REG_STATUS) == 0) continue;
 
 			// Wait until we read an error or DREQ
			for(;;) {
				// Read status register
				status = ata_reg_read(driver, channel, ATA_REG_STATUS);

				// If the error bit is set, we don't have an ATA device
				if ((status & ATA_SR_ERR)) {
					isATAPI = true;
					break;
				}

				// If BSY is clear and DREQ is set, we can read data
				if(!(status & ATA_SR_BSY) && (status & ATA_SR_DRQ)) break;
			}
 
			// If the drive is an ATAPI drive, send IDENTIFY PACKET command
			if(isATAPI) {
				uint8_t cl = ata_reg_read(driver, channel, ATA_REG_LBA1);
				uint8_t ch = ata_reg_read(driver, channel, ATA_REG_LBA2);
 
				if (cl == 0x14 && ch ==0xEB)
					type = ATA_DEVICE_TYPE_ATAPI;
				else if (cl == 0x69 && ch == 0x96)
					type = ATA_DEVICE_TYPE_ATAPI;
				else
					continue; // Unknown Type (may not be a device).
 
				ata_reg_write(driver, channel, ATA_REG_COMMAND, ATA_CMD_IDENTIFY_PACKET);

				// Initialise ATAPI buffer for thsi packet
				driver->devices[count].atapi_buffer[0] = 0xA8;
			}
 
			// Read Device identification block (512 bytes)
			ata_do_pio_read(driver, ide_buf, 256, channel);
 
			// Allocate a page of memory for the ATA output memory
			ata_info_t *info = (ata_info_t *) malloc(sizeof(ata_info_t));
			memset(info, 0, sizeof(ata_info_t));
			driver->devices[count].ata_info = info;

			// Save a copy of the device identification block
			memcpy(&info->ata_identify, ide_buf, 512);

			// Read device parameters
			driver->devices[count].drive_exists = true;
			driver->devices[count].type = type;
			driver->devices[count].channel = channel;
			driver->devices[count].drive = device;
			driver->devices[count].signature = *((uint16_t *) (ide_buf + ATA_IDENT_DEVICETYPE));
			driver->devices[count].capabilities = *((uint16_t *) (ide_buf + ATA_IDENT_CAPABILITIES));
			driver->devices[count].commandSets = *((uint32_t *) (ide_buf + ATA_IDENT_COMMANDSETS));
 
			// Get size of drive
			if (driver->devices[count].commandSets & (1 << 26)) { // 48-bit LBA
				driver->devices[count].size	= *((uint32_t *) (ide_buf + ATA_IDENT_MAX_LBA_EXT));
			} else { // CHS or 28 bit addressing
				driver->devices[count].size	= *((uint32_t *) (ide_buf + ATA_IDENT_MAX_LBA));
			}
 
			// Convert model string
			char *model_read = (char *) (ide_buf + ATA_IDENT_MODEL);
			for(k = 0; k < 40; k += 2) {
				driver->devices[count].model[k] = model_read[k+1];
				driver->devices[count].model[k+1] = model_read[k];
			}

			// Does the drive support DMA?
			if(*((uint16_t *) (ide_buf + ATA_IDENT_FIELDVALID))) {
				// Get supported UDMA modes
				uint16_t udma_modes = *((uint16_t *) (ide_buf + ATA_IDENT_UDMASUPPORT));
				if(udma_modes & 0x01) {
					driver->devices[count].udma_supported = kATA_UDMA0;
				} if(udma_modes & 0x02) {
					driver->devices[count].udma_supported = kATA_UDMA1;
				} if(udma_modes & 0x04) {
					driver->devices[count].udma_supported = kATA_UDMA2;
				} if(udma_modes & 0x08) {
					driver->devices[count].udma_supported = kATA_UDMA3;
				} if(udma_modes & 0x10) {
					driver->devices[count].udma_supported = kATA_UDMA4;
				} if(udma_modes & 0x20) {
					driver->devices[count].udma_supported = kATA_UDMA5;
				}

				// No UDMA support
				if (!(udma_modes & 0x2F)) {
					driver->devices[count].udma_supported = kATA_UDMANone;
				}


				// Get supported multiword DMA modes
				uint16_t mwdma_modes = *((uint16_t *) (ide_buf + ATA_IDENT_MWDMASUPPORT));
				if(mwdma_modes & 0x01) {
					driver->devices[count].mwdma_supported = kATA_MWDMA0;
				} if(mwdma_modes & 0x02) {
					driver->devices[count].mwdma_supported = kATA_MWDMA1;
				} if(mwdma_modes & 0x04) {
					driver->devices[count].mwdma_supported = kATA_MWDMA2;
				} 

				// No multiword DMA modes supported
				if (!(mwdma_modes & 0x07)) {
					driver->devices[count].mwdma_supported = kATA_MWDMANone;
				}


				// Get supported singleword DMA modes
				uint16_t swdma_modes = *((uint16_t *) (ide_buf + ATA_IDENT_SWDMASUPPORT));
				if(swdma_modes & 0x01) {
					driver->devices[count].swdma_supported = kATA_SWDMA0;
				} if(swdma_modes & 0x02) {
					driver->devices[count].swdma_supported = kATA_SWDMA1;
				} if(swdma_modes & 0x04) {
					driver->devices[count].swdma_supported = kATA_SWDMA2;
				} 

				// No singleword DMA modes supported
				if (!(swdma_modes & 0x07)) {
					driver->devices[count].swdma_supported = kATA_SWDMANone;
				}


				// Get supported PIO modes with flow control
				uint16_t pio_modes = *((uint16_t *) (ide_buf + ATA_IDENT_PIOSUPPORT));
				if(pio_modes & 0x01) {
					driver->devices[count].pio_supported = kATA_PIO3;
				} if(pio_modes & 0x02) {
					driver->devices[count].pio_supported = kATA_PIO4;
				} else { // If there's no flow control support, fall back to PIO0
					driver->devices[count].pio_supported = kATA_PIO0;					
				}

				// Read PIO transfer cycle lengths
				driver->devices[count].pio_cycle_len = *((uint16_t *) (ide_buf + ATA_IDENT_PIOCYC));
				driver->devices[count].pio_cycle_len_iordy = *((uint16_t *) (ide_buf + ATA_IDENT_PIOCYCIORDY));

				/*
				 * Multiword and singleword DMA are supported regardless of the
				 * cable used, so the best mode the drive supports is the one
				 * we prefer.
				 */
				driver->devices[count].mwdma_preferred = driver->devices[count].mwdma_supported;
				driver->devices[count].swdma_preferred = driver->devices[count].swdma_supported;
				driver->devices[count].pio_preferred = driver->devices[count].pio_supported;
			} else {
					driver->devices[count].udma_supported = kATA_UDMANone;
					driver->devices[count].mwdma_supported = kATA_MWDMANone;
			}

			count++;
		}

		// Enable IRQs for this channel
		driver->channels[channel].nIEN = 0x00;
		ata_reg_write(driver, channel, ATA_REG_CONTROL, driver->channels[channel].nIEN);
	}

	for (int i = 0; i < 4; i++) {
		if (driver->devices[i].drive_exists == true) {
			printk("ATA", "Found %s drive (%s) of %u sectors (", ((driver->devices[i].type == ATA_DEVICE_TYPE_ATA) ? "ATA" : "ATAPI"), driver->devices[i].model, driver->devices[i].size);
			printk("ATA", "Multiword DMA %u, UDMA %u, PIO %u)\n", driver->devices[i].mwdma_supported, driver->devices[i].udma_supported, driver->devices[count].pio_supported);
		}
	}

	// Clean up allocated memory
	free(ide_buf);

	ata_drivers_loaded++;
	return driver;
}

/*
 * Cleans up the ATA driver, de-registers drives and flushes caches.
 */
void ata_deinit(ata_driver_t *driver) {
	// Perform actions for each drive
	for(int i = 0; i < 4; i++) {
		if(driver->devices[i].drive_exists) {
			// Send a flush cache command

			// Release ATA info
			if(driver->devices[i].ata_info) {
				free(driver->devices[i].ata_info);
			}
		}
	}

	// Free memory
	free(driver);
	ata_drivers_loaded--;
}

// ! IO Routines
/*
 * Read from a register of a channel.
 */
static uint8_t ata_reg_read(ata_driver_t *drv, uint8_t channel, uint8_t reg) {
	uint8_t result = 0;

	// Set high LBA flag
	if (reg > 0x07 && reg < 0x0C) {
		ata_reg_write(drv, channel, ATA_REG_CONTROL, 0x80 | drv->channels[channel].nIEN);
	}
	
	// If register is 0-7, read from base address
	if (reg < 0x08) {
		result = inb(drv->channels[channel].base + reg - 0x00);
	}
	
	// Is register SECCOUNT0, or LBA 3 through 5?
	else if (reg < 0x0C) {
		result = inb(drv->channels[channel].base  + reg - 0x06);
	}
	
	// Is register DEVADDRESS?
	else if (reg < 0x0E) {
		result = inb(drv->channels[channel].ctrl  + reg - 0x0A);
	}

	else if (reg < 0x16) {
		result = inb(drv->channels[channel].bmide + reg - 0x0E);
	}
	
	// Reset high LBA flag
	if (reg > 0x07 && reg < 0x0C) {
		ata_reg_write(drv, channel, ATA_REG_CONTROL, drv->channels[channel].nIEN);
	}

	return result;
}

/*
 * Write to an ATA register
 */
static void ata_reg_write(ata_driver_t *drv, unsigned char channel, unsigned char reg, unsigned char data) {
	// High LBA flag
	if (reg > 0x07 && reg < 0x0C) {
		ata_reg_write(drv, channel, ATA_REG_CONTROL, 0x80 | drv->channels[channel].nIEN);
	}
	
	if (reg < 0x08) {
		outb(drv->channels[channel].base  + reg - 0x00, data);
	}
	
	else if (reg < 0x0C) {
		outb(drv->channels[channel].base  + reg - 0x06, data);
	}
	
	else if (reg < 0x0E) {
		outb(drv->channels[channel].ctrl  + reg - 0x0A, data);
	}
	
	else if (reg < 0x16) {
		outb(drv->channels[channel].bmide + reg - 0x0E, data);
	}
	
	// Reset high LBA flag
	if (reg > 0x07 && reg < 0x0C) {
		ata_reg_write(drv, channel, ATA_REG_CONTROL, drv->channels[channel].nIEN);
	}
}

/*
 * Performs a PIO read.
 */
static void ata_do_pio_read(ata_driver_t *drv, void* dest, uint32_t num_words, uint8_t channel) {
	ata_reg_write(drv, channel, ATA_REG_CONTROL, 0x80 | drv->channels[channel].nIEN);

	uint16_t *dest_ptr = (uint16_t *) dest;

	// Read from PIO data port
	static uint16_t temp;
	for(uint32_t i = 0; i < num_words; i++) {
		temp = inw(drv->channels[channel].base);
		*dest_ptr++ = temp;
	}

	ata_reg_write(drv, channel, ATA_REG_CONTROL, drv->channels[channel].nIEN);
}

/*
 * Polls a certain channel until the command either completes successfully, or
 * an abnormal condition happens.
 */
int ata_poll_ready(ata_driver_t *drv, uint8_t channel, bool advanced_check) {
	// Wait for drive to assert BSY
	for(int i = 0; i < 4; i++) {
		ata_reg_read(drv, channel, ATA_REG_ALTSTATUS);
	}

	unsigned int waitCycles = 0;

	// Wait for the device to no longer be busy
	while(ata_reg_read(drv, channel, ATA_REG_STATUS) & ATA_SR_BSY) {
		waitCycles++;

		// Allow requests to time out
		if(waitCycles > ATA_WAIT_TIMEOUT) return ATA_ERR_TIMEOUT;
	}

 	// If set, we perform more in-depth checking
	if(advanced_check) {
		// Read status register
		uint8_t state = ata_reg_read(drv, channel, ATA_REG_STATUS);

		if(state & ATA_SR_ERR) { // Error executing command
			return 2;
		} else if(state & ATA_SR_DF) { // Device fault
			return 1;
		} else if(!(state & ATA_SR_DRQ)) { // BSY = 0; DF = 0; ERR = 0; DRQ = 0
			printk("ATA", "IDE: No data after %u cycles\n", waitCycles);
			return 3;
		}
	}

	// Hooray everything is happy
	return 0;
}

// !Interrupt support
/*
 * Called from the interrupt handler of an IDE driver to 
 */
void ata_irq_callback(registers_t *regs) {

}

// ! Miscellaneous
/*
 * Called when all drivers are loaded to attempt legacy ATA probing
 */
static int ata_post_driver_load(void) {
	if(ata_drivers_loaded == 0) {
		printk("ATA", "No ATA drivers loaded, trying legacy detection\n");

		// Set up ATA driver (this will probe for devices)
		ata = ata_init_pci(0, 0, 0, 0, 0);

		// List devices found
		for (int i = 0; i < 4; i++) {
			if (ata->devices[i].drive_exists == true) {

			}
		}

		// Register IRQs 14 and 15 since parallel ATA
		register_interrupt_handler(14, ata_irq_callback);
		register_interrupt_handler(15, ata_irq_callback);
	}

	return 0;
}
//~ module_post_driver_init(ata_post_driver_load);

/*
 * Handles error codes returned by ata_poll_ready.
 */
static int ata_convert_error(ata_driver_t *drv, int drive, int err) {
	if(err == 1) { // device fault
		err = ATA_ERR_DEVICE_FAULT;
	} else if(err == 2) {
		// Read error register
		uint8_t st = ata_reg_read(drv, drv->devices[drive].channel, ATA_REG_ERROR);

		if(st & ATA_ER_AMNF) { // no address mark
			err = ATA_ERR_NO_ADDR_MARK;
		} if(st & ATA_ER_TK0NF) { // no media or media error
			err = ATA_ERR_MEDIA_ERROR;
		} if(st & ATA_ER_ABRT) { // command aborted
			err = ATA_ERR_CMD_ABORT;
		} if(st & ATA_ER_MCR) { // no media or media error
			err = ATA_ERR_MEDIA_CHANGE_REQUEST;
		} if(st & ATA_ER_IDNF) { // ID mark not found
			err = ATA_ERR_ID_NOT_FOUND;
		} if(st & ATA_ER_MC) { // no media or media error
			err = ATA_ERR_MEDIA_CHANGE;
		} if(st & ATA_ER_UNC) { // uncorrectable data error
			err = ATA_ERR_UNCORRECTABLE_ERROR;
		} if(st & ATA_ER_BBK) { // bad sectors
			err = ATA_ERR_BAD_SECTOR;
		}
	} else if(err == 3) { // no data returned by device
		err = ATA_ERR_NO_DATA;
	} else if (err == 4) { // write protected
		err = ATA_ERR_WRITE_PROTECT;
	} else {
		return err;
	}
 
	return err;
}

/*
 * Reads a certain number of sectors from the specified drive using either DMA
 * or PIO, into the user-supplied buffer.
 */
int ata_read(ata_driver_t *drv, uint8_t drive, uint32_t lba, uint8_t sectors, void *buffer) {
	if(buffer) {
		return ata_access_pio(drv, ATA_READ, drive, lba, sectors, buffer);
	} else {
		return -1;
	}
}

/*
 * Writes a certain number of sectors from the specified buffer to the LBA
 * specified on the selected drive.
 */
int ata_write(ata_driver_t *drv, uint8_t drive, uint32_t lba, uint8_t sectors, void *buffer) {
	if(buffer) {
		return ata_access_pio(drv, ATA_WRITE, drive, lba, sectors, buffer);
	} else {
		return -1;
	}	
}

/*
 * Performs a read or write from a regular ATA drive.
 *
 * Note that this does NOT work on ATAPI drives.
 */
static int ata_access_pio(ata_driver_t *drv, uint8_t rw, uint8_t drive, uint32_t lba, uint8_t numsects, void *buf) {
	uint8_t lba_mode; // 0: CHS, 1:LBA28, 2: LBA48
	uint8_t cmd;

	uint8_t lba_io[6];
	uint8_t channel = drv->devices[drive].channel; // channel
	uint8_t slavebit = drv->devices[drive].drive & 0x01; // master/slave
	uint16_t bus_io_reg = drv->channels[channel].base; // Bus base address

	// Size of a single sector in words (assume 512)
	uint16_t sectorSize = 256;

	uint16_t cyl;
	uint8_t head, sect, err;

	// Ensure the LBA is within range of this device's size
	if(lba > drv->devices[drive].size) {
		return ATA_ERR_INVALID;
	}

	// Ensure we don't try to read past the end of the drive
	if(lba + numsects > drv->devices[drive].size) {
		return ATA_ERR_INVALID;
	}

	// Select the appropriate addressing mode
	if(lba >= 0x10000000) { // LBA48
		lba_mode  = 2;
		lba_io[0] = (lba & 0x000000FF) >> 0;
		lba_io[1] = (lba & 0x0000FF00) >> 8;
		lba_io[2] = (lba & 0x00FF0000) >> 16;
		lba_io[3] = (lba & 0xFF000000) >> 24;
		lba_io[4] = 0;
		lba_io[5] = 0;
		head = 0;
	} else if(drv->devices[drive].capabilities & 0x200)  { // LBA28
		lba_mode  = 1;
		lba_io[0] = (lba & 0x00000FF) >> 0;
		lba_io[1] = (lba & 0x000FF00) >> 8;
		lba_io[2] = (lba & 0x0FF0000) >> 16;
		lba_io[3] = 0;
		lba_io[4] = 0;
		lba_io[5] = 0;
		head = (lba & 0xF000000) >> 24;
	} else { // CHS
		lba_mode = 0;
		sect = (lba % 63) + 1;
		cyl = (lba + 1 - sect) / (16 * 63);
		lba_io[0] = sect;
		lba_io[1] = (cyl >> 0) & 0xFF;
		lba_io[2] = (cyl >> 8) & 0xFF;
		lba_io[3] = 0;
		lba_io[4] = 0;
		lba_io[5] = 0;
		head = (lba + 1 - sect) % (16 * 63) / (63); // Head number is written to HDDEVSEL lower 4-bits.
	}

	// Disable IRQs
	// ata_reg_write(drv, channel, ATA_REG_CONTROL, drv->channels[channel].nIEN = (0x0) + 0x02);

	// Wait until the channel is no longer busy
	while(ata_reg_read(drv, channel, ATA_REG_STATUS) & ATA_SR_BSY);

	// Select drive and LBA/CHS mode
	if (lba_mode == 0) { // CHS
		ata_reg_write(drv, channel, ATA_REG_HDDEVSEL, 0xA0 | (slavebit << 4) | head);
	} else {
		ata_reg_write(drv, channel, ATA_REG_HDDEVSEL, 0xE0 | (slavebit << 4) | head);
	}

	// Write parameters
	if (lba_mode == 2) { // Write LBA48 special regs
		ata_reg_write(drv, channel, ATA_REG_SECCOUNT1, 0);
		ata_reg_write(drv, channel, ATA_REG_LBA3, lba_io[3]);
		ata_reg_write(drv, channel, ATA_REG_LBA4, lba_io[4]);
		ata_reg_write(drv, channel, ATA_REG_LBA5, lba_io[5]);
	}

	ata_reg_write(drv, channel, ATA_REG_SECCOUNT0, numsects);
	ata_reg_write(drv, channel, ATA_REG_LBA0, lba_io[0]);
	ata_reg_write(drv, channel, ATA_REG_LBA1, lba_io[1]);
	ata_reg_write(drv, channel, ATA_REG_LBA2, lba_io[2]);

	// Select correct command
	if (lba_mode == 0 && rw == ATA_READ) cmd = ATA_CMD_READ_PIO;
	if (lba_mode == 1 && rw == ATA_READ) cmd = ATA_CMD_READ_PIO;
	if (lba_mode == 2 && rw == ATA_READ) cmd = ATA_CMD_READ_PIO_EXT;
	if (lba_mode == 0 && rw == ATA_WRITE) cmd = ATA_CMD_WRITE_PIO;
	if (lba_mode == 1 && rw == ATA_WRITE) cmd = ATA_CMD_WRITE_PIO;
	if (lba_mode == 2 && rw == ATA_WRITE) cmd = ATA_CMD_WRITE_PIO_EXT;

	// Send command
	ata_reg_write(drv, channel, ATA_REG_COMMAND, cmd);

	/*
	 * To read and receive blocks, we use "rep insw"/"rep outsw" instructions.
	 * They expect the number of words in ECX, IO port in EDX, and address to
	 * use in ES:EDI.
	 */
#define SYS_KERN_DATA_SEG 0x10
	// Read or write data to the device
	if (rw == ATA_READ) { // Read
		for (uint8_t s = 0; s < numsects; s++) {
			// Wait for drive to have a sector available
			if ((err = ata_poll_ready(drv, channel, true))) {
				printk("ATA", "IDE: Device read error (disk %u on channel %u)\n", slavebit, channel);
				return ata_convert_error(drv, drive, err);
			}

			// Receive data
			__asm__ volatile("pushw %es");
			__asm__ volatile("mov %%ax, %%es" : : "a" (SYS_KERN_DATA_SEG));
			__asm__ volatile("rep insw" : : "c" (sectorSize), "d" (bus_io_reg), "D" (buf));
			__asm__ volatile("popw %es");

			buf += (sectorSize * 2);
		}
	} else { // Write
		for (uint8_t s = 0; s < numsects; s++) {
			// Wait for the device to be ready to accept another sector
			ata_poll_ready(drv, channel, false);

			// Write data
			__asm__ volatile("pushw %ds");
			__asm__ volatile("mov %%ax, %%ds" : : "a" (SYS_KERN_DATA_SEG));
			__asm__ volatile("rep outsw" : : "c" (sectorSize), "d" (bus_io_reg), "S" (buf));
			__asm__ volatile("popw %ds");

			buf += (sectorSize * 2);
		}

		// Flush cache after writing
		uint8_t cacheCmd[3] = {ATA_CMD_CACHE_FLUSH, ATA_CMD_CACHE_FLUSH, ATA_CMD_CACHE_FLUSH_EXT};
		ata_reg_write(drv, channel, ATA_REG_COMMAND, cacheCmd[lba_mode]);

		// Wait for the device to be ready again
		ata_poll_ready(drv, channel, false);
	}

	return ATA_ERR_NONE;
}


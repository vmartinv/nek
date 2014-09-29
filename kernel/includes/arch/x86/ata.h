#ifndef ATA_H
#define ATA_H

#include <types.h>

#define ATA_PRIMARY						0x00
#define ATA_SECONDARY					0x01

#define ATA_DEVICE_TYPE_ATA				0x00
#define ATA_DEVICE_TYPE_ATAPI			0x01

#define ATA_MASTER						0x00
#define ATA_SLAVE						0x01

#define	ATA_READ						0x00
#define ATA_WRITE						0x01

#define ATA_ERR_NONE					0x00 // great success, many good, very ATA
#define ATA_ERR_MISC					0x01 // unexpected stuff
#define ATA_ERR_DMA_CRC					0x02 // interface CRC error during DMA
#define	ATA_ERR_MEDIA_ERROR				0x03 // error reading/writing media
#define	ATA_ERR_MEDIA_CHANGE_REQUEST	0x04 // media change reuqest
#define	ATA_ERR_MEDIA_CHANGE			0x05 // media was changed since last command
#define	ATA_ERR_ID_NOT_FOUND			0x06 // address could not be found
#define	ATA_ERR_NO_ADDR_MARK			0x07 // invalid address or broken media
#define	ATA_ERR_UNCORRECTABLE_ERROR		0x08 // uncorrectable media error
#define	ATA_ERR_BAD_SECTOR				0x09 // bad sectors
#define	ATA_ERR_DEVICE_FAULT			0x0A // device fault
#define	ATA_ERR_CMD_ABORT				0x0B // command aborted
#define	ATA_ERR_NO_DATA					0x0C // no data returned by device
#define	ATA_ERR_WRITE_PROTECT			0x0D // media write-protected
#define ATA_ERR_TIMEOUT					0x0E // timeout while waiting for device
#define ATA_ERR_NO_MEDIA				0x0F // no media present in removable media device
#define ATA_ERR_INVALID					0xFF // invalid inputs to function

/*
 * DMA modes
 */
typedef enum {
	kATA_UDMANone = -1,
	kATA_UDMA0 = 0,
	kATA_UDMA1,
	kATA_UDMA2,
	kATA_UDMA3,
	kATA_UDMA4,
	kATA_UDMA5
} ata_udma_mode_t;

typedef enum {
	kATA_MWDMANone = -1,
	kATA_MWDMA0 = 0,
	kATA_MWDMA1,
	kATA_MWDMA2
} ata_mwdma_mode_t;

typedef enum {
	kATA_SWDMANone = -1,
	kATA_SWDMA0 = 0,
	kATA_SWDMA1,
	kATA_SWDMA2
} ata_swdma_mode_t;

typedef enum {
	kATA_PIO0 = 0,
	kATA_PIO1,
	kATA_PIO2,

	// PIO 3 and 4 use IORDY flow control
	kATA_PIO3,
	kATA_PIO4
} ata_pio_mode_t;

typedef struct ata_info ata_info_t;
typedef struct ata_driver ata_driver_t;
typedef struct ata_device ata_device_t;
typedef struct ata_channel ata_channel_t;

// Struct defining an ATA device
struct ata_device {
	bool drive_exists; // Set to true if a device actually exists

	uint8_t channel; // 0 (Primary Channel) or 1 (Secondary Channel)
	uint8_t drive; // 0: master or 1: slave
	uint16_t type; // 0: ATA, 1:ATAPI

	uint16_t signature;	// drive signature
	uint16_t capabilities; // features
	uint32_t commandSets; // supported comamnd sets

	uint32_t size; // size in sectors
	
	char model[42]; // model name string

	ata_udma_mode_t udma_supported; // highest supported UDMA mode
	ata_mwdma_mode_t mwdma_supported; // highest supported multiword DMA mode
	ata_mwdma_mode_t swdma_supported; // highest supported singleword DMA mode

	ata_udma_mode_t udma_preferred; // Preferred UDMA transfer mode
	ata_mwdma_mode_t mwdma_preferred; // Preferred multiword DMA mode (used only if UDMA is none)
	ata_mwdma_mode_t swdma_preferred; // Preferred singleword DMA mode (used only if MWDMA is none)
	
	ata_pio_mode_t pio_supported; // highest supported PIO mode
	ata_pio_mode_t pio_preferred; // Preferred PIO mode (used until DMA is enabled)
	uint16_t pio_cycle_len; // minimum PIO cycle length without flow control
	uint16_t pio_cycle_len_iordy; // minimum PIO cycle length with flow control

	ata_info_t *ata_info; // Info read from the device (IDENTIFY output)

	bool dma_enabled; // set if the ATA driver associated with this drive will handle DMA

	uint8_t atapi_buffer[12]; // Buffer for ATAPI stuff
};

// Struct defining an ATA channel
struct ata_channel {
	uint16_t base;  // I/O Base.
	uint16_t ctrl;  // Control Base
	uint16_t bmide; // Bus Master IDE
	bool nIEN;  // nIEN (No Interrupt);
};

// Struct to hold ATA info read from the device
struct ata_info {
	uint8_t ata_identify[512];
};

// Struct to interact with the driver
struct ata_driver {
	uint32_t BAR0, BAR1, BAR2, BAR3, BAR4, BAR5;

	// Channels
	ata_channel_t channels[2];

	// Devices
	ata_device_t devices[4];
};

ata_driver_t* ata_init_pci(uint32_t BAR0, uint32_t BAR1, uint32_t BAR2, uint32_t BAR3, uint32_t BAR4);
void ata_deinit(ata_driver_t *driver);

int ata_read(ata_driver_t *drv, uint8_t drive, uint32_t lba, uint8_t sectors, void *buffer);
int ata_write(ata_driver_t *drv, uint8_t drive, uint32_t lba, uint8_t sectors, void *buffer);



#endif

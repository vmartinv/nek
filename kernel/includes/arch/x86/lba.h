#ifndef LBA_H
#define LBA_H

#include <types.h>

#define LBA_PRIMARY_MASTER   0
#define LBA_PRIMARY_SLAVE    1
#define LBA_SECONDARY_MASTER 2
#define LBA_SECONDARY_SLAVE  3

#define LBA_MAGIC_VALUE 0x88

#define LBA_DATA_PORT         0x1f0
#define LBA_FEATURES_PORT     0x1f1
#define LBA_SECTOR_COUNT_PORT 0x1f2
#define LBA_0_BLOCK_ADDR_PORT 0x1f3
#define LBA_1_BLOCK_ADDR_PORT 0x1f4
#define LBA_2_BLOCK_ADDR_PORT 0x1f5
#define LBA_3_BLOCK_ADDR_PORT 0x1f6
#define LBA_COMMAND_PORT      0x1f7
#define LBA_S_OFFSET          0xe0

#define LBA_DATA_P_PORT         0x1f0
#define LBA_NULL_P_PORT         0x1f1
#define LBA_SECTOR_COUNT_P_PORT 0x1f2
#define LBA_0_BLOCK_ADDR_P_PORT 0x1f3
#define LBA_1_BLOCK_ADDR_P_PORT 0x1f4
#define LBA_2_BLOCK_ADDR_P_PORT 0x1f5
#define LBA_3_BLOCK_ADDR_P_PORT 0x1f6
#define LBA_COMMAND_P_PORT      0x1f7

#define LBA_CONTROL_REGISTER_P_PORT 0x3f6

#define LBA_NULL_S_PORT         0x111
#define LBA_SECTOR_COUNT_S_PORT 0x112
#define LBA_0_BLOCK_ADDR_S_PORT 0x113
#define LBA_1_BLOCK_ADDR_S_PORT 0x114
#define LBA_2_BLOCK_ADDR_S_PORT 0x115
#define LBA_3_BLOCK_ADDR_S_PORT 0x116
#define LBA_COMMAND_S_PORT      0x117

#define LBA_READ_COMMAND          0x20
#define LBA_WRITE_COMMAND         0x30
#define LBA_READ_COMMAND_EXT      0x24
#define LBA_WRITE_COMMAND_EXT     0x34
#define LBA_READ_COMMAND_DMA      0x25
#define LBA_WRITE_COMMAND_DMA     0x35
#define LBA_SELECT_MASTER_COMMAND 0xa0
#define LBA_SELECT_SLAVE_COMMAND  0xb0
#define LBA_IDENTIFY_COMMAND      0xec
#define LBA_CACHE_FLUSH_COMMAND   0xe7

#define LBA_STATUS_ERROR       (1<<0)
#define LBA_STATUS_DATA_READY  (1<<3)
#define LBA_STATUS_MODE_SRV    (1<<4)
#define LBA_STATUS_DRIVE_FAULT (1<<5)
#define LBA_STATUS_DRIVE_READY (1<<6)
#define LBA_STATUS_DRIVE_BUSY  (1<<7)

typedef struct lba_drive {
  enum {MASTER, SLAVE} type;
  enum {PRIMARY, SECONDARY} bus;
  bool supports_lba48;
  uint64_t addressable_sectors;
  unsigned char udma_modes;
  unsigned short udma_mode;
} lba_drive_t;

int init_drives();
int lba_read_sectors(lba_drive_t* drive, unsigned long address, unsigned short count, unsigned char *buff);
void lba_write_sectors(lba_drive_t* drive, unsigned long address, unsigned short count, unsigned char *buff);
lba_drive_t* get_drive(unsigned short dn);
#endif

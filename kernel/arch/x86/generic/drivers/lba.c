#include <stdio.h>
#include <sys/timer.h>
#include <stdlib.h>
#include <arch/x86/lba.h>
#include <arch/x86/descriptor_tables.h>
#include <arch/x86/regs.h>
#include <arch/x86/ports.h>

#define IO_DELAY()        do{inb(0x80); inb(0x80); inb(0x80); inb(0x80);}while(0)

lba_drive_t *drives[4];

static void primary_disk_handler(registers_t *r) {
  printf("primary disk handler: %b\n", inb(LBA_COMMAND_P_PORT));
  outb(PIC1_COMMAND, PIC_EOI);
  outb(PIC2_COMMAND, PIC_EOI);
}

static void secondary_disk_handler(registers_t *r) {
  printf("secondary disk handler called\n");
}

static inline void wait_for_data(int offset) {
  while(!(inb(LBA_COMMAND_PORT + offset) & LBA_STATUS_DRIVE_READY));
  //check LBAhi and LBAmid
  while(!(inb(LBA_COMMAND_PORT + offset) & LBA_STATUS_DATA_READY));

}

static void send_rw_command(int command, unsigned short count, lba_drive_t* drive, uint64_t address){

  unsigned short offset = drive->bus == PRIMARY ? 0 : LBA_S_OFFSET,
    slavebit = drive->type == MASTER ? 0 : 1,
    thing = drive->type == MASTER ? 0x40 : 0x50; //How's this called?

  //  outb(LBA_3_BLOCK_ADDR_P_PORT, LBA_SELECT_MASTER_COMMAND);
  outb(LBA_FEATURES_PORT + offset, 0x00);
  outb(LBA_3_BLOCK_ADDR_PORT + offset, thing | (slavebit << 4));
  IO_DELAY();

  outb(LBA_SECTOR_COUNT_PORT + offset, count >> 8);
  outb(LBA_0_BLOCK_ADDR_PORT + offset, (unsigned char) (address >> 24) & 0xff);
  outb(LBA_1_BLOCK_ADDR_PORT + offset, (unsigned char) (address >> 32) & 0xff);
  outb(LBA_2_BLOCK_ADDR_PORT + offset, (unsigned char) (address >> 40) & 0xff);
  outb(LBA_SECTOR_COUNT_PORT + offset, count & 0xff);
  outb(LBA_0_BLOCK_ADDR_PORT + offset, (unsigned char) address & 0xff);
  outb(LBA_1_BLOCK_ADDR_PORT + offset, (unsigned char) (address >> 8) & 0xff);
  outb(LBA_2_BLOCK_ADDR_PORT + offset, (unsigned char) (address >> 16) & 0xff);

  outb(LBA_COMMAND_PORT + offset, command);
  IO_DELAY();
}

int lba_read_sectors(lba_drive_t* drive, unsigned long address, unsigned short count, unsigned char *buff) {
  send_rw_command(LBA_READ_COMMAND_EXT, count, drive, address);
  unsigned short offset = drive->bus == PRIMARY ? 0 : LBA_S_OFFSET;
  int i;
  unsigned char response;
  do{
    response = inb(LBA_COMMAND_PORT + offset);
  }while((response & LBA_STATUS_DRIVE_BUSY) && !(response & 1) );
  for (i = 0; i < count; i++) {
    while(!(inb(LBA_COMMAND_PORT + offset) & LBA_STATUS_DRIVE_READY));
    //wait_for_data(offset);
    do {
      response = inb(LBA_COMMAND_PORT + offset);
      if ((response & LBA_STATUS_ERROR) || (response & LBA_STATUS_DRIVE_FAULT)) {
        printf("disk error reading 0x%x: %b\n", address, response);
        return 0;
      }
    } while(!(response & LBA_STATUS_DATA_READY));

    insw(LBA_DATA_PORT+offset, (char*)(buff+i*512), 256);
  }
  return 1;
}

void lba_write_sectors(lba_drive_t* drive, unsigned long address, unsigned short count, unsigned char *buff) {
  send_rw_command(LBA_WRITE_COMMAND_EXT, count, drive, address);
  unsigned short offset = drive->bus == PRIMARY ? 0 : LBA_S_OFFSET;
  int i;
  unsigned char response;
  for (i = 0; i < count; i++) {
    do {
      response = inb(LBA_COMMAND_PORT + offset);
      if ((response & LBA_STATUS_ERROR) || (response & LBA_STATUS_DRIVE_FAULT)) {
        return;
      }

    } while(!(response & LBA_STATUS_DRIVE_READY));


    int idx;
    unsigned short temp;
    for (idx = 0; idx < 256; idx++) {
      temp = ((unsigned short)buff[i*256 + idx*2] << 8) + buff[i*256 + idx*2 + 1];
      outw(LBA_DATA_PORT + offset, temp);
      printf("");
    }
    outb(LBA_COMMAND_PORT + offset, LBA_CACHE_FLUSH_COMMAND);
    //    sleep(0.1);
    while(!(inb(LBA_COMMAND_PORT + offset) & LBA_STATUS_DATA_READY));

  }
}

static void show_drive_info(lba_drive_t* d) {
  printf("%s %s using %s, %d available sectors (%d mbytes)\n",
         d->bus == PRIMARY ? "Primary" : "Secondary",
         d->type == MASTER ? "master" : "slave",
         d->supports_lba48 ? "LBA48" : "LBA28",
         (int)d->addressable_sectors,
          (int)((d->addressable_sectors / 1024 / 1024) *512 ) ); //TODO: improve printf accepts long longs
}

static bool identify(unsigned short drive_t) {
  unsigned short offset=0;
  switch (drive_t) {
  case LBA_PRIMARY_MASTER:
    outb(LBA_3_BLOCK_ADDR_P_PORT, LBA_SELECT_MASTER_COMMAND);
    offset = 0;
    break;
  case LBA_PRIMARY_SLAVE:
    outb(LBA_3_BLOCK_ADDR_P_PORT, LBA_SELECT_SLAVE_COMMAND);
    offset = 0;
    break;
  case LBA_SECONDARY_MASTER:
    outb(LBA_3_BLOCK_ADDR_S_PORT, LBA_SELECT_MASTER_COMMAND);
    offset = LBA_S_OFFSET;
    break;
  case LBA_SECONDARY_SLAVE:
    outb(LBA_3_BLOCK_ADDR_S_PORT, LBA_SELECT_SLAVE_COMMAND);
    offset = LBA_S_OFFSET;
    break;
  }
  //~ settextcolor(GREEN, BLACK);
  printf("IDENTIFY\n");

  outb(LBA_SECTOR_COUNT_PORT + offset, 0x00);
  outb(LBA_0_BLOCK_ADDR_PORT + offset, 0x00);
  outb(LBA_1_BLOCK_ADDR_PORT + offset, 0x00);
  outb(LBA_2_BLOCK_ADDR_PORT + offset, 0x00);
  outb(LBA_COMMAND_PORT + offset, LBA_IDENTIFY_COMMAND);
  IO_DELAY();

  unsigned int result;
  result = inb(LBA_COMMAND_PORT + offset);
  if (result) {
    wait_for_data(offset);
    uint16_t buff[256];
    int i;
    for(i=0;i<256;i++)
      buff[i] = inw(LBA_DATA_P_PORT);

    lba_drive_t* drive = (lba_drive_t*)malloc(sizeof(lba_drive_t*));
    drive->type = drive_t == LBA_PRIMARY_MASTER || drive_t == LBA_SECONDARY_MASTER ? MASTER : SLAVE;
    drive->bus = drive_t == LBA_PRIMARY_MASTER || drive_t == LBA_PRIMARY_SLAVE ? PRIMARY : SECONDARY;

    drive->supports_lba48 = (buff[83] & (1<<10)) != 0;
    if (drive->supports_lba48)
		if(buff[101] || buff[103]) drive->addressable_sectors=-1;//cant fit... choose MAX
		else  drive->addressable_sectors = buff[100] + ((uint64_t)buff[101] << 32);
       //~ + (buff[102] << 64)
       //~ + (buff[103] << 96)
    else
      drive->addressable_sectors = buff[60] + ((uint64_t)buff[61] << 32);
    drives[drive_t] = drive;
    return true;
  } else {
    return false;
  }
}

int init_drives() {
  unsigned char response;
  int i;
  for (i=0; i < 4; i++) drives[i] = NULL;

  //~ register_interrupt_handler(14, primary_disk_handler); 
  //~ register_interrupt_handler(15, secondary_disk_handler);
  outb(LBA_COMMAND_P_PORT, (1<<1));
  outb(LBA_COMMAND_S_PORT, (1<<1));

  outb(LBA_0_BLOCK_ADDR_P_PORT, LBA_MAGIC_VALUE);
  response = inb(LBA_0_BLOCK_ADDR_P_PORT);
  if (response == LBA_MAGIC_VALUE) {
    outb(LBA_3_BLOCK_ADDR_P_PORT, LBA_SELECT_MASTER_COMMAND);
    sleep(100);
    response = inb(LBA_COMMAND_P_PORT);
    if (response & LBA_STATUS_DRIVE_READY) {
      identify(LBA_PRIMARY_MASTER);
    }
    else{
		return 1;
	}
  } else {
    return 2;
  }

  outb(LBA_0_BLOCK_ADDR_S_PORT, LBA_MAGIC_VALUE);
  response = inb(LBA_0_BLOCK_ADDR_S_PORT);
  if (response == LBA_MAGIC_VALUE) {
    outb(LBA_3_BLOCK_ADDR_P_PORT, LBA_SELECT_SLAVE_COMMAND);
    sleep(100);
    response = inb(LBA_COMMAND_S_PORT);
    if (response & LBA_STATUS_DRIVE_READY) {
      identify(LBA_PRIMARY_SLAVE);
    }
  } else {
    // Secondary controller not present
  }

  for (i=0; i < 4; i++) {
    if (drives[i])
      show_drive_info(drives[i]);
  }
  return 0;
}

lba_drive_t* get_drive(unsigned short drive_n) {
  return drives[drive_n];
}

#ifndef MBR_H
#define MBR_H

#include <types.h>
#include <fs/ptable.h>
#include <arch/x86/ata.h>
#include <fs/disk.h>

ptable_t* mbr_load(disk_t *disk);

#endif

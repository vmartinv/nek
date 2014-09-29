#include <types.h>
#include <arch/x86/ata.h>
#include <string.h>
#include <stdlib.h>

#include <fs/disk.h>

/*
 * Allocates memory for a disk.
 */
disk_t* disk_allocate() {
	disk_t *disk = (disk_t *) malloc(sizeof(disk_t));
	memset(disk, 0, sizeof(disk_t));
	return disk;
}

/*
 * Various wrappers that call the driver's functions for the disk.
 */
DISK_ERROR disk_reset(disk_t* disk) {
	return disk->reset(disk);
}

DISK_ERROR disk_init(disk_t* disk) {
	return disk->init(disk);
}

DISK_ERROR disk_read(disk_t* disk, uint64_t lba, uint32_t num_sectors, void* destination) {
	return disk->read(disk, lba, num_sectors, destination);
}

DISK_ERROR disk_write(disk_t* disk, uint64_t lba, uint32_t num_sectors, void* source) {
	return disk->write(disk, lba, num_sectors, source);
}

DISK_ERROR disk_flush_cache(disk_t* disk) {
	return disk->flush_cache(disk);
}

DISK_ERROR disk_erase(disk_t* disk, uint64_t lba, uint32_t num_sectors) {
	return disk->erase(disk, lba, num_sectors);
}

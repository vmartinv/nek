#ifndef DISK_H
#define DISK_H

#include <types.h>

typedef enum {
	kDiskErrorNone = 0,
	kDiskErrorIO,
	kDiskErrorController,
	kDiskErrorATA,
	kDiskErrorUnknown
} DISK_ERROR;


typedef struct disk {
	// Pointer to driver's data structure for the drive
	void* driver_specific_data;

	DISK_ERROR (*reset)(struct disk*);
	DISK_ERROR (*init)(struct disk*);
	DISK_ERROR (*read)(struct disk*, uint64_t, uint32_t, void*);
	DISK_ERROR (*write)(struct disk*, uint64_t, uint32_t, void*);
	DISK_ERROR (*flush_cache)(struct disk*);
	DISK_ERROR (*erase)(struct disk*, uint64_t, uint32_t);

	// Size of the drive (sectors) or 0 if unknown
	uint64_t size;
} disk_t;


disk_t* disk_allocate();

DISK_ERROR disk_reset(disk_t* disk);
DISK_ERROR disk_init(disk_t* disk);

DISK_ERROR disk_read(disk_t* disk, uint64_t lba, uint32_t num_sectors, void* destination);

DISK_ERROR disk_write(disk_t* disk, uint64_t lba, uint32_t num_sectors, void* source);
DISK_ERROR disk_flush_cache(disk_t* disk);

DISK_ERROR disk_erase(disk_t* disk, uint64_t lba, uint32_t num_sectors);

/*DISK_ERROR disk_eject(disk_t* disk, bool eject);
DISK_ERROR disk_idle(disk_t* disk, uint8_t timeout);
DISK_ERROR disk_standby(disk_t* disk);
DISK_ERROR disk_wake(disk_t* disk);

DISK_ERROR disk_get_status(disk_t* disk, int* destination);
DISK_ERROR disk_get_health(disk_t* disk, void* healthBuffer);*/

#endif

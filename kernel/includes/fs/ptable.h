/*
 * This header defines types to work with partition tables and constants for
 * use in identifying filesystems.
 */

#ifndef PTABLE_H
#define PTABLE_H

#include <types.h>
#include <fs/disk.h>

#define PARTITION_LBA_REL2ABS(lba, pt) (((lba)+(pt))->lba_start)

// Entry in a partition table (i.e. a partition)
typedef struct ptable_entry {
 	uint32_t lba_start;
 	uint32_t lba_length;

 	// Identifier of filesystem that a kernel extension registered for
 	uint16_t type;

 	// Logical partition number
 	uint8_t part_num;

	struct ptable_entry *next;

	// Partition table pointer
	void* ptable;
 } ptable_entry_t;

// Kinds of partition tables
typedef enum {
	kPartitionTableUnknown,
	kPartitionTableMBR
} ptable_type_t;

// Defines a partition table
typedef struct {
	disk_t *disk;
	ptable_type_t type;
	ptable_entry_t *first;
} ptable_t;

#endif

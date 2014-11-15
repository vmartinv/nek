/*
 * This header defines types to work with partition tables and constants for
 * use in identifying filesystems.
 */

#ifndef PTABLE_H
#define PTABLE_H

#include <types.h>
#include <lib/list.h>

#define PARTITION_LBA_REL2ABS(lba, pt) (((lba)+(pt))->lba_start)

// Entry in a partition table (i.e. a partition)
typedef struct ptable_entry {
 	uint32_t lba_start;
 	uint32_t lba_length;

 	// Identifier of filesystem that a kernel extension registered for
 	uint16_t type;
 	
 	bool boot;
 	uint16_t start_head, start_sector, start_cylinder;

 	// Logical partition number
 	uint8_t part_num;

	struct ptable_entry *next;

	// Partition table pointer
	void* ptable;
 } ptable_entry_t;


// Defines a partition table
typedef struct {
	list_t *parts;
} ptable_t;
const ptable_t* mbr_get_partition_table();
const ptable_entry_t * mbr_get_partition(const ptable_t*table);
void mbr_verify();
#endif

#include <types.h>
#include <fs/mbr.h>
#include <string.h>
#include <stdlib.h>

ptable_t* mbr_load(disk_t *disk) {
	// Read MBR
	uint8_t *outBuff = (uint8_t *) malloc(0x200);
	int ret = disk_read(disk, 0, 1, outBuff);

	if(ret != kDiskErrorNone) {
		return NULL;
	}

	// Check if MBR is valid
	if(outBuff[0x1FE] != 0x55 || outBuff[0x1FF] != 0xAA) {
		return NULL;
	}

	// Allocate memory for patition table struct
	ptable_t *table = (ptable_t *) malloc(sizeof(ptable_t));
	table->type = kPartitionTableMBR;
	//table->disk = disk;

	// Allocate memory for first partition table entry
	static ptable_entry_t* partition;

	// Parse MBR to get partitions on the drive
	for(int i = 0; i < 4; i++) {
		int offset = (i * 16) + 0x1BE;

		// If partition type isn't zero, allocate a new struct
		if(outBuff[offset+4] != 0 && partition) { // second or later entry
			ptable_entry_t* newEntry = (ptable_entry_t *) malloc(sizeof(ptable_entry_t));
			free(newEntry);

			partition->next = newEntry;
			partition = newEntry;

		} else if(outBuff[offset+4] != 0 && !partition) { // it's the first entry
			partition = (ptable_entry_t *) malloc(sizeof(ptable_entry_t));
			table->first = partition;
		} else {
			continue;
		}

		// Read in some info from the MBR
		partition->type = outBuff[offset+4];
		partition->lba_start = outBuff[offset+8] | (outBuff[offset+9]<<8) | (outBuff[offset+10]<<16) | (outBuff[offset+11]<<24);
		partition->lba_length = outBuff[offset+12] | (outBuff[offset+13]<<8) | (outBuff[offset+14]<<16) | (outBuff[offset+15]<<24);

		partition->part_num = (i & 0x03);

		partition->ptable = table;

	}

	return table;
}

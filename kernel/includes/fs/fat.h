#ifndef FAT_H
#define FAT_H

#include <types.h>
#include <fs/vfs.h>

#define FAT_BUF_SECTORS 8
#define FAT_SECTOR_BUFFER_SIZE 512*FAT_BUF_SECTORS

#define FAT32_MASK 0x0FFFFFFF
#define FAT32_BAD_CLUSTER 0x0FFFFFF7
#define FAT32_END_CHAIN 0x0FFFFFF0
#define FAT16_BAD_CLUSTER 0xFFF7
#define FAT16_END_CHAIN 0xFFF8

#define FAT_ATTR_READ_ONLY 0x01
#define FAT_ATTR_HIDDEN 0x02
#define FAT_ATTR_SYSTEM 0x04
#define FAT_ATTR_VOLUME_ID 0x08
#define FAT_ATTR_DIRECTORY 0x10
#define FAT_ATTR_ARCHIVE 0x20
#define FAT_ATTR_LFN FAT_ATTR_READ_ONLY | FAT_ATTR_HIDDEN | FAT_ATTR_SYSTEM | FAT_ATTR_VOLUME_ID

// We can cast the sector buffer to one of these
typedef struct fat_extBS_32 {
	uint8_t bootjmp[3];
	char oem_name[8];
	uint16_t bytes_per_sector;
	uint8_t sectors_per_cluster;
	uint16_t reserved_sector_count;
	uint8_t table_count;
	uint16_t root_entry_count;
	uint16_t total_sectors_16;
	uint8_t media_type;
	uint16_t table_size_16;
	uint16_t sectors_per_track;
	uint16_t head_side_count;
	uint32_t hidden_sector_count;
	uint32_t total_sectors_32;

	uint32_t table_size_32;
	uint16_t extended_flags;
	uint16_t fat_version;
	uint32_t root_cluster;
	uint16_t fat_info;
	uint16_t backup_BS_sector;
	uint8_t reserved_0[12];
	uint8_t drive_number;
	uint8_t reserved_1;
	uint8_t boot_signature;
	uint32_t volume_id;
	uint8_t  volume_label[11];
	uint8_t fat_type_label[8];
 
} __attribute__((packed)) fat_fs_bpb32_t;

typedef struct fat_extBS_16 {
	uint8_t bootjmp[3];
	char oem_name[8];
	uint16_t bytes_per_sector;
	uint8_t sectors_per_cluster;
	uint16_t reserved_sector_count;
	uint8_t table_count;
	uint16_t root_entry_count;
	uint16_t total_sectors_16;
	uint8_t media_type;
	uint16_t table_size_16;
	uint16_t sectors_per_track;
	uint16_t head_side_count;
	uint32_t hidden_sector_count;
	uint32_t total_sectors_32;

	uint8_t bios_drive_num;
	uint8_t reserved1;
	uint8_t boot_signature;
	uint32_t volume_id;
	uint8_t volume_label[11];
	uint8_t fat_type_label[8];
 
} __attribute__((packed)) fat_fs_bpb16_t;

typedef struct {
	uint8_t bootjmp[3];
	char oem_name[8];
	uint16_t bytes_per_sector;
	uint8_t sectors_per_cluster;
	uint16_t reserved_sector_count;
	uint8_t table_count;
	uint16_t root_entry_count;
	uint16_t total_sectors_16;
	uint8_t media_type;
	uint16_t table_size_16;
	uint16_t sectors_per_track;
	uint16_t head_side_count;
	uint32_t hidden_sector_count;
	uint32_t total_sectors_32;
} __attribute__((packed)) fat_fs_bpb_t;

typedef struct {
	fat_fs_bpb_t* bpb;

	// either 16 or 32
	uint8_t fat_type;

	uint32_t root_dir_sectors;
	uint32_t root_cluster;
	uint32_t first_data_sector;
	uint32_t first_fat_sector;
	uint32_t data_sectors;
	uint32_t total_clusters;

	// buffers
	void* root_directory;
	uint32_t root_dir_length;
} fat_fs_info_t;

typedef struct {
	char name[8];
	char ext[3];
	
	uint8_t attributes;
	
	uint8_t reserved_0;

	uint8_t time_created_seconds;
	uint16_t created_time;
	
	uint16_t created_date;
	uint16_t accessed_date;

	uint16_t cluster_high;

	uint16_t write_time;
	uint16_t write_date;

	uint16_t cluster_low;

	uint32_t filesize; // in bytes
} __attribute__((packed)) fat_dirent_t;

void fat_read_get_root_dir(fs_superblock_t* superblock, void* buffer, uint32_t buffer_size);
void* fat_read_directory(fs_superblock_t* superblock, char* path);
void* fat_read_file(fs_superblock_t* superblock, char* file, void* buffer, uint32_t buffer_size);

int fat_unmount(fs_superblock_t* superblock);

#endif

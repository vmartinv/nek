#ifndef EXT2_H
#define EXT2_H

#include <fs/fs.h>

#define EXT2_FS_STATE_CLEAN 1
#define EXT2_FS_STATE_ERROR 2

#define EXT2_ERROR_BEHAVIOUR_IGNORE  1
#define EXT2_ERROR_BEHAVIOUR_REMOUNT 2
#define EXT2_ERROR_BEHAVIOUR_PANIC   3

#define EXT2_FT_UNKNOWN 0  // Unknown File Type
#define EXT2_FT_REG_FILE 1 // Regular File
#define EXT2_FT_DIR 2  // Directory File
#define EXT2_FT_CHRDEV 3  // Character Device
#define EXT2_FT_BLKDEV 4  // Block Device
#define EXT2_FT_FIFO 5  // Buffer File
#define EXT2_FT_SOCK 6  // Socket File
#define EXT2_FT_SYMLINK 7  // Symbolic Link

#define EXT2_ROOT_INODE 2

typedef struct ext2_superblock {
  unsigned int total_inodes;
  unsigned int total_blocks;
  unsigned int superuser_blocks;
  unsigned int unallocated_blocks;
  unsigned int unallocated_inodes;

  unsigned int superblock_block_num;
  unsigned int block_size_log;
  unsigned int fragment_size_log;

  unsigned int blocks_per_group;
  unsigned int fragments_per_group;
  unsigned int inodes_per_group;

  unsigned int last_mount; //UNIX timestamps
  unsigned int last_write;

  unsigned short mounts_since_last_fsck;
  unsigned short allowed_mounts_before_fsck;

  unsigned short signature;
  unsigned short state;
  unsigned short err_behaviour;

  unsigned short minor_version;
  unsigned int last_fsck;
  unsigned int interval_fsck;
  unsigned int fs_creator_id;
  unsigned int major_version;

  unsigned short superuser_id;
  unsigned short superuser_groupid;

  // Extended fields
  unsigned int first_free_inode;
  unsigned short inode_size;
  unsigned short containing_superblock; //Block group that this superblock is part of (if backup copy)
  unsigned int optional_features;
  unsigned int required_features;
  unsigned int read_only_features;

  char fs_id[16];
  char vol_name[16];
  char mount_path[64];

  unsigned int compression_alg;
  unsigned char file_preallocate_blocks;
  unsigned char dir_preallocate_blocks;
  unsigned short unused;

  char journal_id[16];
  unsigned int journal_inode;
  unsigned int journal_device;
  unsigned int orphan_inode_list_head;

} ext2_superblock_t;

typedef struct ext2_bgd_entry {
  unsigned int block_usage_bmap;
  unsigned int inode_usage_bmap;
  unsigned int inode_table;
  unsigned short unallocated_blocks;
  unsigned short unallocated_inodes;
  unsigned short directories;
} ext2_bgd_entry_t;

typedef struct ext2_inode {
  unsigned short type_and_perm;
  unsigned short uid;
  unsigned int size_lower;

  unsigned int last_access;
  unsigned int created;
  unsigned int last_modification;
  unsigned int deletion;

  unsigned short gid;
  unsigned short hardlink_count;
  unsigned int sector_count;
  unsigned int flags;
  unsigned int os_value1;

  uint64_t direct_pointers[12];
  uint64_t singly_indirect_pointer;
  uint64_t doubly_indirect_pointer;
  uint64_t triply_indirect_pointer;

  unsigned int gen_number;
  unsigned int ext_attr_block;
  unsigned int size_higher;

  unsigned int fragment_addr;
  char os_value2[12];
} ext2_inode_t;

typedef struct ext2_dir_entry {
  unsigned int inode;
  unsigned short total_size;
  unsigned char type;
  char *name;
} ext2_dir_entry_t;

typedef struct ext2_fs_info {
  ext2_superblock_t *superblock;
  ext2_bgd_entry_t **bgdt;
  partition_info_t *partition;
} ext2_fs_info_t;

ext2_fs_info_t* ext2_init_fs(partition_info_t* p);
#endif

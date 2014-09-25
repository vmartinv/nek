#ifndef FS_H
#define FS_H

#include <arch/x86/lba.h>

#define FS_TYPE_EXT2  0x83
#define FS_TYPE_FAT32 0x0b

#define FS_FILE        0x01
#define FS_DIRECTORY   0x02
#define FS_CHARDEVICE  0x03
#define FS_BLOCKDEVICE 0x04
#define FS_PIPE        0x05
#define FS_SYMLINK     0x06
#define FS_MOUNTPOINT  0x08 // Is the file an active mountpoint?

// File modes: first bit is read/write, second bit is append, third on is binary mode
#define FS_MODE_READ 0
#define FS_MODE_WRITE 2
#define FS_MODE_APPEND 4
#define FS_MODE_BINARY 8

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END -1
typedef struct partition_info {
  char active;
  char start_head;
  char start_sector;
  unsigned short start_cylinder;
  unsigned char system_id;
  unsigned char end_head;
  unsigned char end_sector;
  unsigned short end_cylinder;
  unsigned int start_lba;
  unsigned int total_sectors;

  lba_drive_t *drive;
} partition_info_t;


struct fs_node;
typedef struct fs_node fs_node_t;

typedef unsigned int (*read_type_t)(struct fs_node*,unsigned int,unsigned int,char*);
typedef unsigned int (*write_type_t)(struct fs_node*,unsigned int,unsigned int,char*);
typedef void (*open_type_t)(struct fs_node*);
typedef void (*close_type_t)(struct fs_node*);
typedef struct dirent * (*readdir_type_t)(struct fs_node*,unsigned int);
typedef struct fs_node * (*finddir_type_t)(struct fs_node*,char *name);

typedef struct file {
  unsigned short mode;
  unsigned int pos;
  fs_node_t *node;
} file_t;

typedef struct dir {
  unsigned int pos;
  fs_node_t *node;
} dir_t;

struct dirent {
  char name[128];
  unsigned int inode;
};

struct fs_node {
  char name[128];
  unsigned int mask; // The permissions mask
  unsigned int uid;
  unsigned int gid;
  unsigned int flags;
  unsigned int inode; // Filesystem specific inode
  unsigned int length;
  void *fs; // Pointer to fs specific info

  read_type_t read;
  write_type_t write;
  open_type_t open;
  close_type_t close;
  readdir_type_t readdir; // Returns the n'th child of a directory.
  finddir_type_t finddir; // Try to find a child in a directory by name.

  struct fs_node *ptr; // For mountpoints and symlinks
};

extern fs_node_t *fs_root; // The root of the filesystem

int init_fs();

unsigned int fs_read(fs_node_t *node, unsigned int offset, unsigned int size, char *buffer);
unsigned int fs_write(fs_node_t *node, unsigned int offset, unsigned int size, char *buffer);
void fs_open(fs_node_t *node, unsigned char read, unsigned char write);
void fs_close(fs_node_t *node);
struct dirent *fs_readdir(fs_node_t *node, unsigned int index);
fs_node_t *fs_finddir(fs_node_t *node, char *name);

dir_t* opendir(char *path);
struct dirent* readdir(dir_t *d);
file_t* fopen(const char *path, const char *mode);
void fclose(file_t* f);
unsigned int fread(void *ptr, unsigned int size, unsigned int count, file_t *f);
void fseek(file_t *f, unsigned int offset, int whence);
unsigned int ftell(file_t *f);

partition_info_t* parse_partition_info(char* sector);
partition_info_t** init_partition_table(lba_drive_t *d);
void fs_read_sectors(partition_info_t *partition,
                     unsigned long address,
                     unsigned short count,
                     unsigned char *buff);
#endif

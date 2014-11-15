#ifndef VFS_H
#define VFS_H
#include <types.h>

#define VFS_FILE        0x01
#define VFS_DIRECTORY   0x02
#define VFS_CHARDEVICE  0x03
#define VFS_BLOCKDEVICE 0x04
#define VFS_PIPE        0x05
#define VFS_SYMLINK     0x06
#define VFS_MOUNTPOINT  0x08

#define VFS_PERMISSION_READ  0x01
#define VFS_PERMISSION_WRITE 0x02
#define VFS_PERMISSION_EXECUTE 0x04

#define PATH_SEPARATOR '/'
#define PATH_SEPARATOR_STRING "/"
#define PATH_UP  ".."
#define PATH_DOT "."

struct vfs_node;

typedef uint32_t (*read_type_t) (struct vfs_node *, uint32_t, uint32_t, uint8_t *);
typedef uint32_t (*write_type_t) (struct vfs_node *, uint32_t, uint32_t, uint8_t *);
typedef void (*open_type_t) (struct vfs_node *);
typedef void (*close_type_t) (struct vfs_node *);
typedef void (*ioctl_type_t) (struct vfs_node *, long int, uint8_t *);
typedef struct dirent *(*readdir_type_t) (struct vfs_node *, uint32_t);
typedef struct vfs_node *(*finddir_type_t) (struct vfs_node *, const char *name);

typedef struct vfs_fnode
{
	char name[128];
	uint32_t mask;
} vfs_fnode_t;

typedef struct vfs_dnode
{
	char name[128];
	uint32_t mask;
} vfs_dnode_t;

typedef struct vfs_node
{
	uint32_t type;
	uint32_t flags;
	size_t size;
	read_type_t read;
	write_type_t write;
	open_type_t open;
	close_type_t close;
	ioctl_type_t ioctl;
	readdir_type_t readdir;
	finddir_type_t finddir;
	struct vfs_node *ptr; // Used by mountpoints and symlinks.
	void *internal;
	union
	{
		vfs_fnode_t * fnode;
		vfs_dnode_t * dnode;
	};
} vfs_node_t;


typedef struct file {
  unsigned short mode;
  unsigned int pos;
  vfs_node_t *node;
} file_t;

typedef struct dir {
  unsigned int pos;
  vfs_node_t *node;
} dir_t;

typedef struct dirent // One of these is returned by the readdir call, according to POSIX.
{
	char d_name[128]; // Filename.
	uint32_t ino;     // Inode number. Required by POSIX.
} dirent_t;

// File modes: first bit is read/write, second bit is append, third on is binary mode
#define FS_MODE_READ 0
#define FS_MODE_WRITE 2
#define FS_MODE_APPEND 4
#define FS_MODE_BINARY 8

int init_vfs(void *initrd);
int vfs_mount(vfs_node_t * node, char* mountPoint);
#endif

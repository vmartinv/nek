#include <types.h>
#include <fs/vfs.h>
#include <lib/hashmap.h>
#include <stdlib.h>
#include <sys/logging.h>
#include <string.h>
#include <errno.h>

static fs_type_t* vfs_find_fs(uint16_t type);

static fs_type_t* fs_map;
static fs_type_t* fs_last;

static hashmap_t* mountPointMap;

/*
 * Initialises the VFS subsystem
 */
static int vfs_init(void) {
	mountPointMap = hashmap_allocate();
	ASSERT(mountPointMap != NULL);

	return 0;
}

//~ module_early_init(vfs_init); //TODO

/*
 * Registers a filesystem with the kernel.
 */
int vfs_register(fs_type_t* fs) {
	fs->next = NULL;
	fs->prev = NULL;

	// Check if there isn't any filesystems loaded
	if(!fs_map) {
		fs_map = fs;
		fs_last = fs;
	} else {
		fs->prev = fs_last;

		fs_last->next = fs;
		fs_last = fs;
	}

	return 0;
}

/*
 * Unregisters a filesystem with the kernel. Note that this does not relinquish
 * memory allocated by the filesystem, nor the "type" structure.
 */
void vfs_deregister(fs_type_t* fs) {
	fs_type_t *fs_list_ptr = fs_map;
	ASSERT(fs_list_ptr != NULL);

	// Loop through all filesystems
	while(true) {
		if(fs_list_ptr == fs) {
			// Remove from linked list
			if(fs->prev) {
				fs_type_t* prev = fs->prev;
				prev->next = fs->next;
			}

			// Update pointer to last element in list
			if(fs == fs_last) {
				fs_last = fs_last->prev;
			}
		}

		fs_list_ptr = fs_list_ptr->next;
		if(!fs_list_ptr) break;
	}
}

/*
 * Mounts all filesystems in a certain partition map.
 */
void vfs_mount_all(ptable_t* pt) {
	ptable_entry_t* partInfo = pt->first;

	// Process each filesystem
	while(true) {
		vfs_mount_filesystem(partInfo, "/");

		partInfo = partInfo->next;
		if(!partInfo) break;
	}
}

/*
 * Mounts a specific filesystem.
 */
int vfs_mount_filesystem(ptable_entry_t* fs, char* mountPoint) {
	// Is something already mounted here?
	fs_superblock_t *superblock;

	if((superblock = hashmap_get(mountPointMap, mountPoint)) != NULL) {
		printk("vfs", "A filesystem is already mounted at %s!\n", mountPoint);
		return -1;
	}

	// Get the appropriate filesystem
	fs_type_t* fdrv = vfs_find_fs(fs->type);

	// If we have one, mount it
	if(likely(fdrv != NULL)) {
		printk("vfs", "Mounting partition %u: Type 0x%X (%s), start 0x%X, length 0x%X sectors at %s\n", fs->part_num, fs->type, fdrv->name, fs->lba_start, fs->lba_length, mountPoint);

		// Allocate memory for the superblock
		superblock = (fs_superblock_t*) malloc(sizeof(fs_superblock_t));
		memset(superblock, 0, sizeof(fs_superblock_t));

		ASSERT(superblock != NULL);

		superblock->disk = ((ptable_t *)fs->ptable)->disk;
		superblock->pt = fs;

		fs_superblock_t *ret = fdrv->create_super(superblock, fs);

		ret->vol_mount_point = mountPoint;

		// Add to mountpoint hashmap
		hashmap_insert(mountPointMap, mountPoint, superblock);

		return (int) superblock;
	} else {
		printk("vfs", "Unknown filesystem type 0x%X\n", fs->type);
		return -1;
	}

	return 0;
}

/*
 * Locates the pointer to a filesystem to handle a certain type.
 */
static fs_type_t* vfs_find_fs(uint16_t type) {
	fs_type_t *fs_list_ptr = fs_map;
	ASSERT(fs_list_ptr != NULL);

	// Loop through all filesystems
	while(true) {
		if(fs_list_ptr->type == type) return fs_list_ptr;

		fs_list_ptr = fs_list_ptr->next;
		if(!fs_list_ptr) break;
	}

	return NULL;
}

/*
 * Tries to unmount the filesystem that is mounted at the specified place.
 */
int vfs_unmount(char* mountPoint) {
	fs_superblock_t* superblock = hashmap_get(mountPointMap, mountPoint);

	// Error out if there's no superblock
	if(unlikely(!superblock)) {
		errno = ENOTFOUND;
		goto done;
	}

	int unmountStatus = (superblock->fp_unmount)(superblock);

	// If filesystem encountered an error, jump
	if(unlikely(unmountStatus != 0)) {
		errno = unmountStatus;
		goto done;
	}

	// Remove from hashmap anmd clean up
	hashmap_delete(mountPointMap, mountPoint);
	free(superblock);

done:;
	return errno;
}

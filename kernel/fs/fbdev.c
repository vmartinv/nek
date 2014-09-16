#include <stdint.h>
#include <stddef.h>
#include <vfs.h>
#include <string.h>
#include <stdlib.h>
#define FBDEV_SIZE 80 * 25
uint8_t * vfb_ptr[6];

uint32_t fbdev_read(vfs_node_t *node, uint32_t offset, uint32_t size, uint8_t *buffer)
{
	//Offset is the term number. Size is how much data, and buffer is well, the data.
	
	return 0;
}

uint32_t fbdev_write(vfs_node_t *node, uint32_t offset, uint32_t size, uint8_t *buffer)
{
	return 0;
}

uint32_t fbdev_ioctl(vfs_node_t * node, unsigned long request, uint8_t * buffer)
{
	return 0;
}

vfs_node_t * fbdev_create(int term)
{
	vfs_node_t * node = (vfs_node_t*)malloc(sizeof(vfs_node_t));
	memset(node,0,sizeof(vfs_node_t));

	node->fnode = (vfs_fnode_t*)malloc(sizeof(vfs_fnode_t));
	strcpy(node->fnode->name, "fb0");

	node->type = VFS_FILE;
	node->read = (read_type_t)&fbdev_read;
	node->write = (write_type_t)&fbdev_write;
	node->ioctl = (ioctl_type_t)&fbdev_ioctl;
	
	vfb_ptr[term] = malloc(FBDEV_SIZE);
	return node;
}


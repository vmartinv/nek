#include <stdint.h>
#include <stddef.h>
#include <vfs.h>
#include <string.h>
#include <stdlib.h>
#define DEBUGDEV_BUFFERSIZE 0x1000
uint8_t * debug_buffer;

vfs_node_t * debug_vfs_node;

uint32_t debugdev_locked = 0;

uint32_t debugdev_read(vfs_node_t *node, uint32_t offset, uint32_t size, uint8_t *buffer)
{
	if(offset + size > DEBUGDEV_BUFFERSIZE)
	{
		return 0xFFFFFFFF;
	}
	for(uint32_t i = 0; i != size; i++)
	{
		buffer[i] = debug_buffer[i + offset];
	}
	return 0;
}

uint32_t debugdev_write(vfs_node_t *node, uint32_t offset, uint32_t size, uint8_t *buffer)
{
	if(offset + size > DEBUGDEV_BUFFERSIZE)
	{
		return 0xFFFFFFFF;
	}
	if(debugdev_locked)
	{
		return 1;
	}
	for(uint32_t i = 0; i != size; i++)
	{
		debug_buffer[i + offset] = buffer[i];
	}
	return 0;
}

uint32_t debugdev_ioctl(vfs_node_t * node, unsigned long request, uint8_t * buffer)
{
	if(request == 0)
	{
		debugdev_locked = 0;
	}
	else if (request == 1)
	{
		debugdev_locked = 1;
	}
	else if (request == 2)
	{
		memset(debug_buffer,0,0x1000); // Clear buffer
	}
	return 0;
}

vfs_node_t * debugdev_getnode()
{
	return debug_vfs_node;
}

vfs_node_t * setup_debugdev()
{
	debug_vfs_node = (vfs_node_t*)malloc(sizeof(vfs_node_t));
	memset(debug_vfs_node,0,sizeof(vfs_node_t));

	debug_vfs_node->fnode = (vfs_fnode_t*)malloc(sizeof(vfs_fnode_t));
	strcpy(debug_vfs_node->fnode->name, "null");

	debug_vfs_node->type = VFS_FILE;
	debug_vfs_node->read = (read_type_t)&debugdev_read;
	debug_vfs_node->write = (write_type_t)&debugdev_write;
	debug_vfs_node->ioctl = (ioctl_type_t)&debugdev_ioctl;
	
	debug_buffer = malloc(DEBUGDEV_BUFFERSIZE); //Allocate 4k
	return debug_vfs_node;
}


#ifndef INITRD_H
#define INITRD_H

#include <fs/vfs.h>
#include <lib/tree.h>

vfs_node_t *load_initrd(void *initrd);

#endif

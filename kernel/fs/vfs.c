#include <vfs.h>
#include <lib/tree.h>
#include <logging.h>
#include <memory.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
tree_t * vfs_tree; 
vfs_node_t * vfs_root;
void debug_print_node(vfs_node_t * node);

int init_vfs()
{
	//Create the tree and ensure its valid
	vfs_tree = tree_create();
	if(vfs_tree == NULL)
	{
		printk("fail","=> Couldn't create tree\n");
		return 1;
	}
	//Create root node and then set it as root. This is only temporary until
	//A filesystem is mounted on /, otherwise it will contain nothing but a
	//directory entry listing all of the mountpoints.
	vfs_root = malloc(sizeof(vfs_node_t));
	memset(vfs_root,0,sizeof(vfs_node_t));

	vfs_root->type = VFS_DIRECTORY | VFS_MOUNTPOINT;
	vfs_root->dnode = malloc(sizeof(vfs_dnode_t));
	vfs_root->dnode->name[0] = '/';

	tree_set_root(vfs_tree,vfs_root);

	vfs_node_t * test = malloc(sizeof(vfs_node_t));
	test->fnode = malloc(sizeof(vfs_fnode_t));
	test->type = VFS_FILE;
	strcpy(test->fnode->name,"helloworld");

	tree_node_insert_child(vfs_tree, vfs_tree->root, tree_node_create(test));
	return 0;
}

char * vfs_canonicalize_path(char *cwd, char *input) {
	list_t *out = list_create();

	if (strlen(input) && input[0] != PATH_SEPARATOR) {
		char *path = malloc((strlen(cwd) + 1) * sizeof(char));
		memcpy(path, cwd, strlen(cwd) + 1);

		char *pch;
		char *save;
		pch = strtok_r(path,PATH_SEPARATOR_STRING,&save);

		while (pch != NULL) {
			char *s = malloc(sizeof(char) * (strlen(pch) + 1));
			memcpy(s, pch, strlen(pch) + 1);

			list_insert(out, s);
			pch = strtok_r(NULL,PATH_SEPARATOR_STRING,&save);
		}
		free(path);
	}

	char *path = malloc((strlen(input) + 1) * sizeof(char));
	memcpy(path, input, strlen(input) + 1);

	char *pch;
	char *save;
	pch = strtok_r(path,PATH_SEPARATOR_STRING,&save);

	while (pch != NULL) {
		if (!strcmp(pch,PATH_UP)) {
			list_node_t * n = list_pop(out);
			if (n) {
				free(n->value);
				free(n);
			}
		} else if (!strcmp(pch,PATH_DOT)) {

		} else {
			char * s = malloc(sizeof(char) * (strlen(pch) + 1));
			memcpy(s, pch, strlen(pch) + 1);
			list_insert(out, s);
		}
		pch = strtok_r(NULL, PATH_SEPARATOR_STRING, &save);
	}
	free(path);

	size_t size = 0;
	foreach(item, out) {
		size += strlen(item->value) + 1;
	}

	char *output = malloc(sizeof(char) * (size + 1));
	char *output_offset = output;
	if (size == 0) {
		output = realloc(output, sizeof(char) * 2);
		output[0] = PATH_SEPARATOR;
		output[1] = '\0';
	} else {
		foreach(item, out) {
			output_offset[0] = PATH_SEPARATOR;
			output_offset++;
			memcpy(output_offset, item->value, strlen(item->value) + 1);
			output_offset += strlen(item->value);
		}
	}

	list_destroy(out);
	list_free(out);
	free(out);

	return output;
}


int vfs_add_node(char * path, vfs_node_t * node)
{
	//char * apath = vfs_canonicalize_path("/", path);
	char * parent_path = malloc(strlen(path) + 4);
	sprintf(parent_path, "%s", path);
	printf("parent path: %s\n",parent_path);
	return 0;
}


uint8_t vfs_tree_comparator(vfs_node_t * a, vfs_node_t * b)
{
	if(a == b)
	{
		return 1;
	}
	return 0;
}

uint32_t vfs_read(vfs_node_t * node, uint32_t offset, uint32_t size, uint8_t *buffer)
{
	if(node != NULL && node->read != NULL && ((node->type & VFS_DIRECTORY) != VFS_DIRECTORY))
	{
		return (uint32_t)node->read(node,offset,size,buffer);
	}
	else
	{
		printk("error","vfs_node 0x%X does not meet the requirements to read\n",node);
		return 0;
	}
}

uint32_t vfs_write(vfs_node_t * node, uint32_t offset, uint32_t size, uint8_t *buffer)
{
	if(node != NULL && node->write != NULL && ((node->type & VFS_DIRECTORY) != VFS_DIRECTORY))
	{
		return (uint32_t)node->write(node,offset,size,buffer);
	}
	else
	{
		printk("error","vfs_node 0x%X does not meet the requirements to write\n",node);
		return 0;
	}
}

void vfs_ioctl(vfs_node_t * node, unsigned long request, uint8_t * buffer)
{
	if(node != NULL && node->ioctl != NULL)
	{
		node->ioctl(node,request,buffer);
	}
	else
	{
		printk("error","vfs_node 0x%X does not meet the requirements to be ioctl'd\n",node);
	}
}

void vfs_open(vfs_node_t * node, unsigned int flags)
{
	if(node != NULL && node->open != NULL)
	{
		node->open(node,flags);
	}
	else
	{
		printk("error","vfs_node 0x%X does not meet the requirements to be opened\n",node);
	}
}

vfs_node_t * vfs_openpath(char * input, char *cwd,uint32_t flags)
{
	char * path = vfs_canonicalize_path(cwd, input);
	printf("vfs_open: %s\n",path);
	size_t path_len = strlen(path);
	if(path_len == 1)
	{
		return vfs_root;
	}
	return NULL;
}


void debug_print_node(vfs_node_t * node)
{
	if(node->type ==  VFS_DIRECTORY  ||
	   node->type ==  VFS_MOUNTPOINT ||
	   node->type == (VFS_DIRECTORY  |  VFS_MOUNTPOINT))
	{
		printk("dir","\'%s\' of type 0x%x\n",node->dnode->name,node->type);
	}
	else
	{
		printk("file","\'%s\' of type 0x%x\n",node->fnode->name,node->type);
	}
	//Search tree for vfs node
	tree_node_t * tree_node = tree_find(vfs_tree, node, (tree_comparator_t)vfs_tree_comparator);

	if(tree_node == NULL)
	{
		printk("no","\'%s\' is not in the tree\n",node->fnode->name);
	}
	else
	{
		printk("yes","\'%s\' is in the tree (0x%x)\n",node->fnode->name,tree_node);
		printk("yes","->\tIt has %d/%d children\n",
			tree_node->children->length,vfs_tree->nodes - 1);
		
	}
}
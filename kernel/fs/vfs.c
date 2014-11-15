#include <fs/vfs.h>
#include <fs/initrd.h>
#include <lib/tree.h>
#include <sys/logging.h>
#include <stdlib.h>
#include <types.h>
#include <stdio.h>
#include <string.h>
#include <stdio.h>
vfs_node_t * vfs_root;


void debug_print_vfs(vfs_node_t * node);
int init_vfs(void *initrd){
	vfs_root=load_initrd(initrd);
	if(!vfs_root) return 1;
	//~ debug_print_vfs(vfs_tree->root);
	return 0;
}

void vfs_ioctl(vfs_node_t * node, unsigned long request, uint8_t * buffer)
{
	if(node != NULL && node->ioctl != NULL)
		node->ioctl(node,request,buffer);
	else
		printk("error","vfs_node 0x%X does not meet the requirements to be ioctl'd\n",node);
}


//~ vfs_node_t *fs_finddir(tree_node_t *tnode, const char *name) {//TODO: it only searchs in the first level
	//~ vfs_node_t *node=tnode->value;
	//~ if(node->type&VFS_FILE){
		//~ if(!strcmp(node->fnode->name, name))
			//~ return node;
	//~ }
	//~ if (!(node->type&VFS_DIRECTORY)) return NULL;
	//~ foreach(child, tnode->children){
		//~ vfs_node_t *res=fs_finddir(child->value, name);
		//~ if(res) return res;
	//~ }
	//~ return NULL;
//~ }


void debug_print_vfs(vfs_node_t * node){
	if(node->type&VFS_DIRECTORY  || node->type&VFS_MOUNTPOINT){
		printk("dir","\'%s\' of type 0x%x\n",node->dnode->name,node->type);
		//~ foreach(child, tnode->children){
			//~ debug_print_vfs(child->value);
		//~ }
	}
	else{
		printk("file","\'%s\' of type 0x%x\n",node->fnode->name,node->type);
	}
}

//~ long int fs_write(fs_node_t *node, size_t offset, size_t size, char *buff) {
	//~ if (node->write != 0)
		//~ return node->write(node, offset, size, buff);
	//~ else
		//~ return 0;
//~ }

dir_t* opendir(const char *path) {
	if(!vfs_root->finddir) return NULL;
	vfs_node_t *node = vfs_root->finddir(vfs_root, path);
	if(!node) return NULL;
	dir_t *d = (dir_t*)malloc(sizeof(dir_t));
	d->pos = 0;
	d->node = node;
	return d;
}

dirent_t* readdir(dir_t *d) {
	if(!(d->node->type&VFS_DIRECTORY) || !d->node->readdir ) return NULL;
	return d->node->readdir(d->node, d->pos++);
}



file_t* fopen(const char *path, const char *mode) {
	unsigned char mode_flags=0;
	vfs_node_t *node = vfs_root->finddir(vfs_root, path);
	if(!node) return NULL;
	for(int i=0; mode[i]; i++) {
		switch(mode[i]) {
		case 'r':
			mode_flags |= FS_MODE_READ;
			break;
		case 'w':
			mode_flags |= FS_MODE_WRITE;
			break;
		case 'b':
			mode_flags |= FS_MODE_BINARY;
			break;
		}
	}
	node->open(node);
	file_t *f = (file_t*)malloc(sizeof(file_t));
	f->mode = mode_flags;
	f->node = node;
	f->pos = 0;
	return f;
}

void fclose(file_t* f) {
	f->node->close(f->node);
	//~ free(f->node);
	free(f);
}

void closedir(dir_t*d){}

long int fread(void *ptr, size_t size, size_t count, file_t *f) {
	long int tot_read;
	tot_read = f->node->read(f->node, f->pos, size*count, (uint8_t*)ptr);
	f->pos += tot_read;
	return tot_read;
}

char fgetc(file_t *f){
	char ret=0;
	fread(&ret, 1, 1, f);
	return ret;
}

void fseek(file_t *f, long int offset, int whence) {
	switch(whence) {
	case SEEK_SET:
		f->pos = offset;
		break;
	case SEEK_CUR:
		f->pos += offset;
		break;
	case SEEK_END:
		f->pos = f->node->size-offset;
		break;
	}
}

long int ftell(file_t *f) {
	return f->pos;
}


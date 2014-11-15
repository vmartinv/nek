#include <fs/initrd.h>
#include <string.h>
#include <stdlib.h>
const u32 magic=0xBF;

typedef struct initrd_header
{
   u32 magic; // The magic number is there to check for consistency.
   char name[64];
   u32 length; // Length of the file.
}initrd_header_t;


dirent_t *initrd_readdir(vfs_node_t *node, uint32_t pos){
	if(pos>=node->size) return NULL;
	vfs_node_t *next=&((vfs_node_t*)node->internal)[pos];
	static dirent_t ret;
	if(next->type&VFS_FILE) strcpy(ret.d_name, next->fnode->name);
	else  strcpy(ret.d_name, next->dnode->name);
	return &ret;
}

vfs_node_t *initrd_finddir(vfs_node_t *node, const char *name){
	if(!strcmp(name, ".") || !strcmp(name, "/") || !strcmp(name, "./")) return node;
	vfs_node_t *files=(vfs_node_t *)node->internal;
	for(unsigned i=0; i<node->size; i++){
		if(files[i].type&VFS_FILE && !strcmp(name, files[i].fnode->name)) return &files[i];
	}
	return NULL;
}

uint32_t initrd_read(vfs_node_t *node, uint32_t pos, uint32_t count, uint8_t *dst){
	uint32_t toread=count;
	if(pos+toread > node->size) toread=node->size-pos;
	memcpy((void*)dst, node->internal+pos, toread);
	return toread;
}

void initrd_close(vfs_node_t *node){}
void initrd_open(vfs_node_t *node){}

vfs_node_t *load_initrd(void *initrd){
	assert(*(u32*)(initrd)==magic);
	initrd+=sizeof(u32);
	u32 nheaders=*(u32*)(initrd);
	initrd+=sizeof(u32);
	
	vfs_node_t *files=(vfs_node_t*)malloc(sizeof(vfs_node_t)*nheaders);
	memset(files,0,sizeof(vfs_node_t)*nheaders);
	
	vfs_node_t * vfs_root = (vfs_node_t *)malloc(sizeof(vfs_node_t));
	memset(vfs_root,0,sizeof(vfs_node_t));

	vfs_root->type = VFS_DIRECTORY | VFS_MOUNTPOINT;
	vfs_root->dnode = malloc(sizeof(vfs_dnode_t));
	vfs_root->dnode->name[0] = '/';
	vfs_root->size=nheaders;
	vfs_root->readdir=initrd_readdir;
	vfs_root->finddir=initrd_finddir;
	vfs_root->internal=(void*)files;
	
	for(u32 i=0; i<nheaders; i++){
		initrd_header_t *header=(initrd_header_t*)initrd;
		initrd+=sizeof(initrd_header_t);
		assert(header->magic==magic);
		vfs_node_t *file=&files[i];
		file->type = VFS_FILE;
		file->fnode = (vfs_fnode_t*)malloc(sizeof(vfs_fnode_t));
		strcpy(file->fnode->name, header->name);
		file->size=header->length;
		file->internal=initrd;
		file->open=initrd_open;
		file->close=initrd_close;
		file->read=initrd_read;
		initrd+=header->length;
	}
	return vfs_root;
}





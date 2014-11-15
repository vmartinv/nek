#include <sys/logging.h>
#include <sys/multiboot.h>
#include <stdio.h>
#include <sys/timer.h>
#include <sys/syscall.h>
#include <sys/task.h>
#include <fs/vfs.h>
#include <arch/x86/lba.h>
#include <fs/mbr.h>
#include <fs/fat.h>
void kbd_init();

//~ int init_vfs(const multiboot_info_t * multiboot){
	//~ if(lba_init()) return 1;
	//~ const ptable_entry_t * pe=mbr_get_partition(mbr_get_partition_table());
	//~ if(!pe) return 2;
	//~ if(fat_init()) return 3;
	//~ return 0;
//~ }

int start_service(const char *daemon,int essential, int res)
{
	if(res){
		printk("daemon","Starting kernel daemon %s... failed\n",daemon);
		if(essential) panic("Couldn't start required daemon!");
		return 1;
	}
	else printk("daemon","Starting kernel daemon %s... done\n",daemon);
	return 0;
}

int main();

void kmain(const multiboot_info_t * multiboot)
{
	printk("ok","Entering Kernel Environment...\n");
	kbd_init();
	//Start services
	start_service("vfsd",1,init_vfs((void*)*((u32*)multiboot->mods_addr)));
	//~ start_service("filesystemd",1,init_fs(multiboot));
	start_service("timerd",1,init_timer());
	
	
		// Disk test
	/*disk_t *hda0 = disk_allocate();
	hda0->bus = 0;
	hda0->drive_id = 0;
	ata_driver_init(hda0);
	DISK_ERROR ret = disk_init(hda0);

	if(ret != kDiskErrorNone) {
		printk("kmain", "hd0 initialisation error: 0x%X\n", ret);
	} else {
		ptable_t* mbr = mbr_load(hda0);

		// Mount the root filesystem
		ptable_entry_t* partInfo = mbr->first;
		fs_superblock_t *superblock = (fs_superblock_t *) vfs_mount_filesystem(partInfo, "/");

		// Try to make a new task from the ELF we loaded
		//~ void* elfFile = fat_read_file(superblock, "/TEST.ELF", NULL, 0);
		//~ elf_file_t *elf = elf_load_binary(elfFile);
		//~ kprintf("\nParsed ELF to 0x%X\n", elf);
//~ 
		//~ i386_task_t* task = task_allocate(elf);
	}*/
	main();
}

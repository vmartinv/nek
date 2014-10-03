#include <sys/logging.h>
#include <stdio.h>
//~ #include <sys/vfs.h>
#include <sys/timer.h>
#include <sys/syscall.h>
#include <sys/task.h>
//~ #include <fs/disk.h>
//~ #include <fs/mbr.h>
//~ #include <fs/vfs.h>
#include <arch/x86/lba.h>
void kbd_init();

int start_service(char *daemon,int essential,int (*func)())
{
	if(func()){
		printk("daemon","Starting kernel daemon %s... failed\n",daemon);
		if(essential) panic("Couldn't start required daemon!");
		return 1;
	}
	else printk("daemon","Starting kernel daemon %s... done\n",daemon);
	return 0;
}

int main();

void kmain()
{
	printk("ok","Entering Kernel Environment...\n");
	kbd_init();
	//Start services
	//~ start_service("syscalld",1,init_syscalls);
	//~ start_service("taskd",1,task_init);
	//~ start_service("kvfsd",0,init_vfs);
	//start_service("filesystemd",1,init_fs);
	start_service("timerd",1,init_timer);
	verify_DPT();
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

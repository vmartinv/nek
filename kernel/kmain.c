#include <sys/logging.h>
#include <sys/multiboot.h>
#include <stdio.h>
#include <sys/timer.h>
#include <sys/syscall.h>
#include <sys/task.h>
#include <fs/vfs.h>
#include <arch/x86/lba.h>
#include <arch/x86/pci/pci.h>
//~ #include <arch/x86/bus.h>
#include <fs/mbr.h>
#include <fs/fat.h>

void kbd_init();

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
	//Start services
	start_service("vfsd",1,init_vfs((void*)*((u32*)multiboot->mods_addr)));
	//~ start_service("filesystemd",1,init_fs(multiboot));
	start_service("timerd",1,init_timer());
	PciInit();
	//~ bus_sys_init();
	//~ pci_init();
	//~ pci_load_drivers();
	
	main();
}

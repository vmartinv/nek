#include <stdint.h>
#include <stdio.h>
#include <multiboot.h>
#include <elf.h>
void bs_init_console();
void print(const char *c);

int magic_no; 
multiboot_info_t * mb;
void bootstrap_init(int magic, multiboot_info_t * multiboot)
{
	//Save Data
	magic_no = magic;
	mb = multiboot;
	//Setup Console Immedately so we can print status
	bs_init_console();
	printf("Cedille x64 Bootstrap - v 0.0.0.1 (c) Corwin McKnight 2014\n");

	if(magic_no != MULTIBOOT_BOOTLOADER_MAGIC)
	{
		print("=> Boot was unsucessfull. Halting...");
		return;
	}
	printf("=> Boot verified\n");

	int mods_loaded = multiboot->mods_count;
	if(mods_loaded == 0)
	{
		printf("=> No kernel image was loaded into memory; Adjust your bootloader's paramaters.\n");
		printf("=> Halting\n");
	}

	//Assume first passed module is kernel and try to boot it
	//Get its address
	multiboot_module_t * mod = (multiboot_module_t*)multiboot->mods_addr;
	uint32_t kernel_start = mod->mod_start;

	printf("=> Booting image in memory (@0x%X)\n",kernel_start);
	Elf64_Ehdr * kernel = (Elf64_Ehdr*)kernel_start;

	if(kernel->e_ident[4] != 2) // 2 signifies 64bit
	{
		printf("=> 64 bit kernel required!\n");
		return;
	}
	else
		printf("=> 64 bit ELF\n");
	printf("Executing at 0x%X\n",kernel->e_entry);

	
}
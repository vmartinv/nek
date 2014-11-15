#include <types.h>
#include <sys/logging.h>
#include <arch/x86/descriptor_tables.h>
#include <arch/x86/ports.h>


/* Skelix by Xiaoming Mo (skelixos@gmail.com)
 * Licence: GPLv2 */
#include <arch/x86/lba.h>

/* IDT_ADDR and GDT_ADDR must align to 16 */
#define IDT_ADDR	0x80000
#define IDT_SIZE	(256*8)
#define GDT_ADDR	((IDT_ADDR)+IDT_SIZE)
#define GDT_ENTRIES	5
#define GDT_SIZE	(8*GDT_ENTRIES)
/* 
   NULL				0	0
   CODE_SEL			8	1
   DATA_SEL			10	2
   CURR_TASK_TSS	18	3
   CURR_TASK_LDT	20	4
 */
#define HD0_ADDR	(GDT_ADDR+GDT_SIZE)
#define HD0_SIZE	(4*8)


struct HD_PARAM {
	unsigned int cyl;
	unsigned int head;
	unsigned int sect;
} HD0 = {208, 16, 63};

int lba_init(){
	return 0;
}

/* lba:	starts from 0 
 start_sect: starts from 1 */
int lba_rw(unsigned int lba, unsigned int com, unsigned int sects_to_access, void *buf) {
	/* lba to chs */
	/* cylinder = LBA / (heads_per_cylinder * sectors_per_track)
	   temp = LBA % (heads_per_cylinder * sectors_per_track)
	   head = temp / sectors_per_track
	   sector = temp % sectors_per_track + 1 */
	unsigned int cyl = lba/(HD0.head * HD0.sect);
	unsigned int head = (lba%(HD0.head*HD0.sect))/HD0.sect;
	unsigned int sect = (lba%(HD0.head*HD0.sect))%HD0.sect+1;

#ifdef HD_DEBUG
	if (com == HD_WRITE)
		printk("lba", "[w");
	else
		printk("lba", "[r");
	printk("lba", "%d]", lba);
#endif
	while ((inb(HD_PORT_STATUS)&0xc0)!=0x40);
	outb(HD_PORT_SECT_COUNT, sects_to_access);
	outb(HD_PORT_SECT_NUM, sect);
	outb(HD_PORT_CYL_LOW, cyl&0xFF);
	outb(HD_PORT_CYL_HIGH, cyl>>8);
	outb(HD_PORT_DRV_HEAD, 0xa0|head);
	outb(HD_PORT_COMMAND, com);
	while (! (inb(HD_PORT_STATUS)&0x8))
		;
	if (com == HD_READ)
		insl(HD_PORT_DATA, sects_to_access<<7, buf);
	else if (com == HD_WRITE)
		outsl(HD_PORT_DATA, sects_to_access<<7, buf);
	return 0;
}

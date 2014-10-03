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

/* lba:	starts from 0 
 start_sect: starts from 1 */
void hd_rw(unsigned int lba, unsigned int com, unsigned int sects_to_access, void *buf) {
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
}
/* DANGEROUS
static void setup_DPT(void) {
	unsigned char sect[512] = {0};
	sect[0x1be] = 0x80;
	sect[0x1be + 0x04] = FST_FS;
	*(unsigned long *)&sect[0x1be + 0x08] = 1;
	*(unsigned long *)&sect[0x1be + 0x0c] = 85*1024*2; //85MB
	sect[0x1ce + 0x04] = FST_SW;
	*(unsigned long *)&sect[0x1ce + 0x08] = 85*1024*2+1;
	*(unsigned long *)&sect[0x1ce + 0x0c] = 16*1024*2; // 16MB
	sect[0x1fe] = 0x55;
	sect[0x1ff] = 0xaa;
	hd_rw(0, HD_WRITE, 1, sect);
}*/

void verify_DPT(void) {
	unsigned char sect[512];
	unsigned i = 0;
	unsigned int *q = (unsigned int *)(HD0_ADDR);

	hd_rw(0, HD_READ, 1, sect);
	if ((sect[0x1fe]==0x55) && (sect[0x1ff]==0xaa)) {
		unsigned char *p = &sect[0x1be];
		char *s;
		printk("lba", "   | Bootable | Type      | Start Sector | Capacity \n");
		for (i=0; i<4; ++i) {
			printk("lba", " %d ", i);
			/* system indicator at offset 0x04 */
			if (p[0x04] == 0x00) {
				printk("lba", "| Empty\n");
				p += 16;
				q += 2;
				continue;
			}
			if (p[0x00] == 0x80)
				s = "| Yes      ";
			else
				s = "| No       ";
			printk("lba", s);
			/* system indicator at offset 0x04 */
			if (p[0x04] == FST_FS) {
				printk("lba", "| Skelix FS ");
			} else if (p[0x04] == FST_SW) {
				printk("lba", "| Skelix SW ");
			} else
				printk("lba", "| Unknown   ", *p);
			/* starting sector number */
			*q++ = *(unsigned long *)&p[0x08];
			printk("lba", "| 0x%x   ", *(unsigned long*)&p[0x08]);
			/* capacity */
			*q++ = *(unsigned long*)&p[0x0c];
			printk("lba", "| %dM\n", (*(unsigned long*)&p[0x0c]*512)>>20);
			p += 16;
		}
	} else {
		printk("lba", "No bootable DPT found on HD0\n");
		printk("lba", "Creating DPT on HD0 automaticly\n");
		printk("lba", "Creating file system whatever you want it or not!!\n");
	}
}

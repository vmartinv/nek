#ifndef LBA_H
#define LBA_H

#include <types.h>
/* Skelix by Xiaoming Mo (skelixos@gmail.com)
 * Licence: GPLv2 */

#define HD_PORT_DATA		0x1f0
#define HD_PORT_ERROR		0x1f1
#define HD_PORT_SECT_COUNT	0x1f2
#define HD_PORT_SECT_NUM	0x1f3
#define HD_PORT_CYL_LOW		0x1f4
#define HD_PORT_CYL_HIGH	0x1f5
#define	HD_PORT_DRV_HEAD	0x1f6
#define HD_PORT_STATUS		0x1f7
#define HD_PORT_COMMAND		0x1f7

#define HD_READ		0x20
#define HD_WRITE	0x30
#define FST_FS	0x2e			/* normal partition */
#define FST_SW	0x2f			/* swap partition */

int lba_rw(unsigned int lba, unsigned int com, unsigned int sects_to_access, void *buf);
int lba_init();

#endif

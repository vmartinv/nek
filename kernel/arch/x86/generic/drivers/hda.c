
#include <arch/x86/hda.h>
//~ #include "usb/ehci.h"
//~ #include "usb/controller.h"
//~ #include "usb/dev.h"
//~ #include "console/console.h"
//~ #include "cpu/io.h"
//~ #include "mem/vm.h"
#include <arch/x86/pci/registry.h>
#include <arch/x86/pci/pci.h>
#include <sys/logging.h>
//~ #include "time/pit.h"
//~ #include "stdlib/link.h"
//~ #include "stdlib/string.h"


void HdaInit(unsigned id, PciDeviceInfo *info){
	
	if(!(
	(info->vendorId==0x8086 &&  (info->deviceId==0x2668  || info->deviceId==0x27D8)) ||
	(info->vendorId==0x1002 &&  info->deviceId==0x4383)  ))
		return;

	printk("hda", "hola\n");
}


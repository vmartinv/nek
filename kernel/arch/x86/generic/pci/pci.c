// ------------------------------------------------------------------------------------------------
// pci/pci.c
// ------------------------------------------------------------------------------------------------

#include <arch/x86/pci/pci.h>
#include <arch/x86/pci/driver.h>
#include <arch/x86/pci/registry.h>
#include <arch/x86/ports.h>
#include <sys/logging.h>

// ------------------------------------------------------------------------------------------------
static void PciVisit(unsigned bus, unsigned dev, unsigned func)
{
    unsigned id = PCI_MAKE_ID(bus, dev, func);

    PciDeviceInfo info;
    info.vendorId = PciRead16(id, PCI_CONFIG_VENDOR_ID);
    if (info.vendorId == 0xffff)
    {
        return;
    }

    info.deviceId = PciRead16(id, PCI_CONFIG_DEVICE_ID);
    info.progIntf = PciRead8(id, PCI_CONFIG_PROG_INTF);
    info.subclass = PciRead8(id, PCI_CONFIG_SUBCLASS);
    info.classCode = PciRead8(id, PCI_CONFIG_CLASS_CODE);

    printk("pci", "%02x:%02x:%d 0x%04x/0x%04x: %s\n",
        bus, dev, func,
        info.vendorId, info.deviceId,
        PciClassName(info.classCode, info.subclass, info.progIntf)
        );

    const PciDriver *driver = g_pciDriverTable;
    while (driver->init)
    {
        driver->init(id, &info);
        ++driver;
    }
}

// ------------------------------------------------------------------------------------------------
void PciInit()
{
    printk("pci", "PCI Initialization\n");
    for (unsigned bus = 0; bus < 256; ++bus)
    {
        for (unsigned dev = 0; dev < 32; ++dev)
        {
            unsigned baseId = PCI_MAKE_ID(bus, dev, 0);
            u8 headerType = PciRead8(baseId, PCI_CONFIG_HEADER_TYPE);
            unsigned funcCount = headerType & PCI_TYPE_MULTIFUNC ? 8 : 1;

            for (unsigned func = 0; func < funcCount; ++func)
            {
                PciVisit(bus, dev, func);
            }
        }
    }
}

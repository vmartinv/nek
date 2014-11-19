// ------------------------------------------------------------------------------------------------
// pci/driver.c
// ------------------------------------------------------------------------------------------------

#include <arch/x86/pci/driver.h>
#include <arch/x86/ports.h>
#include <arch/x86/hda.h>


// ------------------------------------------------------------------------------------------------
const PciDriver g_pciDriverTable[] =
{
	{HdaInit},
    { 0 },
};

// ------------------------------------------------------------------------------------------------
u8 PciRead8(unsigned id, unsigned reg)
{
    u32 addr = 0x80000000 | id | (reg & 0xfc);
    outl(PCI_CONFIG_ADDR, addr);
    return inb(PCI_CONFIG_DATA + (reg & 0x03));
}

// ------------------------------------------------------------------------------------------------
u16 PciRead16(unsigned id, unsigned reg)
{
    u32 addr = 0x80000000 | id | (reg & 0xfc);
    outl(PCI_CONFIG_ADDR, addr);
    return inw(PCI_CONFIG_DATA + (reg & 0x02));
}

// ------------------------------------------------------------------------------------------------
u32 PciRead32(unsigned id, unsigned reg)
{
    u32 addr = 0x80000000 | id | (reg & 0xfc);
    outl(PCI_CONFIG_ADDR, addr);
    return inl(PCI_CONFIG_DATA);
}

// ------------------------------------------------------------------------------------------------
void PciWrite8(unsigned id, unsigned reg, u8 data)
{
    u32 address = 0x80000000 | id | (reg & 0xfc);
    outl(PCI_CONFIG_ADDR, address);
    outb(PCI_CONFIG_DATA + (reg & 0x03), data);
}

// ------------------------------------------------------------------------------------------------
void PciWrite16(unsigned id, unsigned reg, u16 data)
{
    u32 address = 0x80000000 | id | (reg & 0xfc);
    outl(PCI_CONFIG_ADDR, address);
    outw(PCI_CONFIG_DATA + (reg & 0x02), data);
}

// ------------------------------------------------------------------------------------------------
void PciWrite32(unsigned id, unsigned reg, u32 data)
{
    u32 address = 0x80000000 | id | (reg & 0xfc);
    outl(PCI_CONFIG_ADDR, address);
    outl(PCI_CONFIG_DATA, data);
}

// ------------------------------------------------------------------------------------------------
static void PciReadBar(unsigned id, unsigned index, u32 *address, u32 *mask)
{
    unsigned reg = PCI_CONFIG_BAR0 + index * sizeof(u32);

    // Get address
    *address = PciRead32(id, reg);

    // Find out size of the bar
    PciWrite32(id, reg, 0xffffffff);
    *mask = PciRead32(id, reg);

    // Restore adddress
    PciWrite32(id, reg, *address);
}

// ------------------------------------------------------------------------------------------------
void PciGetBar(PciBar *bar, unsigned id, unsigned index)
{
    // Read pci bar register
    u32 addressLow;
    u32 maskLow;
    PciReadBar(id, index, &addressLow, &maskLow);

    if (addressLow & PCI_BAR_64)
    {
        // 64-bit mmio
        u32 addressHigh;
        u32 maskHigh;
        PciReadBar(id, index + 1, &addressHigh, &maskHigh);
        bar->u.address = (void *)(((uintptr_t)((u64)addressHigh << 32)) | (addressLow & ~0xf));
        bar->size = ~(((u64)maskHigh << 32) | (maskLow & ~0xf)) + 1;
        bar->flags = addressLow & 0xf;
    }
    else if (addressLow & PCI_BAR_IO)
    {
        // i/o register
        bar->u.port = (u16)(addressLow & ~0x3);
        bar->size = (u16)(~(maskLow & ~0x3) + 1);
        bar->flags = addressLow & 0x3;
    }
    else
    {
        // 32-bit mmio
        bar->u.address = (void *)(uintptr_t)(addressLow & ~0xf);
        bar->size = ~(maskLow & ~0xf) + 1;
        bar->flags = addressLow & 0xf;
    }
}

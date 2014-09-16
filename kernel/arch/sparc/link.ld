OUTPUT_FORMAT(binary)
ENTRY(_start)
 
SECTIONS
{
    . = 0x3980;
    .main : 
    {
        LONG(0x01030107);
		LONG(_boot_end - _boot_start);
		LONG(0);
		LONG(0);
		LONG(0);
		LONG(0);
		LONG(0);
		LONG(0);
		_boot_start = .;
		*(.text*)
		*(.rodata*)
		*(.data*)
		*(.bss*)
		_boot_end = .;
    }
}
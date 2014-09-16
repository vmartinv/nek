ENTRY (_start)

SECTIONS
{
    . = 0x10000;
    _kernel_start = .;
    .text : { *(.text*) *(.rodata*) }
    .data : { *(.data*) }
    .bss : { *(.bss*) *(COMMON*) }
    _kernel_end = .;
}
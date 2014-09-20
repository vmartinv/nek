Building
================
You should use make and provide the cross compiler:
> PATH=/path/to/crosscompiler/bin:$PATH make kernel

If there are not errors, kernel.elf should be in the bin folder.

Installing
================
You may copy it to /boot/ and add an entry at /etc/grub.d/40_custom like:
> menuentry "nesos2" {
>
>	multiboot /boot/kernel.elf
>
>}

Remember to update the config file:
> sudo grub-mkconfig -o /boot/grub/grub.cfg

Dependencies
================
- i686 cross compiler (both C and ASM), like GCC and nasm
- GRUB2(optional): Generates .iso (cd image)
- QEMU(optional): Emulation
- Doxygen and Graphviz (optional): For generating the documentation

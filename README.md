Introduction
================
Nintendo Entertainment Kernel is a very basic kernel with only the minimun
to run a NES emulator. It does not have pagging nor userspace nor any kind of protection at all.
Only one program can be running at a time. Only on program can be run.
It is embedded with the kernel, which means the program's code is compiled with the kernel.

Screenshots
================
![alt Home Screen] (https://raw.githubusercontent.com/mvpossum/nek/master/examples/homescreen.png)

![alt Mario Bros](https://raw.githubusercontent.com/mvpossum/nek/master/examples/samplegame.png)

Building
================
You should use make and provide the cross compiler:
> PATH=/path/to/crosscompiler/bin:$PATH make kernel program=emulador

If there are not errors, kernel.elf should be in the bin folder.

Examples
================
Besides "emulador" there are two other working programs.
mandelbrot (shows a explorable fractal)
paleta (shows nes avalaible colors)
corazon (a heart animation using math functions)
All of them can be compiled with the kernel using make.
Ej: make program=examples/mandelbrot

Installing
================
You may copy kernel.elf to /boot/ and add an entry at /etc/grub.d/40_custom like:
> menuentry "nek" {
>
>	multiboot /boot/kernel.elf
>	module /boot/initrd.img
>
>}

Remember to update the config file:
> sudo grub-mkconfig -o /boot/grub/grub.cfg

Testing
================
The makefile should run QEMU using the disk image disk.img. A loop device to this file must have been set up in the mount folder.


Dependencies
================
- i686 cross compiler (both C and ASM), like GCC and nasm
- GRUB2(optional): Generates .iso (cd image)
- QEMU(optional): Emulation
- Doxygen and Graphviz (optional): For generating the documentation
- grub-mkrescue (optional): For generating a booteable cd image (.iso)


Boot process
================
GRUB (multiboot standard)

kernel/arch/x86/init/boot.s	<-- calls c code, initializes stack

kernel_entry() at kernel/arch/x86/init/kernel_entry.c <--base system initilization

kmain() at kernel/kmain.c	<--- some services are started

main() at program/main.(c|cpp)	<--- main program



Folder Structure
================
- kernel folder has all related to the kernel
- kernel/arch code that depends on the architecture
- kernel/fs file system(currently only vfs implemented)
- fdlibm: a math library downloaded from internet
- util/specs: Some useful documents
- informe: Informe borrador
- iso: a few files to generate an .iso with the kernel
- util: contains the program to generate the initrd and a few other things
- util/fuente: Tools/file used to generate the console font

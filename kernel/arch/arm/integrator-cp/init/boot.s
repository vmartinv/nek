interrupt_vector_table:
    b . @ Reset
    b . 
    b . @ SWI instruction
    b . 
    b .
    b .
    b .
    b .

.comm stack, 0x1000              @ Reserve 4k stack in the BSS

_start:
    .globl _start
    ldr sp, =stack+0x1000        @ Set up the stack

    bl kernel_entry    @ Jump to the main function

1: 
    b 1b

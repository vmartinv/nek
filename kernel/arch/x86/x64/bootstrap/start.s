;Multiboot
MBALIGN     equ  1<<0
MEMINFO     equ  1<<1
FLAGS       equ  MBALIGN | MEMINFO
MAGIC       equ  0x1BADB002
CHECKSUM    equ -(MAGIC + FLAGS)

section .multiboot
align 4
	dd MAGIC
	dd FLAGS
	dd CHECKSUM

section .bootstrap_stack
align 4
	stack_bottom:
	times 4096 db 0
	stack_top:

; Kernel entry
section .text
[GLOBAL _start]
[EXTERN bootstrap_init]

_start:
	;Setup stack
	mov esp, stack_top
	mov ebp, 0
	;Push multiboot params
	push ebx
	push eax
	call bootstrap_init
	cli
.x86_halt:
	hlt
	jmp .x86_halt
global read_eip
read_eip:
	pop eax
	jmp eax
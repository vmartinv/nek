;Multiboot
MBALIGN     equ  1<<0
MEMINFO     equ  1<<1
VIDINFO     equ  1<<2
%ifndef CONSOLE_TEXTMODE
FLAGS       equ  MBALIGN | MEMINFO | VIDINFO
%else
FLAGS       equ  MBALIGN | MEMINFO
%endif
MAGIC       equ  0x1BADB002
CHECKSUM    equ -(MAGIC + FLAGS)

section .multiboot
align 4
	dd MAGIC
	dd FLAGS
	dd CHECKSUM
%ifndef CONSOLE_ONLY
	dd 0,0,0,0,0
	dd 0						; Sets the linear graphics mode
	dd 320,200,24 				; Width, height, depth
%endif

section .bootstrap_stack
align 4
	stack_bottom:
	times 16384 db 0
	stack_top:

; Kernel entry
section .text
[GLOBAL start]
[EXTERN kernel_entry]

start:
	;Setup stack
	mov esp, stack_top
	mov ebp, 0
	;Push multiboot params
	push ebx
	push eax
	call kernel_entry
	cli
.x86_halt:
	hlt
	jmp .x86_halt
global read_eip
read_eip:
	pop eax
	jmp eax

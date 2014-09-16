global x86_switch_to_usermode_asm
x86_switch_to_usermode_asm:
	cli
	mov ax, 0x23
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov eax, esp
	push 0x23
	push eax
	pushf
	pop eax
	or eax, 0x200
	push eax
	push 0x1B
	push after
	iret
after:
	jmp after
	ret
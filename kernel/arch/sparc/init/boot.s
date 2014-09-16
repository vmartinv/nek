	.file	"boot.s"
	.data
	.global	cputyp, nbpg, pgofset, pgshift
cputyp:
	.word	1
nbpg:
	.word	1
pgofset:
	.word	1
pgshift:
	.word	1
prom_address:
	.long 1

	.section bss
	
	.text
	.global _start
	.extern kernel_entry
_start:
	call kernel_entry
		nop

halt:
	call halt
		nop

!End of file

#ifndef ELF_H
#define ELF_H
#include <stdint.h>
#define ELF32_ST_TYPE(i) ((i)&0xf)

typedef uint32_t   Elf32_Addr;
typedef uint16_t   Elf32_Half;
typedef uint32_t   Elf32_Off;
typedef int32_t    Elf32_Sword;
typedef uint32_t   Elf32_Word;
typedef uint64_t   Elf64_Addr;
typedef uint16_t   Elf64_Half;
typedef int16_t    Elf64_SHalf;
typedef uint64_t   Elf64_Off;
typedef int32_t    Elf64_Sword;
typedef uint32_t   Elf64_Word;
typedef uint64_t   Elf64_Xword;
typedef int64_t    Elf64_Sxword;
#define EI_NIDENT       16

typedef struct elf32_hdr{
	unsigned char e_ident[EI_NIDENT];
	Elf32_Half    e_type;
	Elf32_Half    e_machine;
	Elf32_Word    e_version;
	Elf32_Addr    e_entry;  /* Entry point */
	Elf32_Off     e_phoff;
	Elf32_Off     e_shoff;
	Elf32_Word    e_flags;
	Elf32_Half    e_ehsize;
	Elf32_Half    e_phentsize;
	Elf32_Half    e_phnum;
	Elf32_Half    e_shentsize;
	Elf32_Half    e_shnum;
	Elf32_Half    e_shstrndx;
} Elf32_Ehdr;
typedef struct elf64_hdr {
	unsigned char e_ident[EI_NIDENT];     /* ELF "magic number" */
	Elf64_Half e_type;
	Elf64_Half e_machine;
	Elf64_Word e_version;
	Elf64_Addr e_entry;           /* Entry point virtual address */
	Elf64_Off e_phoff;            /* Program header table file offset */
	Elf64_Off e_shoff;            /* Section header table file offset */
	Elf64_Word e_flags;
	Elf64_Half e_ehsize;
	Elf64_Half e_phentsize;
	Elf64_Half e_phnum;
	Elf64_Half e_shentsize;
	Elf64_Half e_shnum;
	Elf64_Half e_shstrndx;
} Elf64_Ehdr;
#endif
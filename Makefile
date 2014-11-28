# Add toolchain to path
export PATH := ../cross/bin/:$(PATH)

#Configuration
#--------------------------------------------
ARCH := x86
BOARD := generic

BOARD_ID := ${ARCH}/${BOARD}
#Tools
#--------------------------------------------
CC:=i686-elf-gcc
CCPLUS:=i686-elf-g++
AS:=nasm -felf
LD:=i686-elf-ld -m elf_i386
OBJDUMP:=i686-elf-objdump
OBJCOPY:=i686-elf-objcopy

BUILD_NUMBER_FILE=build.num
BUILD_NUMBER_LDFLAGS += -Xlinker --defsym -Xlinker KERN_BNUM=$$(cat $(BUILD_NUMBER_FILE))

# Freestanding binary, strip extra shits
#CFLAGS= -std=gnu99  -ffreestanding  -finline-functions -Wno-unused-parameter -Wall -Wextra -I./ -I./kernel/includes -march=i686  -D_IEEE_MODE
CPPFLAGS := -ffreestanding  -fno-exceptions -fno-rtti  -nostartfiles -D_IEEE_MODE -pedantic -Ofast -std=c++11
CFLAGS := -ffreestanding  -std=gnu99 -nostartfiles -D_IEEE_MODE
CFLAGS   += -Wall -Wextra -Wno-unused-function -Wno-unused-parameter
CPPFLAGS += -Wall -Wextra -Wno-unused-function -Wno-unused-parameter
LDFLAGS= -ffreestanding -nostdlib -lgcc $(BUILD_NUMBER_LDFLAGS)
ASFLAGS =

debug ?= 0
ifeq ($(debug), 1)
    CFLAGS += -DDEBUG -g -fvar-tracking -fvar-tracking-assignments  -gdwarf-2
    CPPFLAGS += -DDEBUG -g -Og -fvar-tracking -fvar-tracking-assignments  -gdwarf-2
    LDFLAGS += -g
else
	CFLAGS += -O3
	CPPFLAGS += -O3
endif

consoleonly ?= 0
ifeq ($(consoleonly), 1)
	CFLAGS += -DCONSOLE_ONLY
	ASFLAGS += -DCONSOLE_ONLY
endif

program ?= kbd_test
LD_SCRIPT := kernel/arch/${ARCH}/${BOARD}/link.ld
INCLUDE_DIR := -I"kernel/includes" -I"${program}/includes"

GENISO := xorriso -as mkisofs
EMU := qemu-system-i386

#FILES
#--------------------------------------------
FDLIBM_FILES := $(patsubst %.c,%.o,$(wildcard fdlibm/*.c))

MAIN_FILES := $(patsubst %.c,%.o,$(wildcard ${program}/*/*.c)) $(patsubst %.c,%.o,$(wildcard ${program}/*.c)) $(patsubst %.cpp,%.o,$(wildcard ${program}/*/*.cpp)) $(patsubst %.cpp,%.o,$(wildcard ${program}/*.cpp))


GLOBAL_ROOT_FILES := $(patsubst %.c,%.o,$(wildcard kernel/*.c))
GLOBAL_SYS_FILES := $(patsubst %.c,%.o,$(wildcard kernel/sys/*.c))
GLOBAL_INIT_FILES := $(patsubst %.c,%.o,$(wildcard kernel/init/*.c))
GLOBAL_DRIVERS_FILES := $(patsubst %.c,%.o,$(wildcard kernel/drivers/*.c))
GLOBAL_LOW_FILES := $(patsubst %.c,%.o,$(wildcard kernel/low/*.c))
GLOBAL_FS_FILES := $(patsubst %.c,%.o,$(wildcard kernel/fs/*.c))
GLOBAL_LIB_FILES := $(patsubst %.c,%.o,$(wildcard kernel/lib/*.c)) $(patsubst %.cpp,%.o,$(wildcard kernel/lib/*.cpp))
GLOBAL_GRAPHICS_FILES := $(patsubst %.c,%.o,$(wildcard kernel/graphics/*.c))

ARCH_ROOT_FILES := $(patsubst %.s,%.o,$(wildcard kernel/arch/${ARCH}/*.s)) $(patsubst %.c,%.o,$(wildcard kernel/arch/${ARCH}/*.c))
ARCH_DRIVERS_FILES := $(patsubst %.c,%.o,$(wildcard kernel/arch/${ARCH}/drivers/*.c))
ARCH_LOW_FILES := $(patsubst %.s,%.o,$(wildcard kernel/arch/${ARCH}/low/*.s))
ARCH_FS_FILES := $(patsubst %.c,%.o,$(wildcard kernel/arch/${ARCH}/fs/*.c))
ARCH_LIB_FILES := $(patsubst %.c,%.o,$(wildcard kernel/arch/${ARCH}/lib/*.c))

BOARD_ROOT_FILES := $(patsubst %.s,%.o,$(wildcard kernel/arch/${ARCH}/${BOARD}/*.s)) $(patsubst %.c,%.o,$(wildcard kernel/arch/${ARCH}/${BOARD}/*.c))
BOARD_INIT_FILES := $(patsubst %.c,%.o,$(wildcard kernel/arch/${ARCH}/${BOARD}/init/*.c)) $(patsubst %.s,%.o,$(wildcard kernel/arch/${ARCH}/${BOARD}/init/*.s))
BOARD_PCI_FILES := $(patsubst %.c,%.o,$(wildcard kernel/arch/${ARCH}/${BOARD}/pci/*.c))
BOARD_DRIVER_FILES := $(patsubst %.c,%.o,$(wildcard kernel/arch/${ARCH}/${BOARD}/drivers/*.c))
BOARD_LOW_FILES := $(patsubst %.s,%.o,$(wildcard kernel/arch/${ARCH}/${BOARD}/low/*.s))
BOARD_FS_FILES := $(patsubst %.c,%.o,$(wildcard kernel/arch/${ARCH}/${BOARD}/fs/*.c))
BOARD_LIB_FILES := $(patsubst %.c,%.o,$(wildcard kernel/arch/${ARCH}/${BOARD}/lib/*.c))

BOARD_BOOTSTRP_FILES := $(patsubst %.c,%.o,$(wildcard kernel/arch/${ARCH}/${BOARD}/bootstrap/*.c))

#Canonicate them together
GLOBAL_FILES :=  ${FDLIBM_FILES} ${GLOBAL_ROOT_FILES} ${GLOBAL_SYS_FILES} ${GLOBAL_INIT_FILES} ${GLOBAL_DRIVERS_FILES} ${GLOBAL_LOW_FILES} ${GLOBAL_FS_FILES} ${GLOBAL_LIB_FILES} ${GLOBAL_TEST_FILES} ${GLOBAL_GRAPHICS_FILES} ${MAIN_FILES}
ARCH_FILES := ${ARCH_ROOT_FILES} ${ARCH_DRIVERS_FILES} ${ARCH_LOW_FILES} ${ARCH_FS_FILES} ${ARCH_LIB_FILES} ${ARCH_TEST_FILES} 
BOARD_FILES :=${BOARD_ROOT_FILES} ${BOARD_INIT_FILES} ${BOARD_DRIVER_FILES} ${BOARD_LOW_FILES} ${BOARD_FS_FILES} ${BOARD_LIB_FILES} ${BOARD_TEST_FILES} ${BOARD_PCI_FILES}

ALL_SOURCE_FILES := ${ARCH_FILES} ${BOARD_FILES} ${GLOBAL_FILES}

#RULES
#--------------------------------------------

all: run

bin-dir:
	@echo "DIR    | bin"
	@mkdir -p bin
	
fdlibm: ${FDLIBM_FILES}

arch: ${ARCH_FILES}

board: ${BOARD_FILES}

kernel: bin-dir ${ALL_SOURCE_FILES} fdlibm arch board
	@echo " LD [K]| kernel.elf"
	@${LD} ${LFLAGS} -T ${LD_SCRIPT} -o bin/kernel.elf $(ALL_SOURCE_FILES)
ifeq ($(debug), 1)
	@echo "GEN [M]| build/kernel.elf -> bin/kernel.sym"
	@$(OBJDUMP) -b elf32-i386 -S -d bin/kernel.elf > bin/symbols.txt
	@$(OBJCOPY) -I elf32-i386 --only-keep-debug bin/kernel.elf bin/kernel.sym
	@$(OBJCOPY) -I elf32-i386 --strip-debug bin/kernel.elf
endif

iso: kernel
	@echo "ISO [A]| bin/cd.iso"
	@cp bin/kernel.elf iso/boot/nek.img
	@grub-mkrescue iso -o bin/cd.iso

mount:
	@echo "DIR    | mount"
	@mkdir -p mount

installtodisk: kernel
	@(mount | grep mount > /dev/null) || (sudo losetup -f -o 32256 --sizelimit 33521664 disk.img && sudo mount /dev/loop0 mount)
	@cp bin/kernel.elf mount/nek.img
	@sync

run: installtodisk
ifeq ($(debug), 1)
	@${EMU} -s -S -m 25 -serial stdio -soundhw hda -hda disk.img
else
	@${EMU} -m 25 -serial stdio -soundhw hda -hda disk.img
endif

gdb:
	@gdb --eval-command='target remote :1234' --symbols=bin/kernel.sym


%.o: %.c .compiler_flags
	@echo " CC    |" $@
	@${CC} -c ${CFLAGS} ${COMPILE_OPTIONS} ${INCLUDE_DIR} -DBOARD${ARCH}${BOARD} -DARCH${ARCH} -o $@ $<

%.o: %.cpp .compiler_flags
	@echo " CCPLUS|" $@
	@${CCPLUS} -c ${CPPFLAGS} ${COMPILE_OPTIONS} ${INCLUDE_DIR} -DBOARD${ARCH}${BOARD} -DARCH${ARCH} -o $@ $<

%.o: %.s .compiler_flags
	@echo " AS    |" $@
	@${AS} ${ASFLAGS} -o $@ $<

fdlibm/%.o: fdlibm/%.c
	@echo " CC    |" $@
	@${CC} -c ${CFLAGS} ${COMPILE_OPTIONS} -Wno-strict-aliasing -Wno-maybe-uninitialized ${INCLUDE_DIR} -o $@ $<

kernel/lib/liballoc.o: kernel/lib/liballoc.c .compiler_flags
	@echo " CC    |" $@
	@${CC} -c ${CFLAGS} ${COMPILE_OPTIONS} ${INCLUDE_DIR} -DBOARD${ARCH}${BOARD} -DARCH${ARCH} -w -o $@ $<
	

.PHONY: clean force

.compiler_flags: force
	@echo '$(CFLAGS) $(ASFLAGS)' | cmp -s - $@ || echo '$(CFLAGS) $(ASFLAGS)' > $@

install: kernel
	@cp bin/kernel.elf /boot/nek.img
	@grub-mkconfig -o /boot/grub/grub.cfg

clean:
	@echo "CLN    | *.o" 
	@-find . -name "*.o" -type f -delete
	@echo "CLN    | *.elf" 
	@-find . -name "*.elf" -type f -delete
	@echo "CLN    | bin" 
	@rm -f bin/*
	@rmdir bin
	@echo "CLN    | compiler_flags" 
	@rm -f compiler_flags
	@rm -f iso/boot/nek.img

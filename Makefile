export TARGET=i686-elf
export SYSROOT=$(shell readlink -f sysroot)
export PREFIX=/usr
export INCLUDE_DIR=$(PREFIX)/include

export CC=$(TARGET)-gcc
export LINKER=$(TARGET)-gcc
export AR=$(TARGET)-ar
export CFLAGS=-ffreestanding -g -O0 -Wall -Wextra -std=gnu11 --sysroot=$(SYSROOT) -isystem=$(INCLUDE_DIR)
export LFLAGS=-ffreestanding -g -O0 -nostdlib --sysroot=$(SYSROOT)

OBJS=\
	kernel/i686/boot.o\
	kernel/i686/tty.o\
	kernel/i686/crti.o\
	kernel/i686/crtn.o\
	kernel/main.o\
	kernel/keyboard.o\

.PHONY: iso sysroot stdlib clean qemu debug
.default: iso

iso: os.kernel
	mkdir -p isodir/boot/grub
	cp grub.cfg isodir/boot/grub
	cp $(SYSROOT)/os.kernel isodir/boot/os.kernel
	grub-mkrescue -o os.iso isodir

os.kernel: sysroot stdlib
	make -Ckernel os.kernel

sysroot:
	mkdir -p $(SYSROOT)$(INCLUDE_DIR)
	make -Ckernel sysroot
	make -Clibc sysroot

stdlib: sysroot
	mkdir -p $(SYSROOT)$(PREFIX)/lib
	make -Clibc libc.a
	make -Clibc libk.a

clean:
	rm -rf $(SYSROOT) isodir os.iso
	make -Ckernel clean
	make -Clibc clean

qemu:
	qemu-system-i386 -cdrom os.iso

debug:
	@echo "Connect with (gdb)target remote localhost:1234 and then (gdb)continue"
	qemu-system-i386 -s -cdrom os.iso

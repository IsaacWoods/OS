TARGET=i686-elf
CC=$(TARGET)-gcc
LINKER=$(TARGET)-gcc
CFLAGS=-ffreestanding -O2 -Wall -Wextra -std=gnu11

OBJS=\
	src/boot.o\
	src/main.o\

.PHONY: iso clean qemu
.default: iso

iso: os.kernel
	mkdir -p isodir/boot/grub
	cp grub.cfg isodir/boot/grub
	cp os.kernel isodir/boot/os.kernel
	grub-mkrescue -o os.iso isodir

os.kernel: $(OBJS)
	$(LINKER) -T linker.ld -o $@ -ffreestanding -O2 -nostdlib $^ -lgcc

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)

%.o: %.s
	nasm -felf32 -o $@ $<

clean:
	rm -f os.kernel $(OBJS)

qemu:
	qemu-system-i386 -cdrom os.iso

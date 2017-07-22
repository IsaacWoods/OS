os.iso: stage1.s
	nasm -f bin -o stage1.o $<
	dd if=/dev/zero of=floppy.img bs=1024 count=1440
	dd if=stage1.o of=floppy.img seek=0 count=1 conv=notrunc
	mkdir -p iso
	cp floppy.img iso/
	genisoimage -quiet -V 'OS' -input-charset iso8859-1 -o $@ -b floppy.img -hide floppy.img iso/

run: os.iso
	qemu-system-i386 -cdrom $<

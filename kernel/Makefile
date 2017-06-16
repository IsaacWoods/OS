OBJS=\
	i686/boot.o\
	i686/tty.o\
	i686/platform.o\
	i686/stuff.o\
	i686/crti.o\
	i686/crtn.o\
	i686/crtbegin.o\
	i686/crtend.o\
	main.o\
	keyboard.o\

.PHONY: clean sysroot

sysroot:
	mkdir -p $(SYSROOT)$(INCLUDE_DIR)/kernel
	cp -r --preserve=timestamps include/* $(SYSROOT)$(INCLUDE_DIR)/kernel

os.kernel: $(OBJS) sysroot
	$(LINKER) -T linker.ld -o $@ $(OBJS) $(LFLAGS) -lk -lgcc
	grub-file --is-x86-multiboot $@
	cp --preserve=timestamps os.kernel $(SYSROOT)/os.kernel

i686/crtbegin.o i686/crtend.o:
	OBJ=`$(CC) $(CFLAGS) $(LFLAGS) -print-file-name=$(@F)` && cp "$$OBJ" $@

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)

%.o: %.s
	nasm -felf32 -o $@ $<

%.o: %.gas
	$(TARGET)-as $< -o $@

clean:
	rm -f os.kernel $(OBJS)

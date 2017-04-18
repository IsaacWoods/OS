OBJS=\
	string.o\
	stdio.o\
	stdlib.o\

LIBK_OBJS=$(OBJS:.o=.libk.o)

.PHONY: clean sysroot

sysroot:
	cp -r --preserve=timestamps include/* $(SYSROOT)$(INCLUDE_DIR)

libc.a: $(OBJS)
	$(AR) rcs $@ $(OBJS)
	cp $@ $(SYSROOT)$(PREFIX)/lib

libk.a: $(LIBK_OBJS)
	$(AR) rcs $@ $(LIBK_OBJS)
	cp $@ $(SYSROOT)$(PREFIX)/lib

%.o: %.c
	$(CC) -c $< -o $@ -std=gnu11 $(CFLAGS)

%.o: %.S
	$(CC) -c $< -o $@ $(CFLAGS)

%.libk.o: %.c
	$(CC) -c $< -o $@ -std=gnu11 $(CFLAGS) -D__is_libk

%.libk.o: %.s
	$(CC) -c $< -o $@ $(CFLAGS) -D__is_libk

clean:
	rm -f *.a
	rm -f $(OBJS) $(LIBK_OBJS)

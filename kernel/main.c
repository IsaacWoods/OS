/*
 * Copyright (C) 2017, Isaac Woods. All rights reserved.
 */

#include <kernel/multiboot.h>
#include <kernel/tty.h>
#include <kernel/platform.h>
#include <stdio.h>

void kmain(uint32_t magic, struct multiboot_info* bootInfo)
{
  InitializeTerminal();

  if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
  {
    KERNEL_PANIC("Incorrect bootloader magic: %x\n", magic);
  }

  printf("Hello, World!\n");
  printf("Ohh fancy: %u\n", 42);
  printf("Hexxxx: %x\n", 0xBADBEEF);
  printf("Strang: %s\n", "My name is Isaac");
}

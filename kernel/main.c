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
  InitPlatform();

  if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
  {
    KERNEL_PANIC("Incorrect bootloader magic: %x\n", magic);
  }

  printf("Hello, World!\n");
//  asm volatile("int $0x3");
}

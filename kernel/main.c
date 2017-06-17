/*
 * Copyright (C) 2017, Isaac Woods. All rights reserved.
 */

#include <kernel/multiboot.h>
#include <kernel/tty.h>
#include <kernel/platform.h>
#include <kernel/keyboard.h>
#include <stdio.h>
#include <stdbool.h>

__attribute__((noreturn))
void kmain(uint32_t magic, struct multiboot_info* bootInfo)
{
  InitializeTerminal();

  if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
  {
    KERNEL_PANIC("Incorrect bootloader magic: %x\n", magic);
  }

  printf("Hello, World!\n");
  printf("Mem lower: %u\nMem upper: %u\n", bootInfo->mem_lower, bootInfo->mem_upper);
  SetTimerFrequency(1000u);
  InitPlatform();
  InitKeyEventBuffer();

  while (true)
  {
    struct KeyEvent event;

    if (PopKeyEvent(&event) && event.state == KEY_PRESSED)
    {
      printf("%c", event.c);
    }
  }

  /*
   * XXX: We shouldn't return from kmain, or interrupts will be disabled and we'll hang
   */
}

/*
 * Copyright (C) 2017, Isaac Woods. All rights reserved.
 */

#include <kernel/multiboot.h>
#include <kernel/tty.h>
#include <kernel/platform.h>
#include <kernel/keyboard.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

__attribute__((noreturn))
void kmain(uint32_t magic, struct multiboot_info* bootInfo)
{
  InitializeTerminal();
  InitKeyEventBuffer();

  if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
  {
    KERNEL_PANIC("Incorrect bootloader magic: %x\n", magic);
  }

  printf("Max uint32_t: (%u) (%x)\n", UINT32_MAX, UINT32_MAX);

  printf("Hello, World!\n");
  printf("Mem lower: %x\nMem upper: %x\n", bootInfo->mem_lower, bootInfo->mem_upper);
  printf("End of kernel: %x\n", g_endKernel);
  SetTimerFrequency(1000u);
  InitPlatform();

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

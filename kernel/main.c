/*
 * Copyright (C) 2017, Isaac Woods. All rights reserved.
 */

#include <kernel/tty.h>
#include <stdio.h>

void kmain(void)
{
  InitializeTerminal();
  printf("Hello, World!\n");
  printf("Ohh fancy: %u\n", 42);
  printf("Hexxxx: %x\n", 0xBADBEEF);
}

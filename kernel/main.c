/*
 * Copyright (C) 2017, Isaac Woods. All rights reserved.
 */

#include <kernel/tty.h>
#include <stdio.h>

void kmain(void)
{
  InitializeTerminal();
  printf("Hello, World!\n");
  printf("Second line\n");
}

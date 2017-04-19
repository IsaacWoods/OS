/*
 * Copyright (C) 2017, Isaac Woods. All rights reserved.
 */

#include <kernel/platform.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

__attribute__((__noreturn__))
void Internal_KernelPanic(const char* file, int line, const char* message, ...)
{
  va_list args;
  va_start(args, message);

  printf("PANIC(%s:%d): ", file, line);
  vprintf(message, args);

  va_end(args);
  abort();
}

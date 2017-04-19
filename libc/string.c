/*
 * Copyright (C) 2017, Isaac Woods. All rights reserved.
 */

#include <string.h>

size_t strlen(const char* str)
{
  size_t length = 0u;
  while (str[length])
  {
    length++;
  }
  return length;
}

size_t strcpy(char* dest, const char* src)
{
  size_t i = 0u;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsequence-point"
  while ((dest[i] = src[i++]));
#pragma GCC diagnostic pop
  return i;
}

/*
 * Copyright (C) 2017, Isaac Woods. All rights reserved.
 */

#include <stdlib.h>
#include <stdio.h>

char* itoa(int value, char* buffer, int base)
{
  unsigned long int temp = value;
  int i = 0;

  do
  {
    temp = value % base;
    buffer[i++] = (temp<10) ? ('0'+temp) : ('a'-10+temp);
  } while (value /= base);

  buffer[i--] = '\0';

  for (int j = 0;
       j < i;
       j++, i--)
  {
    temp = buffer[j];
    buffer[j] = buffer[i];
    buffer[i] = temp;
  }

  return buffer;
}

char* itoaLong(uint64_t value, char* buffer, int base)
{
  uint64_t temp = value;
  uint64_t i = 0u;

  do
  {
    temp = value % base;
    buffer[i++] = (temp<10u) ? ('0'+temp) : ('a'-10u+temp);
  } while (value /= base);

  buffer[i--] = '\0';

  for (uint64_t j = 0u;
       j < i;
       j++, i--)
  {
    temp = buffer[j];
    buffer[j] = buffer[i];
    buffer[i] = temp;
  }

  return buffer;
}

int memcmp(const void* aPtr, const void* bPtr, size_t length)
{
  const unsigned char* a = (const unsigned char*)aPtr;
  const unsigned char* b = (const unsigned char*)bPtr;
  
  for (size_t i = 0u;
       i < length;
       i++)
  {
    if (a[i] < b[i])
    {
      return -1;
    }
    else if (a[i] > b[i])
    {
      return 1;
    }
  }

  return 0;
}

void* memcpy(void* __restrict destPtr, const void* __restrict srcPtr, size_t length)
{
  unsigned char* dest = (unsigned char*)destPtr;
  const unsigned char* src = (const unsigned char*)srcPtr;

  for (size_t i = 0u;
       i < length;
       i++)
  {
    dest[i] = src[i];
  }

  return destPtr;
}

void* memmove(void* destPtr, const void* srcPtr, size_t length)
{
  unsigned char* dest = (unsigned char*)destPtr;
  const unsigned char* src = (const unsigned char*)srcPtr;

  if (dest < src)
  {
    for (size_t i = 0u;
         i < length;
         i++)
    {
      dest[i] = src[i];
    }
  }
  else
  {
    for (size_t i = length;
         i != 0u;
         i--)
    {
      dest[i-1u] = src[i-1u];
    }
  }

  return destPtr;
}

void* memset(void* p, unsigned char value, size_t length)
{
  unsigned char* buffer = (unsigned char*)p;

  for (size_t i = 0u;
       i < length;
       i++)
  {
    buffer[i] = value;
  }

  return p;
}

__attribute__((__noreturn__))
void abort(void)
{
#if defined(__is_libk)
  // TODO(Isaac): properly kernel panic
  printf("Kernel Panic: abort()\n");
#else
  // TODO(Isaac): abnormally terminate process like SIGABRT
  printf("abort()\n");
#endif
  while(1) { }
  __builtin_unreachable();
}

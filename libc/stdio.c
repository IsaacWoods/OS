/*
 * Copyright (C) 2017, Isaac Woods. All rights reserved.
 */

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#if defined(__is_libk)
  #include <kernel/tty.h>
#endif

int puts(const char* str)
{
  return printf("%s\n", str);
}

char putchar(char c)
{
#if defined(__is_libk)
  WriteToTTY(&c, sizeof(c));
#else
  // TODO(Isaac): call the write systemcall
#endif

  return c;
}

static void PrintDec(unsigned int value, unsigned int width, char* buffer, int* ptr)
{
  unsigned int nWidth = 1u;
  unsigned int i = 9u;

  while (value > i && i < UINT32_MAX)
  {
    nWidth++;
    i *= 10u;
    i += 9u;
  }

  int printed = 0;
  while (nWidth + printed < width)
  {
    buffer[*ptr] = '0';
    (*ptr)++;
    printed++;
  }

  i = nWidth;
  while (i > 0u)
  {
    unsigned int n = value / 10u;
    int r = value % 10u;
    buffer[*ptr+i-1u] = '0'+r;
    i--;
    value = n;
  }

  *ptr += nWidth;
}

static void PrintHex(unsigned int value, unsigned int width, char* buffer, int* ptr)
{
  int i = width;
  if (i == 0) i = 8;

  unsigned int nWidth = 1u;
  unsigned int j = 0x0F;
  while (value > j && j < UINT32_MAX)
  {
    nWidth++;
    j *= 0x10;
    j += 0x0F;
  }

  while (i > (int)nWidth)
  {
    buffer[*ptr] = '0';
    (*ptr)++;
    i--;
  }

  i = (int)nWidth;
  while (i-- > 0)
  {
    buffer[*ptr] = "0123456789abcdef"[(value>>(i*4))&0xF];
    (*ptr)++;
  }
}

int vprintf(const char* __restrict fmt, va_list args)
{
  char buffer[1024u] = {};
  size_t bufferLength = 0u;
  bool confused = false;

  #define EMIT(str) \
    strcpy(&(buffer[bufferLength]), str); \
    bufferLength += strlen(str);

  while (*fmt != '\0')
  {
    if (*fmt != '%')
    {
      printCharacter:
        buffer[bufferLength++] = *fmt++;
        continue;
    }

    const char* backupFmt = fmt;

    if (*(++fmt) == '%')
    {
      goto printCharacter;
    }

    if (confused)
    {
      gotConfused:
        confused = true;
        fmt = backupFmt;
        goto printCharacter;
    }

    // NOTE(Isaac): hopefully this gets turned into a lookup-and-jump table
    switch (*fmt)
    {
      case 'c':
      {
        fmt++;
        buffer[bufferLength++] = (char)(va_arg(args, int));  // NOTE(Isaac): `char` is promoted to `int`
      } break;

      case 's':
      {
        fmt++;
        const char* str = va_arg(args, const char*);
        EMIT(str);
      } break;

      case 'u':
      case 'd':
      {
        fmt++;
        unsigned long value = va_arg(args, unsigned long);
        PrintDec(value, 0, buffer, &bufferLength);
      } break;
#if 0
      case 'f':
      {
        fmt++;
        double value = va_arg(args, double);

        /*
         * NOTE(Isaac): I couldn't work out the longest value that could be contained in a IEEE-754
         * double-precision number, so I gave up and hoped this won't overflow
         */
        char valueBuffer[16u];
        EMIT(gcvt(value, 8, valueBuffer));
      } break;

      case 'b':
      {
        fmt++;
        unsigned int value = va_arg(args, unsigned int);

        /*
         * NOTE(Isaac): In binary, the longest would be 32 1s (or 0s tbf), so we need 32 characters
         */
        char valueBuffer[32u];
        itoa(value, valueBuffer, 2);

        EMIT("0b");
        EMIT(valueBuffer);
      } break;
#endif
      case 'x':
      case 'X':
      {
        fmt++;
        unsigned long value = va_arg(args, unsigned long);
        EMIT("0x");
        PrintHex(value, 0, buffer, &bufferLength);
      } break;

      default:
      {
        goto gotConfused;
      }
    }
  }

#if defined(__is_libk)
  WriteToTTY(buffer, bufferLength);
#else
  // TODO(Isaac): call the write systemcall
#endif

  return bufferLength;
}

int printf(const char* __restrict fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  int result = vprintf(fmt, args);
  va_end(args);
  return result;
}

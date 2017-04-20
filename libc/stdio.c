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
        int value = va_arg(args, int);

        /*
         * NOTE(Isaac): The longest value an `int` can take would be -2147483647, which has 11 characters.
         */
        char valueBuffer[11u];
        itoa(value, valueBuffer, 10);
        EMIT(valueBuffer);
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
#endif
      case 'b':
      {
        fmt++;
        uint64_t value = va_arg(args, uint64_t);

        /*
         * NOTE(Isaac): In binary, the longest would be 64 1s (or 0s tbf), so we need 32 characters
         */
        char valueBuffer[64u];
        itoaLong(value, valueBuffer, 2);

        EMIT("0b");
        EMIT(valueBuffer);
      } break;

      case 'x':
      case 'X':
      {
        fmt++;
        uint64_t value = va_arg(args, uint64_t);

        /*
         * NOTE(Isaac): In hex, the longest value an int can take is `-7FFFFFFF` so we need 9 characters
         */
        char valueBuffer[64u];
        itoaLong(value, valueBuffer, 16);
        EMIT("0x");
        EMIT(valueBuffer);
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

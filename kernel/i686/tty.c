/*
 * Copyright (C) 2017, Isaac Woods. All rights reserved.
 */

#include <kernel/tty.h>

static const size_t     WIDTH = 80u;
static const size_t     HEIGHT = 25u;
static const uint16_t*  VGA_MEMORY = (uint16_t*)0xB8000;

static size_t ttyRow, ttyColumn;
static uint8_t ttyColor;
static uint16_t* ttyBuffer;

static uint8_t CreateVGAColor(enum VGAColor foreground, enum VGAColor background)
{
  return foreground | background << 4;
}

static void PutVGAEntry(size_t x, size_t y, unsigned char c, uint8_t color)
{
  ttyBuffer[x+y*WIDTH] = (uint16_t)c | (uint16_t)color << 8;
}

void InitializeTerminal(void)
{
  ttyRow = 0u;
  ttyColumn = 0u;
  ttyColor = CreateVGAColor(VGA_COLOR_LIGHT_GRAY, VGA_COLOR_BLACK);
  ttyBuffer = VGA_MEMORY;

  for (size_t y = 0u;
       y < HEIGHT;
       y++)
  {
    for (size_t x = 0u;
         x < WIDTH;
         x++)
    {
      PutVGAEntry(x, y, ' ', ttyColor);
    }
  }
}

void WriteToTTY(const char* str, size_t length)
{
  for (size_t i = 0u;
       i < length;
       i++)
  {
    unsigned char c = (unsigned char)str[i];

    switch (c)
    {
      case '\n':
      {
        ttyRow++;
        ttyColumn = 0u;
      } break;

      default:
      {
        PutVGAEntry(ttyColumn, ttyRow, c, ttyColor);

        if (++ttyColumn == WIDTH)
        {
          ttyColumn = 0u;  
          if (++ttyRow == HEIGHT)
          {
            ttyRow = 0u;
          }
        }
      } break;
    }

  }
}

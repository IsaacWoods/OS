/*
 * Copyright (C) 2017, Isaac Woods. All rights reserved.
 */

#include <kernel/tty.h>
#include <kernel/i686.h>

#define TTY_WIDTH   80u
#define TTY_HEIGHT  25u

/*
 * Video memory has the physical address 0xB8000, so is mapped to 0xC00B8000 because the kernel is mapped
 * from 0x1000 to 0xC0100000
 */
#define VGA_MEMORY  ((uint16_t*)0xC00B8000)

static size_t ttyRow, ttyColumn;
static uint8_t ttyColor;
static uint16_t* ttyBuffer;

static uint8_t CreateVGAColor(enum VGAColor foreground, enum VGAColor background)
{
  return foreground | background << 4;
}

static void PutVGAEntry(size_t x, size_t y, unsigned char c, uint8_t color)
{
  ttyBuffer[x+y*TTY_WIDTH] = (uint16_t)c | (uint16_t)color << 8;
}

void InitializeTerminal(void)
{
  ttyRow = 0u;
  ttyColumn = 0u;
  ttyColor = CreateVGAColor(VGA_COLOR_LIGHT_GRAY, VGA_COLOR_BLACK);
  ttyBuffer = VGA_MEMORY;

  for (size_t y = 0u;
       y < TTY_HEIGHT;
       y++)
  {
    for (size_t x = 0u;
         x < TTY_WIDTH;
         x++)
    {
      PutVGAEntry(x, y, ' ', ttyColor);
    }
  }
}

void WriteToTTY(const char* str, size_t length)
{
  // If the screen is full, move everything up one line, leaving a free line at the bottom
  if (ttyRow >= TTY_HEIGHT)
  {
    for (size_t y = 0u;
         y < TTY_HEIGHT;
         y++)
    {
      for (size_t x = 0u;
           x < TTY_WIDTH;
           x++)
      {
        ttyBuffer[x+y*TTY_WIDTH] = ttyBuffer[x+(y+1u)*TTY_WIDTH];
      }
    }
    --ttyRow;
  }

  for (size_t i = 0u;
       i < length;
       i++)
  {
    unsigned char c = (unsigned char)str[i];

    if (c == '\n' || ttyColumn == TTY_WIDTH)
    {
      ++ttyRow;
      ttyColumn = 0u;
    }
    else if (c == '\b')
    {
      // FIXME: This doesn't work if we're at the start of the line and do a backspace
      --ttyColumn;

      PutVGAEntry(ttyColumn, ttyRow, ' ', ttyColor);
    }
    else
    {
      PutVGAEntry(ttyColumn, ttyRow, c, ttyColor);
      ++ttyColumn;
    }
  }

  // Update the cursor's position
  unsigned short position = (ttyRow*TTY_WIDTH)+ttyColumn;
  outb(0x3D4, 0x0F);
  outb(0x3D5, (unsigned char)(position&0xFF));
  outb(0x3D4, 0x0E);
  outb(0x3D5, (unsigned char)((position>>8u)&0xFF));
}

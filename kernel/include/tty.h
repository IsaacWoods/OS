/*
 * Copyright (C) 2017, Isaac Woods. All rights reserved.
 */

#pragma once

#include <stdint.h>
#include <stddef.h>

enum VGAColor
{
  VGA_COLOR_BLACK         = 0,
  VGA_COLOR_BLUE          = 1,
  VGA_COLOR_GREEN         = 2,
  VGA_COLOR_CYAN          = 3,
  VGA_COLOR_RED           = 4,
  VGA_COLOR_MAGENTA       = 5,
  VGA_COLOR_BROWN         = 6,
  VGA_COLOR_LIGHT_GRAY    = 7,
  VGA_COLOR_DARK_GRAY     = 8,
  VGA_COLOR_LIGHT_BLUE    = 9,
  VGA_COLOR_LIGHT_GREEN   = 10,
  VGA_COLOR_LIGHT_CYAN    = 11,
  VGA_COLOR_LIGHT_RED     = 12,
  VGA_COLOR_LIGHT_MAGENTA = 13,
  VGA_COLOR_LIGHT_BROWN   = 14,
  VGA_COLOR_WHITE         = 15,
};

void InitializeTerminal(void);
void WriteToTTY(const char* str, size_t length);

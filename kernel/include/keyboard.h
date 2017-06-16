/*
 * Copyright (C) 2017, Isaac Woods. All rights reserved.
 */

#pragma once

#include <stdbool.h>

extern unsigned char g_scancodeTable[];

enum KeyState
{
  KEY_PRESSED,
  KEY_RELEASED
};

struct KeyEvent
{
  unsigned char c;
  enum KeyState state;
  bool          shift;
  bool          ctrl;
  bool          alt;
};

void InitKeyEventBuffer();
void PushKeyEvent(struct KeyEvent event);
bool PopKeyEvent(struct KeyEvent* event);

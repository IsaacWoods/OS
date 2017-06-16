/*
 * Copyright (C) 2017, Isaac Woods. All rights reserved.
 */

#include <kernel/keyboard.h>
#include <stdlib.h>

unsigned char g_scancodeTable[] =
{
  /*0x00*/  0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0, '\t',  '`',    0,
  /*0x10*/  0,    0,    0,    0,    0,  'q',  '1',    0,    0,    0,  'z',  's',  'a',  'w',  '2',    0,
  /*0x20*/  0,  'c',  'x',  'd',  'e',  '4',  '3',    0,    0,  ' ',  'v',  'f',  't',  'r',  '5',    0,
  /*0x30*/  0,  'n',  'b',  'h',  'g',  'y',  '6',    0,    0,    0,  'm',  'j',  'u',  '7',  '8',    0,
  /*0x40*/  0,  ',',  'k',  'i',  'o',  '0',  '9',    0,    0,  '.',  '/',  'l',  ';',  'p',  '-',    0,
  /*0x50*/  0, '\'',    0,    0,  '[',  '=',    0,    0,    0,    0,    0,  ']',    0,    0,    0,    0,
  /*0x60*/  0,    0,    0,    0,    0,    0,    0,    0,    0,  '1',    0,  '4',  '7',    0,    0,    0,
  /*0x70*/'0',  '.',  '2',  '5',  '6',  '8',    0,    0,    0,  '+',  '3',  '-',  '*',  '9',    0,    0,
  /*0x80*/  0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
};

#define RING_BUFFER_SIZE 16u
static struct KeyEvent    g_keyBuffer[RING_BUFFER_SIZE] = {};
static struct KeyEvent*   g_keyHead;
static struct KeyEvent*   g_keyTail;
static size_t             g_keyEventCount;

void InitKeyEventBuffer()
{
  g_keyHead = &g_keyBuffer[0u];
  g_keyTail = g_keyHead;
  g_keyEventCount = 0u;
}

void PushKeyEvent(struct KeyEvent event)
{
  g_keyEventCount++;
  
  if (g_keyHead == &g_keyBuffer[RING_BUFFER_SIZE-1u])
  {
    g_keyHead = &g_keyBuffer[0u];
  }
  
  memcpy(g_keyHead, &event, sizeof(struct KeyEvent));
  g_keyHead++;
}

bool PopKeyEvent(struct KeyEvent* event)
{
  if (g_keyEventCount == 0u)
  {
    return false;
  }
  
  if (g_keyTail == &g_keyBuffer[RING_BUFFER_SIZE-1u])
  {
    g_keyTail = &g_keyBuffer[0u];
  }

  g_keyEventCount--;
  memcpy(event, g_keyTail, sizeof(struct KeyEvent));
  g_keyTail++;
  return true;
}

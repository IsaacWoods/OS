/*
 * Copyright (C) 2017, Isaac Woods. All rights reserved.
 */

#pragma once

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

struct registers
{
  uint32_t ds;                                      // Preserved by the ISR stub
  uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;  // Pushed by pusha
  uint32_t intNum, errCode;                         // Pushed by the ISR wrapper
  uint32_t eip, cs, eflags, useresp, ss;            // Automatically pushed by the CPU
};

#define IRQ0  32
#define IRQ1  33
#define IRQ2  34
#define IRQ3  35
#define IRQ4  36
#define IRQ5  37
#define IRQ6  38
#define IRQ7  39
#define IRQ8  40
#define IRQ9  41
#define IRQ10 42
#define IRQ11 43
#define IRQ12 44
#define IRQ13 45
#define IRQ14 46
#define IRQ15 47

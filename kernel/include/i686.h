/*
 * Copyright (C) 2017, Isaac Woods. All rights reserved.
 */

#pragma once

struct registers
{
  uint32_t ds;                                      // Preserved by the ISR stub
  uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;  // Pushed by pusha
  uint32_t intNum, errCode;                         // Pushed by the ISR wrapper
  uint32_t eip, cs, eflags, useresp, ss;            // Automatically pushed by the CPU
};

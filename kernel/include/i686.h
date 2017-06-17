/*
 * Copyright (C) 2017, Isaac Woods. All rights reserved.
 */

#pragma once

void outb(uint16_t port, uint8_t value);
uint8_t inb(uint16_t port);

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

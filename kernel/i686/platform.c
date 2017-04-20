/*
 * Copyright (C) 2017, Isaac Woods. All rights reserved.
 */

#include <kernel/platform.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>

__attribute__((__noreturn__))
void Internal_KernelPanic(const char* file, int line, const char* message, ...)
{
  va_list args;
  va_start(args, message);

  printf("PANIC(%s:%d): ", file, line);
  vprintf(message, args);

  va_end(args);
  abort();
}

// --- GDT and IDT ---
#define NUM_GDT_ENTRIES 5u
#define NUM_IDT_ENTRIES 256u

#define SEG_DESCTYPE(x)  ((x) << 0x04)            // Descriptor type (0 for system, 1 for code/data)
#define SEG_PRES(x)      ((x) << 0x07)            // Present
#define SEG_SAVL(x)      ((x) << 0x0C)            // I have no idea tbh, stolen from manual (should be 0)
#define SEG_LONG(x)      ((x) << 0x0D)            // Long mode
#define SEG_SIZE(x)      ((x) << 0x0E)            // Size (0 for 16-bit, 1 for 32-bit)
#define SEG_GRAN(x)      ((x) << 0x0F)            // Granularity (0 for bytes, 1 for 4kb pages)
#define SEG_PRIV(x)     (((x) &  0x03) << 0x05)   // Ring (0-3)
#define SEG_DATA_RDWR    0x02
#define SEG_CODE_EXRD    0x0A

static uint64_t gdtEntries[NUM_GDT_ENTRIES];
static struct gdt_pointer
{
  uint16_t size;
  uint32_t firstEntryAddr;
} __attribute__((packed)) gdtPtr;
static uint64_t idtEntries[NUM_IDT_ENTRIES];
static struct idt_pointer
{
  uint16_t size;
  uint32_t firstEntryAddr;
} __attribute__((packed)) idtPtr;

static uint64_t CreateGDTEntry(uint32_t base, uint32_t limit, uint16_t type)
{
  uint64_t entry = 0u;
  entry  = limit & 0x000F0000;
  entry |= (type << 8u) & 0x00F0FF00;
  entry |= (base >> 16u) & 0x000000FF;
  entry |= base & 0xFF000000;
  entry <<= 32u;
  entry |= base << 16u;
  entry |= limit & 0x0000FFFF;

  return entry;
}

static uint64_t CreateIDTEntry(uint32_t offset, uint16_t selector, uint8_t type)
{
  uint64_t entry = 0u;
  entry  = offset & 0x0000FFFF;
  entry |= selector << 16u;
  entry |= (uint64_t)type << 40u;
  entry |= ((uint64_t)(offset >> 16u) & 0x0000FFFF) << 48u;

  return entry;
}

extern void isr0 ();
extern void isr1 ();
extern void isr2 ();
extern void isr3 ();
extern void isr4 ();
extern void isr5 ();
extern void isr6 ();
extern void isr7 ();
extern void isr8 ();
extern void isr9 ();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();

extern void FlushGDT(uint32_t);
extern void FlushIDT(uint32_t);
void InitPlatform()
{
  // --- Create the GDT ---
  gdtPtr.size = (sizeof(uint64_t) * NUM_GDT_ENTRIES) - 1u;
  gdtPtr.firstEntryAddr = (uint32_t)&gdtEntries;

  // Null segment
  gdtEntries[0u] = CreateGDTEntry(0u, 0u, 0u);

  // Kernel code segment
  gdtEntries[1u] = CreateGDTEntry(0u, 0xFFFFF, SEG_DESCTYPE(1)|SEG_PRES(1)|SEG_SAVL(0)|
                                               SEG_LONG(0)|SEG_SIZE(1)|SEG_GRAN(1)|
                                               SEG_PRIV(0)|SEG_CODE_EXRD);

  // Kernel data segment
  gdtEntries[2u] = CreateGDTEntry(0u, 0xFFFFF, SEG_DESCTYPE(1)|SEG_PRES(1)|SEG_SAVL(0)|
                                               SEG_LONG(0)|SEG_SIZE(1)|SEG_GRAN(1)|
                                               SEG_PRIV(0)|SEG_DATA_RDWR);

  // User code segment
  gdtEntries[3u] = CreateGDTEntry(0u, 0xFFFFF, SEG_DESCTYPE(1)|SEG_PRES(1)|SEG_SAVL(0)|
                                               SEG_LONG(0)|SEG_SIZE(1)|SEG_GRAN(1)|
                                               SEG_PRIV(3)|SEG_CODE_EXRD);

  // User data segment
  gdtEntries[4u] = CreateGDTEntry(0u, 0xFFFFF, SEG_DESCTYPE(1)|SEG_PRES(1)|SEG_SAVL(0)|
                                               SEG_LONG(0)|SEG_SIZE(1)|SEG_GRAN(1)|
                                               SEG_PRIV(3)|SEG_DATA_RDWR);

  FlushGDT((uint32_t)&gdtPtr);

  // --- Create the IDT ---
  idtPtr.size = (sizeof(uint64_t) * NUM_IDT_ENTRIES) - 1u;
  idtPtr.firstEntryAddr = (uint32_t)&idtEntries;

  // TODO: better way of specifying types
  idtEntries[0u ] = CreateIDTEntry((uint32_t)isr0 , 0x08, 0x8E);
  idtEntries[1u ] = CreateIDTEntry((uint32_t)isr1 , 0x08, 0x8E);
  idtEntries[2u ] = CreateIDTEntry((uint32_t)isr2 , 0x08, 0x8E);
  idtEntries[3u ] = CreateIDTEntry((uint32_t)isr3 , 0x08, 0x8E);
  idtEntries[4u ] = CreateIDTEntry((uint32_t)isr4 , 0x08, 0x8E);
  idtEntries[5u ] = CreateIDTEntry((uint32_t)isr5 , 0x08, 0x8E);
  idtEntries[6u ] = CreateIDTEntry((uint32_t)isr6 , 0x08, 0x8E);
  idtEntries[7u ] = CreateIDTEntry((uint32_t)isr7 , 0x08, 0x8E);
  idtEntries[8u ] = CreateIDTEntry((uint32_t)isr8 , 0x08, 0x8E);
  idtEntries[9u ] = CreateIDTEntry((uint32_t)isr9 , 0x08, 0x8E);
  idtEntries[10u] = CreateIDTEntry((uint32_t)isr10, 0x08, 0x8E);
  idtEntries[11u] = CreateIDTEntry((uint32_t)isr11, 0x08, 0x8E);
  idtEntries[12u] = CreateIDTEntry((uint32_t)isr12, 0x08, 0x8E);
  idtEntries[13u] = CreateIDTEntry((uint32_t)isr13, 0x08, 0x8E);
  idtEntries[14u] = CreateIDTEntry((uint32_t)isr14, 0x08, 0x8E);
  idtEntries[15u] = CreateIDTEntry((uint32_t)isr15, 0x08, 0x8E);
  idtEntries[16u] = CreateIDTEntry((uint32_t)isr16, 0x08, 0x8E);
  idtEntries[17u] = CreateIDTEntry((uint32_t)isr17, 0x08, 0x8E);
  idtEntries[18u] = CreateIDTEntry((uint32_t)isr18, 0x08, 0x8E);
  idtEntries[19u] = CreateIDTEntry((uint32_t)isr19, 0x08, 0x8E);
  idtEntries[20u] = CreateIDTEntry((uint32_t)isr20, 0x08, 0x8E);
  idtEntries[21u] = CreateIDTEntry((uint32_t)isr21, 0x08, 0x8E);
  idtEntries[22u] = CreateIDTEntry((uint32_t)isr22, 0x08, 0x8E);
  idtEntries[23u] = CreateIDTEntry((uint32_t)isr23, 0x08, 0x8E);
  idtEntries[24u] = CreateIDTEntry((uint32_t)isr24, 0x08, 0x8E);
  idtEntries[25u] = CreateIDTEntry((uint32_t)isr25, 0x08, 0x8E);
  idtEntries[26u] = CreateIDTEntry((uint32_t)isr26, 0x08, 0x8E);
  idtEntries[27u] = CreateIDTEntry((uint32_t)isr27, 0x08, 0x8E);
  idtEntries[28u] = CreateIDTEntry((uint32_t)isr28, 0x08, 0x8E);
  idtEntries[29u] = CreateIDTEntry((uint32_t)isr29, 0x08, 0x8E);
  idtEntries[30u] = CreateIDTEntry((uint32_t)isr30, 0x08, 0x8E);
  idtEntries[31u] = CreateIDTEntry((uint32_t)isr31, 0x08, 0x8E);

  FlushIDT((uint32_t)&idtPtr);
}

struct registers
{
  uint32_t ds;                                      // Preserved by the ISR stub
  uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;  // Pushed by pusha
  uint32_t intNum, errCode;                         // Pushed by the ISR wrapper
  uint32_t eip, cs, eflags, useresp, ss;            // Automatically pushed by the CPU
};

void HandleISR(struct registers regs)
{
  printf("Interrupt: %u\n", regs.intNum);
}

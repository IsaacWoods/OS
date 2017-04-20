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

// --- GDT ---
#define NUM_GDT_ENTRIES 5u

#define SEG_DESCTYPE(x)  ((x) << 0x04) // Descriptor type (0 for system, 1 for code/data)
#define SEG_PRES(x)      ((x) << 0x07) // Present
#define SEG_SAVL(x)      ((x) << 0x0C) // Available for system use
#define SEG_LONG(x)      ((x) << 0x0D) // Long mode
#define SEG_SIZE(x)      ((x) << 0x0E) // Size (0 for 16-bit, 1 for 32)
#define SEG_GRAN(x)      ((x) << 0x0F) // Granularity (0 for 1B - 1MB, 1 for 4KB - 4GB)
#define SEG_PRIV(x)     (((x) &  0x03) << 0x05)   // Set privilege level (0 - 3)
 
#define SEG_DATA_RD        0x00 // Read-Only
#define SEG_DATA_RDA       0x01 // Read-Only, accessed
#define SEG_DATA_RDWR      0x02 // Read/Write
#define SEG_DATA_RDWRA     0x03 // Read/Write, accessed
#define SEG_DATA_RDEXPD    0x04 // Read-Only, expand-down
#define SEG_DATA_RDEXPDA   0x05 // Read-Only, expand-down, accessed
#define SEG_DATA_RDWREXPD  0x06 // Read/Write, expand-down
#define SEG_DATA_RDWREXPDA 0x07 // Read/Write, expand-down, accessed
#define SEG_CODE_EX        0x08 // Execute-Only
#define SEG_CODE_EXA       0x09 // Execute-Only, accessed
#define SEG_CODE_EXRD      0x0A // Execute/Read
#define SEG_CODE_EXRDA     0x0B // Execute/Read, accessed
#define SEG_CODE_EXC       0x0C // Execute-Only, conforming
#define SEG_CODE_EXCA      0x0D // Execute-Only, conforming, accessed
#define SEG_CODE_EXRDC     0x0E // Execute/Read, conforming
#define SEG_CODE_EXRDCA    0x0F // Execute/Read, conforming, accessed

static uint64_t gdtEntries[NUM_GDT_ENTRIES];
static struct gdt_pointer
{
  uint16_t limit;
  uint32_t firstEntryAddr;
} __attribute__((packed)) gdtPtr;

static uint64_t CreateGDTEntry(uint32_t base, uint32_t limit, uint16_t type)
{
  uint64_t entry = 0u;
  entry = limit & 0x000F0000;
  entry |= (type << 8u) & 0x00F0FF00;
  entry |= (base >> 16u) & 0x000000FF;
  entry |= base & 0xFF000000;

  entry <<= 32u;

  entry |= base << 16u;
  entry |= limit & 0x0000FFFF;

  return entry;
}

extern void FlushGDT(uint32_t);
void InitPlatform()
{
  // Create the GDT
  gdtPtr.limit = (sizeof(uint64_t) * NUM_GDT_ENTRIES) - 1u;
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
}

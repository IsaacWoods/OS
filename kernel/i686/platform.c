/*
 * Copyright (C) 2017, Isaac Woods. All rights reserved.
 */

#include <kernel/platform.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>
#include <kernel/i686.h>
#include <kernel/keyboard.h>

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

void outb(uint16_t port, uint8_t value)
{
  asm volatile("outb %0, %1" :: "a"(value), "Nd"(port));
}

uint8_t inb(uint16_t port)
{
  uint8_t result;
  asm volatile("inb %1, %0" : "=a"(result) : "Nd"(port));
  return result;
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
extern void irq0 ();
extern void irq1 ();
extern void irq2 ();
extern void irq3 ();
extern void irq4 ();
extern void irq5 ();
extern void irq6 ();
extern void irq7 ();
extern void irq8 ();
extern void irq9 ();
extern void irq10();
extern void irq11();
extern void irq12();
extern void irq13();
extern void irq14();
extern void irq15();

extern void FlushGDT(uint32_t);
extern void FlushIDT(uint32_t);

static void PITHandler(struct registers regs)
{
  (void)regs;
/*  static unsigned int tick = 0u;
  printf("PIT fired: %u\n", tick++);*/
}

#define PS2_DATA_PORT 0x60
#define PS2_COMMAND   0x64

struct KeyboardHandlerState
{
  bool expectingReleaseScancode;
  bool shiftPressed;
  bool ctrlPressed;
  bool altPressed;
};

static struct KeyboardHandlerState g_keyboardState =
{
  .expectingReleaseScancode = false,
  .shiftPressed             = false,
  .ctrlPressed              = false,
  .altPressed               = false
};

static void KeyHandler(struct registers regs)
{
  volatile uint8_t scancode = inb(PS2_DATA_PORT);

  switch (scancode)
  {
    // Specifies that the next scancode is a released key
    case 0xF0:
    {
      g_keyboardState.expectingReleaseScancode = true;
    } return;

    case 0x12:  // Left shift
    case 0x59:  // Right shift
    {
      if (g_keyboardState.expectingReleaseScancode)
      {
        g_keyboardState.shiftPressed = false;
        g_keyboardState.expectingReleaseScancode = false;
      }
      else
      {
        g_keyboardState.shiftPressed = true;
      }
    } return;

    /*
     * TODO: Right control is 0xE0,0x14 - we need more logic to detect it
     */
    case 0x14:  // Left ctrl
    {
      if (g_keyboardState.expectingReleaseScancode)
      {
        g_keyboardState.ctrlPressed = false;
        g_keyboardState.expectingReleaseScancode = false;
      }
      else
      {
        g_keyboardState.ctrlPressed = true;
      }
    } return;

    // TODO: right alt is 0xE0,0x11
    case 0x11:  // Left alt
    {
      if (g_keyboardState.expectingReleaseScancode)
      {
        g_keyboardState.altPressed = false;
        g_keyboardState.expectingReleaseScancode = false;
      }
      else
      {
        g_keyboardState.altPressed = true;
      }
    } return;

    default:
    {
      struct KeyEvent event;

      event.c = (g_keyboardState.shiftPressed ? g_scancodeTables[1u][scancode] : g_scancodeTables[0u][scancode]);

      if (g_keyboardState.expectingReleaseScancode)
      {
        event.state = KEY_RELEASED;
        g_keyboardState.expectingReleaseScancode = false;
      }
      else
      {
        event.state = KEY_PRESSED;
      }

      PushKeyEvent(event);
    } return;
  }
}

static void InitPS2Controller()
{
  /*
   * Bit 0 of the status register must be SET before attempting to read data from port 0x60
   * Bit 1 of the status register must be CLEAR before attempting to write data to port 0x60
   */
  #define WAIT_FOR_READ()   while ((inb(PS2_COMMAND) & 1u) == 0u) asm volatile("rep nop");
  #define WAIT_FOR_WRITE()  while ((inb(PS2_COMMAND) & 2u) != 0u) asm volatile("rep nop");

  // TODO: We should first check for USB controllers and disable USR Legacy Support
  // TODO: We should also use ACPI to check the controller actually exists
 
  outb(PS2_COMMAND, 0xAD);    // Disable the device on Channel 1
  outb(PS2_COMMAND, 0xA7);    // Disable the device on Channel 2 (if it exists)

  (void)inb(PS2_DATA_PORT);   // Flush the output buffer, in case there's data stuck in it

  // Read the Controller Configuration Byte
  outb(PS2_COMMAND, 0x20);
  WAIT_FOR_READ();
  uint8_t configByte = inb(PS2_DATA_PORT);

  // Modify the Controller Configuration Byte and send it back to the 8042
  configByte &= ~(1u<<0u);    // Disable interrupts from Channel 1 for now
  configByte &= ~(1u<<1u);    // Disable interrupts from Channel 2
  configByte &= ~(1u<<6u);    // Disable scancode translation on Channel 1

  outb(PS2_COMMAND, 0x60);
  WAIT_FOR_WRITE();
  outb(PS2_DATA_PORT, configByte);

  // Perform the P2/2 controller POST
  outb(PS2_COMMAND, 0xAA);
  WAIT_FOR_READ();
  if (inb(PS2_DATA_PORT) != 0x55)
  {
    printf("PS/2 controller POST failed!\n");
  }

  // TODO: We assume it's a single channel controller from now on - probably check that?
  // Perform interface tests
  outb(PS2_COMMAND, 0xAB);
  WAIT_FOR_READ();
  if (inb(PS2_DATA_PORT) != 0x00)
  {
    printf("PS/2 controller (Channel 1) interface test failed\n");
  }

  // Enable the devices again and enable their interrupts
  outb(PS2_COMMAND, 0xAE);
  outb(PS2_COMMAND, 0x20);
  WAIT_FOR_READ();
  configByte = inb(PS2_DATA_PORT);
  configByte |= (1u<<0u);
  outb(PS2_COMMAND, 0x60);
  WAIT_FOR_WRITE();
  outb(PS2_DATA_PORT, configByte);

  // Send a reset command each active device, and check it succeeds
  WAIT_FOR_WRITE();
  outb(PS2_DATA_PORT, 0xFF);
  WAIT_FOR_READ();
  /*
   * TODO: implement a timeout here - the device may also return 0xFC on failure or just
   * fail to return anything if it doesn't actually exist
   */
  if (inb(PS2_DATA_PORT) != 0xFA)
  {
    printf("Failed to reset device on PS/2 Channel 1\n");
  }
}

#define PIC_MASTER_COMMAND          0x20
#define PIC_MASTER_DATA             0x21
#define PIC_SLAVE_COMMAND           0xA0
#define PIC_SLAVE_DATA              0xA1

#define PIC_COMMAND_READ_ISR        0x0B
#define PIC_COMMAND_INIT_ICW1       0x10
#define PIC_COMMAND_ICW1_ICW4       0x01  // Mark ICW4 as unneeded
#define PIC_COMMAND_ICW1_SINGLE     0x02  // Single (cascade) mode
#define PIC_COMMAND_ICW1_INTERVAL4  0x04  // Call address interval 4 (8)
#define PIC_COMMAND_ICW1_LEVEL      0x08  // Level triggered mode
#define PIC_COMMAND_ICW4_8086       0x01  // 8086/88 (MCS-80/85) mode
#define PIC_COMMAND_EOI             0x20

static uint16_t ReadISRFromPIC()
{
  outb(PIC_MASTER_COMMAND,  PIC_COMMAND_READ_ISR);
  outb(PIC_SLAVE_COMMAND,   PIC_COMMAND_READ_ISR);
  return (inb(PIC_SLAVE_COMMAND) << 8u) | inb(PIC_MASTER_COMMAND);
}

static void RemapPIC()
{
  outb(PIC_MASTER_COMMAND,  PIC_COMMAND_INIT_ICW1+PIC_COMMAND_ICW1_ICW4);
  outb(PIC_SLAVE_COMMAND,   PIC_COMMAND_INIT_ICW1+PIC_COMMAND_ICW1_ICW4);
  outb(PIC_MASTER_DATA,     0x20);    // Set the master PIC's vector offset to 32-39
  outb(PIC_SLAVE_DATA,      0x28);    // Set the slave PIC's vector offset to 40-47
  outb(PIC_MASTER_DATA,     0x04);    // Tell the master PIC that IRQ2 is a slave
  outb(PIC_SLAVE_DATA,      0x02);    // Tell the slave PIC its cascade identity
  outb(PIC_MASTER_DATA,     PIC_COMMAND_ICW4_8086);
  outb(PIC_SLAVE_DATA,      PIC_COMMAND_ICW4_8086);
  outb(PIC_MASTER_DATA,     0x0);     // Set the mask of the master to 0
  outb(PIC_SLAVE_DATA,      0x0);     // Set the mask of the slave to 0
}

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
  RemapPIC();

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
  idtEntries[32u] = CreateIDTEntry((uint32_t)irq0 , 0x08, 0x8E);
  idtEntries[33u] = CreateIDTEntry((uint32_t)irq1 , 0x08, 0x8E);
  idtEntries[34u] = CreateIDTEntry((uint32_t)irq2 , 0x08, 0x8E);
  idtEntries[35u] = CreateIDTEntry((uint32_t)irq3 , 0x08, 0x8E);
  idtEntries[36u] = CreateIDTEntry((uint32_t)irq4 , 0x08, 0x8E);
  idtEntries[37u] = CreateIDTEntry((uint32_t)irq5 , 0x08, 0x8E);
  idtEntries[38u] = CreateIDTEntry((uint32_t)irq6 , 0x08, 0x8E);
  idtEntries[39u] = CreateIDTEntry((uint32_t)irq7 , 0x08, 0x8E);
  idtEntries[40u] = CreateIDTEntry((uint32_t)irq8 , 0x08, 0x8E);
  idtEntries[41u] = CreateIDTEntry((uint32_t)irq9 , 0x08, 0x8E);
  idtEntries[42u] = CreateIDTEntry((uint32_t)irq10, 0x08, 0x8E);
  idtEntries[43u] = CreateIDTEntry((uint32_t)irq11, 0x08, 0x8E);
  idtEntries[44u] = CreateIDTEntry((uint32_t)irq12, 0x08, 0x8E);
  idtEntries[45u] = CreateIDTEntry((uint32_t)irq13, 0x08, 0x8E);
  idtEntries[46u] = CreateIDTEntry((uint32_t)irq14, 0x08, 0x8E);
  idtEntries[47u] = CreateIDTEntry((uint32_t)irq15, 0x08, 0x8E);

  FlushIDT((uint32_t)&idtPtr);

  RegisterInterruptHandler(IRQ0, &PITHandler);

  InitPS2Controller();
  RegisterInterruptHandler(IRQ1, &KeyHandler);

  asm volatile("sti");
}

#define USE_PIT
void SetTimerFrequency(uint32_t frequency)
{
#ifdef USE_PIT
  /*
   * We need to calculate the number we want the PIT to divide its base frequency (1193180 Hz) by.
   * This number is weird just because historical reasons.
   */
  uint32_t divisor = 1193180u/frequency;
  outb(0x43, 0x36);   // NOTE(Isaac): 0x36 sets the PIT to repeating mode, and tells it we're sending a divisor
  outb(0x40, (uint8_t)(divisor&0xFF));
  outb(0x40, (uint8_t)((divisor>>8u)&0xFF));
#elif
  #error "At least one timer must be enabled (i686 platform)!"
#endif
}

void HandleISR(struct registers regs)
{
  printf("Interrupt: %u\n", regs.intNum);
}

interrupt_handler_t interruptHandlers[NUM_IDT_ENTRIES];
void HandleIRQ(struct registers regs)
{
  uint32_t irq = regs.intNum - 32u;

  // Handle spurious IRQ7 and IRQ15
  if (irq == 7u && (ReadISRFromPIC() & (1u << 7u)))
  {
    return;
  }

  if (irq == 15u && (ReadISRFromPIC() & (1u << 15u)))
  {
    outb(PIC_MASTER_COMMAND, PIC_COMMAND_EOI);
    return;
  }

  if (interruptHandlers[regs.intNum])
  {
    interrupt_handler_t handler = interruptHandlers[regs.intNum];
    handler(regs);
  }

  // Send the master PIC an EOI
  outb(PIC_MASTER_COMMAND, PIC_COMMAND_EOI);

  if (regs.intNum >= 40u)
  {
    // If IRQ was handled by the slave PIC, also send that an EOI
    outb(PIC_SLAVE_COMMAND, PIC_COMMAND_EOI);
  }
}

void RegisterInterruptHandler(uint8_t interruptNum, interrupt_handler_t handler)
{
  interruptHandlers[interruptNum] = handler;
}

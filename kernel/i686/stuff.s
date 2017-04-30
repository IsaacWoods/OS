;
; Copyright (C) 2017, Isaac Woods. All rights reserved.
;

; Inputs: Physical address of GDT to be loaded
global FlushGDT
FlushGDT:
  mov eax, [esp+4]
  lgdt [eax]

  ; We can now switch to the new data selectors (0x10 is the selector for the new kernel data segment)
  mov ax, 0x10
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax
  mov ss, ax

  ; We now do a far jump to the new kernel code segment (0x08 is the selector for the new code segment)
  jmp 0x08:.flush
.flush:
  ret

; Inputs: Physical address of IDT to be loaded
global FlushIDT
FlushIDT:
  mov eax, [esp+4]
  lidt [eax]
  ret

%macro ISR_NOERR 1
  global isr%1
  isr%1:
    cli
    push byte 0
    push byte %1
    jmp IsrStub
%endmacro

%macro ISR_ERR 1
  global isr%1
  isr%1:
    cli
    push byte %1
    jmp IsrStub
%endmacro

; First param   - ISR mapping
; Second param  - IRQ number
%macro IRQ 2
  global irq%2
  irq%2:
    cli
    push byte 0
    push byte %1
    jmp IrqStub
%endmacro

ISR_NOERR 0
ISR_NOERR 1
ISR_NOERR 2
ISR_NOERR 3
ISR_NOERR 4
ISR_NOERR 5
ISR_NOERR 6
ISR_NOERR 7
ISR_ERR   8
ISR_NOERR 9
ISR_ERR   10
ISR_ERR   11
ISR_ERR   12
ISR_ERR   13
ISR_ERR   14
ISR_NOERR 15
ISR_NOERR 16
ISR_NOERR 17
ISR_NOERR 18
ISR_NOERR 19
ISR_NOERR 20
ISR_NOERR 21
ISR_NOERR 22
ISR_NOERR 23
ISR_NOERR 24
ISR_NOERR 25
ISR_NOERR 26
ISR_NOERR 27
ISR_NOERR 28
ISR_NOERR 29
ISR_NOERR 30
ISR_NOERR 31
IRQ       32, 0
IRQ       33, 1
IRQ       34, 2
IRQ       35, 3
IRQ       36, 4
IRQ       37, 5
IRQ       38, 6
IRQ       39, 7
IRQ       40, 8
IRQ       41, 9
IRQ       42, 10
IRQ       43, 11
IRQ       44, 12
IRQ       45, 13
IRQ       46, 14
IRQ       47, 15

extern HandleISR
IsrStub:
  ; Preserve registers and the current data segment selector
  pusha
  mov ax, ds
  push eax

  ; Load the kernel's data segment
  mov ax, 0x10
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax

  call HandleISR

  ; Reload the original data segment
  pop eax
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax

  ; Restore saved registers and clean up the error code
  popa
  add esp, 8
  sti
  iret

extern HandleIRQ
IrqStub:
  pusha
  mov ax, ds
  push eax

  mov ax, 0x10
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax

  call HandleIRQ

  pop ebx
  mov ds, bx
  mov es, bx
  mov fs, bx
  mov gs, bx

  popa
  add esp, 8
  sti
  ret

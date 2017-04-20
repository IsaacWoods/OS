;
; Copyright (C) 2017, Isaac Woods. All rights reserved.
;

global FlushGDT
; Inputs: Physical address of GDT to be loaded
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

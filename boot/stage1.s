;
; Copyright (C) 2017, Isaac Woods. All rights reserved.
;

mov ax, 0x07c0
mov ds, ax

mov si, msg
loop:
  lodsb
  or al, al
  jz hang
  mov ah, 0x0E
  int 0x10
  jmp loop

hang:
  jmp hang

msg db 'Hello, World!', 13, 10, 0

times 510-($-$$) db 0
db 0x55
db 0xAA

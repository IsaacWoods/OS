MBALIGN   equ 1<<0
MEMINFO   equ 1<<1
FLAGS     equ MBALIGN | MEMINFO
MAGIC     equ 0x1BADB002
CHECKSUM  equ -(MAGIC + FLAGS)

KERNEL_VIRTUAL_BASE equ 0xC0000000                  ; 3GB
KERNEL_PAGE_NUMBER  equ (KERNEL_VIRTUAL_BASE >> 22) ; Index into the page directory (with 4MG PTE)

section .data
align 0x1000
BootPageDirectory:
  ; We identity-map the first 4MB of the physical address space
  ; This means all bits are clear except:
  ; Bit 7 - PS (So the kernel page is 4MB)
  ; Bit 1 - RW (So the kernel page is read/write)
  ; Bit 0 - P  (So the kernel page is present)
  dd 0x00000083                               ; This page is needed otherwise we crash after paging is enabled
                                              ; because we can't fetch the next instruction. Okay to unmap later.
  times (KERNEL_PAGE_NUMBER - 1) dd 0         ; These are the pages before the kernel
  dd 0x00000083                               ; This is the kernel's page
  times (1024 - KERNEL_PAGE_NUMBER - 1) dd 0  ; These are the pages after the kernel

section .text
align 4
MultibootHeader:
  dd MAGIC
  dd FLAGS
  dd CHECKSUM

extern _init
extern kmain
global _start
_start:
  cli

  ; Before paging is set up, we must use position-independent code and use physical, not virtual, addresses
  mov ecx, (BootPageDirectory - KERNEL_VIRTUAL_BASE)
  mov cr3, ecx

  ; Set PSE bit in CR4 to enable 4MB pages
  mov ecx, cr4
  or ecx, 0x00000010
  mov cr4, ecx

  ; Set PG bit in CR0 to enable paging
  mov ecx, cr0
  or ecx, 0x80000000
  mov cr0, ecx

  ; Since EIP holds a physical address at this point, we need to do a long-jump to the correct virtual address
  ; This must be an absolute jump
  lea ecx, [StartHigherHalf]
  jmp ecx

StartHigherHalf:
  ; We can now unmap the first identity-mapped page, since we're in the virtual address space
  mov dword [BootPageDirectory], 0
  invlpg [0]

  ; Paging is been enabled from this point
  ; The first 4MB of physical address space is mapped starting from KERNEL_VIRTUAL_BASE.
  mov esp, stack_top

  ; Make sure the stack is 16-byte aligned
  and esp, -16

  call _init

  ; At the moment, the address of the Multiboot structure is a physical address, so we add the virtual base
  add ebx, KERNEL_VIRTUAL_BASE
  push ebx    ; Pass address of Multiboot info - WARNING: This is a physical address!
  push eax    ; Pass Multiboot magic number onto the kernel
  call kmain

  cli
.hang:
  hlt
  jmp .hang
.end:

section .bss
align 32
stack_bottom:
  resb 16384    ; 16 KiB
stack_top:

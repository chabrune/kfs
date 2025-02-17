section .multiboot
align 4
dd 0x1BADB002 ; magic number 
dd 0x01 | 0x02
dd - (0x1BADB002 + (0x01 | 0x02)) ; checksum

extern kmain

global _start

section .bss
   align 16
   stack_bottom:
      resb 4096
   stack_top:

section .text
_start:
   mov esp, stack_top
   call kmain
   hang:
      cli
      hlt
      jmp hang

section .multiboot
align 4
dd 0x1BADB002  ; magic number 
dd 0x01 | 0x02
dd - (0x1BADB002 + (0x01 | 0x02)) ; checksum

extern kmain
extern stack_top

global _start

section .text
_start:
   mov esp, stack_top
   cli
   call kmain
   hlt
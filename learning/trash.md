GRUB (bootloader)
BIOS -> Bootloader -> OS

Virtualisation via KVM (Qemu) / meilleur pour debug / gdb integre

Utilisation d'une vm ? 

Disque dur virtuel / physique 
Installer grub sur le disque


Structure info grub

boot asm

bios -> grub -> asm -> c  

compiler -o -> linker file via ld

solution temporaire affichage / clavier ?

kvm -curses en cli



- asm boot file :
    - handles multiboot header / call main ft
    - Section .multiboot -> magic number + flags ?

- C kernel file : 
    - handles init / display smt on screen

- Write some helpers functions we'll need later

- Create linker file with ld

- Make all with flag to .o and link to create the final kernel binary

db = define byte

Starts up computer :

1. BIOS is copied from a ROM chip into RAM
2. BIOS starts executing code 
    - initializes hardware
    - run somes tests (POST = power-on self test)
3. BIOS searches for an operating system to start
4. BIOS loads qnd starts the OS
5. Operating system runs

How the BIOS finds an os | 2 ways

1. Legacy booting :

- BIOS loads first sector of each bootable device into memory (at location 0x7C00)
- BIOS checks for 0xAA55 signature
- If found, it starts executing code

2. EFI

- BIOS looks into special EFI partitions
- Operating system must be compiled as an EFI program

ORG (directive)
- Tells assembler where we expect our code to be loaded. The assembler use this information to calculate label adresses.

Registers :

- General purpose registers
        8   16 32  64
    - AL|AH AX EAX RAX
    - BL|BH BX EBX RBX
    - CL|CH CX ECX RCX
    - DL|DH DX EDX RDX
    -  BPL  BP EBP RBP
    -  SIL  SI ESI RSI
    -  DIL  DI EDI RDI
    -  SPL  SP ESP RSP
    -  R8B R8W R8D R8 -> 9/10/11/12/13/14/15

- Status registers
    - IP EIP RIP
    - FLAGS EFLAGS RFLAGS

- Segment registers : These registers are used to specify currently active segments
    - CS - currently running code segment
    - SS - data segment  
    - DS - stack segment
    - ES FS GS - extra (data) segments

- Protected mode registers
    - GDTR IDTR TR LDTR

- Control registers
    - CRO -> 1/2/3/3/MSW/4/5/6/7/8/9/10/11/12/13/14/15/MXCSR

- Debug registers 
    - DR0 -> DR15

- Floating point unit registers
    - ST(0) MM0 -> ST(7) MM7
    - CW SW TW 
    - FP_IP FP_DP FP_CS FP_DS FP_OPC 

- SIMD registers
    - ZMM0 YMM0 XMM0 -> ZMM15 YMM15 XMM15

Memory segmentation
- 0x1234:0x5678 = segment:offset
- each segment = 64k
- overlap with 16 bits each other
- real_address = segment * 16 + offset

exemple :
- segment:offset    real address
  0x0000:0x7C00        0x7C00
  0x0001:0x7BF0        0x7C00
  0x0010:0x7B00        0x7C00
  0x00C0:0x7000        0x7C00
  0x7C00:0x0000        0x7C00



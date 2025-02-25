# 1. What is an OS ?

Os is a software that allows the user to run programs while managing IO devices, memory, CPU time, and data.

IO devices = responding to human action
Memory = workstation for processes
CPU time = multitasking etc
Data = file system

Structure of Os project :
User space
kernel
drivers
bootloader

User space communicate with signals (which are interrupts) to the kernel
    --> system call / syscall

They are basically requests to the kernel for something that has to be done.
Because user has minimal permissions (--> RINGS). User is an idiot.

There is an abstraction which are drivers. They can be placed inside of the kernel or outside.
Car comparaison : Car is just bunch of metal and need a driver to be usefull. It's the same for mouse : They need driver to be initialize and then they aren't just bunch of plastic.

Bootloader boot our kernel

3 Kernel design :
- Monolithic kernel = One big rock/kernel that contains everything on it
- Micro kernel = Contain a micro kernel that has bare minimum and work with servers and drivers (which are just programs) and the user space first communicates with these programs not directly with the kernel (They have previlege between kernel and user space).
- hybrid kernel = for some functionality the communication will go direct to the kernel and for some other the communication will go trough drivers and servers.

# 2. Booting

What happens when press the power button on computer :
- launching the BIOS (basic I/O) : responsible for booting and for basic I/O operations

IBM PC BIOS became sort of standard in 1981 -> Rename to the Legacy BIOS/CSM (compatibility support mode)
UEFI (unified extensible firmware interface) appears in 2006 but became used in 2012 with windows 8

interface = a device or program enabling a user to communicate with a computer.

First thing BIOS do : 
POST (power-on self test) : test to determinate if everything is fine with me and all other hardware on the motherboard. If everything ok -> next step
If not -> error message

Then go to his boot priority list (something that can store Os)
It can be : 
- hard drive
- usb mass storage
- network ?

If I set usb mass storage at top priority on the list and plug USB stick with my childhood photo the BIOS will try to boot with my photos. 
Spoiler : He cannot
For that reason they had to implement some way for the BIOS to know if storage include any bootable code. And the way this is done depends on whether using Legacy BIOS or UEFI BIOS.

Sector : 512 bytes chunk memory that mass storage devices are split into
The first sector of a mass storage device is called its boot sector

## Legacy BIOS :
- Loads boot sector into the random access memory at hexa address 0x7C00 and look for a signature 01010101 10101010 -> 55 AA

Why 01010101 10101010 ? 
If we have 8 bits bus then all combinations will be tested/ will be forced to change his states (0 or 1). It's a very good test over every bit can be flipped.
0x86 CPU is most little endian based so signature will be 0xAA55.

So the BIOS loads the boot sector into RAM and the 2 last bytes of the boot sector has to be 0x55AA.
It's recommanded to fill those 510 bytes with only 0 to avoid misinterpret of the CPU
An infinite loop is added (jmp $) who force the CPU to always have an instruction to do. Why ?
Because if not the CPU will search into the next mass storage device to find a bootsector.

firmware = permanent software programmed into a read-only memory.
partition = is a sort of given section of disk which has a determined starting and ending point - it's a way to split of the disk in some smaller sections

```
Master Boot Record :

| **Offset** | **Taille** | **Contenu** |
|------------|-----------|-------------|
| 0x0000 | 440 bytes | **bootloader code** (boot program) |
| 0x01B8 | 4 bytes | **disk signature** (unique ID of the disk) |
| 0x01BC | 2 bytes | **reserved space** (unused and set to 0) |
| 0x01BE | 64 bytes | **Table des partitions** (4 entries of 16 bytes) |
| 0x01FE | 2 bytes | **signature MBR (0x55AA)** |


Table des partitions (64 octets)
    Contient 4 entrées de 16 octets (1 par partition).
    Chaque entrée décrit une partition :
        Type (FAT32, NTFS, EXT4, etc.)
        Adresse de début et de fin
        Statut (Active/Bootable)
```
**MBR is something you only need if you are going to implement a file system**

## UEFI :

Check for a valid GPT (GUID Partition Table)
GUID means Globally Unique Identifier
```
+---------------------------+  Secteur 0
|       MBR protégé         |  (Contient une partition 0xEE couvrant tout le disque)
+---------------------------+
|     GPT Header (Primary)  |  Secteur 1
+---------------------------+
|  Table des partitions GPT |  Secteurs 2 à 33 (habituellement 128 entrées)
+---------------------------+
|       ESP (EFI System)    |  Partition système EFI (ex: /boot/efi)
+---------------------------+
|     Autres partitions     |  (OS, data, swap, etc.)
+---------------------------+
|    GPT Header (Backup)    |  (Dernier secteur du disque)
+---------------------------+
| Table des partitions GPT  |  (Sauvegarde, juste avant le dernier secteur)
+---------------------------+
```

## Using BIOS instructions

The bootloader has to prepare the environment for the kernel
The x86 CPU boots in a 16 bits mode and have 3 main modes :
- Real mode | 16 bits
    CPU has access to only 1MB of memory | Uses BIOS INT to communicate with hardware | No memory protection or multitasking
- Protected mode | 32 bits
    Access up to 4GB of RAM | Enables protection like paging and segmentation | Bootloader sets up GDT - IDT
- Long mode | 64 bits

What the bootloader has to do :
1. Loading more disk space
    - Especially the space where the rest of the bootloader and the kernel are contained. The MBR (Master Boot Record) is only 512 bytes in size. This is too small to fit a full-fledged bootloader.
2. Going into a higher mode
    - Switches from 16-bit real mode to 32-bit protected mode or 64-bit long mode for full CPU features.
3. Gathering vital information
    - Collects memory maps, CPU info, filesystem data, and boot parameters for the kernel.

Interrupts are basically signals sent to a CPU through an external bus that have a given code.
They tell the CPU that it should stop what it's currently doing and handle something external and only when the CPU has finished handlung that external thing he can go back to the thing that he was doing.

CPU has a chip (most common is 8259 PIC chip) which functions as the interrupt controller - what should be done with those interrupts
** PIC = Programmable Interrupt Controller **
What it does ?

Keyboard exemple : When a key is pressed an interrupt is sent to the CPU with a code that tells who is sending that interrupt and the CPU can read some ports that are assigned to the keyboard and read which key is pressed

1. Key Pressed → Keyboard Controller Sends an Interrupt
    - When you press a key, the keyboard (which has its own microcontroller) sends a signal to the Programmable Interrupt Controller (PIC).
    This signal is an IRQ1 (Interrupt Request 1) because the keyboard is wired to IRQ1 on the PIC.

2. PIC Sends Interrupt to CPU
    - The PIC (Programmable Interrupt Controller) detects that IRQ1 is active.
    The PIC then sends an interrupt signal (Interrupt Vector 0x21, which is IRQ1 + 0x20 offset in protected mode) to the CPU’s IDT (Interrupt Descriptor Table).

3. CPU Handles the Interrupt via IDT
    - The CPU receives the interrupt and checks its Interrupt Descriptor Table (IDT).
    It finds the address of the keyboard interrupt handler.
    The CPU then jumps to that handler.

4. Interrupt Handler Reads the Keyboard Port
    - Your keyboard handler executes and reads from I/O port 0x60, which contains the scancode of the pressed key.
    The scancode represents which key was pressed.

5. Key Processing & Acknowledgment
    - The handler translates the scancode into a readable character (if needed).

6. End Of Interrupt
    - The handler sends an End of Interrupt (EOI) signal to the PIC to indicate the interrupt is processed:
    This allows new interrupts to be received.

System calls : 
    Syscall are handle with interrupts. A given interrupt code will be assigned to a given instruction

In real mode, the CPU and BIOS communicate via interrupts, with the BIOS executing 16-bit code to handle hardware events and return data.
BIOS is our friend.
The BIOS has access to all of the CPU registers. BIOS can read registers later.

The BIOS offers different BIOS instruction which can do different things for us. Those are oganized in so-called instruction vectors.
They are just lists of different instructions. They are accessed via **interrupt vectors** |  IVT (Interrupt Vector Table) 
Interrupt vectors → A table of addresses that point to BIOS routines.

**In protected mode the IDT replaces the IVT and defines handlers for exceptions, IRQs, and software interrupts.**

Le vecteur d'instruction 0x10 en soi est un service d'interruption du BIOS, et les valeurs dans AH (de 0x00 à 0x13) sont les différentes fonctions disponibles dans cette interruption pour gérer l'affichage sur l'écran en mode texte ou graphique.

Set up the register AH before calling the interruption
mov ah, 0x0E
INT 0x10
 
**The BIOS send 16 bits instructions code so obviously in higher bit modes you cannot ask the BIOS to do stuff**

A hard drive disc had a couple plates and in between the plates yoy have heads alternatively called read/write heads
The plates are rotating and what the head does it acts as a little elctromagnet and it switches the polarization of that region which flips the bits.
It can also act as a reading device it can get close and detect the current polarization
Sector should have 4096 different regions that can be separated from each other. 4096 / 8 = 512 bytes.
CHS -> Cylinder

Cylinder is a collection of tracks
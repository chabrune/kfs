# 1. What is an OS ?
Os is a software that allows the user to run programs while managing IO devices, memory, CPU time, and data.

IO devices = responding to human action
Memory = workstation for processes
CPU time = multitasking etc
Data = file system

Structure of Os project :
- User space
- kernel
- drivers
- bootloader


- User space communicate with signals (which are interrupts) to the kernel
    - system call / syscall

They are basically requests to the kernel for something that has to be done.
Because user has minimal permissions (--> RINGS). **User is an idiot**.

There is an abstraction which are drivers. They can be placed inside of the kernel or outside.
Car comparaison : Car is just bunch of metal and need a driver to be usefull. It's the same for mouse : They need driver to be initialize and then they aren't just bunch of plastic.

Bootloader boot our kernel

3 Kernel design :
- Monolithic kernel = One big rock/kernel that contains everything on it
- Micro kernel = Contain a micro kernel that has bare minimum and work with servers and drivers (which are just programs) and the user space first communicates with these programs not directly with the kernel (They have previlege between kernel and user space).
- hybrid kernel = for some functionality the communication will go direct to the kernel and for some other the communication will go trough drivers and servers.

---

# 2. Booting

What happens when press the power button on computer :
- launching the BIOS (basic I/O) : responsible for booting and for basic I/O operations

IBM PC BIOS became sort of standard in 1981 -> Rename to the Legacy BIOS/CSM (compatibility support mode)
UEFI (unified extensible firmware interface) appears in 2006 but became used in 2012 with windows 8

interface = a device or program enabling a user to communicate with a computer.

First thing BIOS do :

POST (power-on self test) : test to determinate if everything is fine with me and all other hardware on the motherboard. 
    - If everything ok -> next step
    - If not -> error message

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

---
## Legacy BIOS :
- Loads boot sector into the random access memory at hexa address `0x7C00` and look for a signature `01010101 10101010 -> 55 AA`

Why `01010101 10101010` ? 

If we have 8 bits bus then all combinations will be tested/ will be forced to change his states (0 or 1). It's a very good test over every bit can be flipped.

0x86 CPU is most little endian based so signature will be `0xAA55`.

So the BIOS loads the boot sector into RAM and the 2 last bytes of the boot sector has to be `0x55AA`.

It's recommanded to fill those 510 bytes with only 0 to avoid misinterpret of the CPU.

An infinite loop is added (jmp $) who force the CPU to always have an instruction to do.

Why ?

Because if not the CPU will search into the next mass storage device to find a bootsector.

firmware = permanent software programmed into a read-only memory.

partition = is a sort of given section of disk which has a determined starting and ending point - it's a way to split of the disk in some smaller sections

**Master Boot Record :**

| **Offset** | **Taille** | **Contenu** |
|------------|-----------|-------------|
| 0x0000 | 440 bytes | **bootloader code** (boot program) |
| 0x01B8 | 4 bytes | **disk signature** (unique ID of the disk) |
| 0x01BC | 2 bytes | **reserved space** (unused and set to 0) |
| 0x01BE | 64 bytes | **Table des partitions** (4 entries of 16 bytes) |
| 0x01FE | 2 bytes | **signature MBR (0x55AA)** |


- Table des partitions (64 octets) : Contient 4 entrées de 16 octets (1 par partition).
    - Chaque entrée décrit une partition :
        - Type (FAT32, NTFS, EXT4, etc.)
        - Adresse de début et de fin
        - Statut (Active/Bootable)

**MBR is something you only need if you are going to implement a file system**

---
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

---
# Using BIOS instructions

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
---
- System calls :
    - Syscall are handle with interrupts. A given interrupt code will be assigned to a given instruction

In real mode, the CPU and BIOS communicate via interrupts, with the BIOS executing 16-bit code to handle hardware events and return data.

BIOS is our friend.

The BIOS has access to all of the CPU registers. BIOS can read registers later.

The BIOS offers different BIOS instruction which can do different things for us. Those are oganized in so-called instruction vectors.

They are just lists of different instructions. They are accessed via **interrupt vectors** |  IVT (Interrupt Vector Table) 

Interrupt vectors → A table of addresses that point to BIOS routines.

**In protected mode the IDT replaces the IVT and defines handlers for exceptions, IRQs, and software interrupts.**

Le vecteur d'instruction 0x10 en soi est un service d'interruption du BIOS, et les valeurs dans AH (de 0x00 à 0x13) sont les différentes fonctions disponibles dans cette interruption pour gérer l'affichage sur l'écran en mode texte ou graphique.

**Set up the register AH before calling the interruption**
```
mov ah, 0x0E
INT 0x10
```
**The BIOS send 16 bits instructions code so obviously in higher bit modes you cannot ask the BIOS to do stuff**

A hard drive disk had a couple plates and in between the plates you have heads alternatively called read/write heads

The plates are rotating and what the head does it acts as a little elctromagnet and it switches the polarization of that region which flips the bits.

It can also act as a reading device it can get close and detect the current polarization

Sector should have 4096 different regions that can be separated from each other. 4096KB / 8 = 512 bytes.

CHS -> Cylinder-Head-Sector

Cylinder is a collection of tracks

In more modern drives, LBA (Logical Block Addressing) has replaced CHS

Avec LBA, chaque secteur du disque est simplement identifié par un numéro unique, appelé adresse LBA (LBA address). Au lieu de dire "va au cylindre 3, tête 2, secteur 8", on dit juste "va au secteur numéro 2048".

Real mode addressing :

We know that in real mode there are only 16 bits registers. But has addressing bus of 20 bits. But how a 16 bits CPU can address 20 bits of memory.

Here comes the segment:offset system

Both segment and offset are 16 bits values

ADDR = (16 * Segment) + Offset

1. Pourquoi multiplier le segment par 16 ?
    - Cela revient à décaler les bits du segment de 4 positions vers la gauche (un décalage de 4 bits correspond à une multiplication par 16).
    - Cela génère une adresse de 20 bits, permettant d’adresser 1 Mo de mémoire.

DI uselly store the offset

ES uselly store the segment but ES value is generally not modified

So we have a maximum of 128 sector each of them is 512 bytes

128*258 = 2^16 = 65536

```
Inst vector 0x13 | INT code
Instruction 0x02 | AH
```

This BIOS interrupt reads sectors from a disk into memory.

params :

| Register |
|----------|
| `AH` = `0x02` |
| `AL` = *Number of sectors to read* (1–128) |
| `CH` = *Cylinder number* (0–1023, bits 0–7) |
| `CL` = *Sector number* (1–63, bits 0–5) + (Cylinder bits 8–9 in bits 6–7) |
| `DH` = *Head number* (0–255) |
| `DL` = *Drive number* (`0x00` = Floppy, `0x80` = First HDD) |
| `ES:BX` = *Memory address to store the data* |
---

If you're loading more than 64 KB, you'd need to adjust ES accordingly.

DI (Destination Index) usually stores the offset in memory.

(Extra Segment) usually stores the segment but is not frequently modified unless switching memory banks.

Since each sector is 512 bytes, if you want to load 128 sectors, that means:

128×512=65,536 (or 216 bytes, exactly 64 KB)

This is why BIOS limits sector reads to 128 at a time—because the maximum size a 16-bit register can address in one go is 64 KB.


| **Registres** | **Utilisation** |
|--------------|----------------|
| **`ES:BX`** | BIOS `INT 0x13` → Stockage des données lues |
| **`ES:DI`** | Opérations mémoire → Copier ou manipuler des données |
---

The boot sector obviously has CHS (0, 0, 1)

So if u want to load the first sector after the boot the value will be (0, 0, 2) CH -> DH -> CL

It's recommanded to set DI to either 0x7E00 or 0x8000

Why 0x7E00 ? 

0x7E00 is 512 bytes after 0x7C00, meaning it is the next sector in memory.

---
# Global Descriptor Table and Protected mode

In 16 bits or real mode we use 20 bits bus but when 32 bits CPU has been released they have to find a way to maintain backward compatibility between 16 and 32 bits.

So they find a lazy solution the A20 line.

The A20 line—a gate that could be enabled or disabled to either allow or block access to memory above 1 MB

CPU use a feature called FAST A20 :
    - They basically read from port 0x92 | IN AL (8 bits value) 0x92
    - Then flip the A20 bit | We want the snd bit to 1 = 00000010 -> OR AL, 2 (to flip the snd bit)
    - And write to the 0x92 port | OUT 0x92, AL

Remember of logical gates :

| **Operation** | **IN A** | **IN B** | **OUT** |
|--------------|------------|------------|-----------------|
| **AND**      | 0          | 0          | 0               |
|              | 0          | 1          | 0               |
|              | 1          | 0          | 0               |
|              | 1          | 1          | 1               |
|              |            |            |                 |
| **OR**       | 0          | 0          | 0               |
|              | 0          | 1          | 1               |
|              | 1          | 0          | 1               |
|              | 1          | 1          | 1               |
|              |            |            |                 |
| **XOR**      | 0          | 0          | 0               |
|              | 0          | 1          | 1               |
|              | 1          | 0          | 1               |
|              | 1          | 1          | 0               |
|              |            |            |                 |
| **NOT**      | 0          | -          | 1               |
|              | 1          | -          | 0               |
---


GDT is a table and a collection of different values, this table is global (accessible anywhere in our Os).

To be precise GDT contains information about differents segments in memory and specify things like there base address, limit address and so on. This is a very vital component to the protected mode

GPF = General Protection Fault

GDT can be used for triggering something called GPF.

- NULL DESCRIPTOR
- CODE/DATA DESCRIPTOR
- TASK STATE SEGMENT DESCRIPTOR (TSS)
- GDTR (GDT Register)

Null descriptor is literally 8 bytes of zeros (64 bits)

Code/Data descriptor describe segments like base address, limit address etc 

Task state segment descriptor used when you make user space multitasking


Le **Task State Segment (TSS)** est une structure en mémoire qui stocke l’état d’un **task (processus ou thread)** sur un processeur x86 en mode protégé. Le **TSS Descriptor (TSSD)** est une entrée dans la **GDT** qui décrit l’emplacement et la taille du TSS.

---

Le **TSS (Task State Segment)** est utilisé par le processeur pour :

**Gérer les changements de tâche (task switching)**  
**Stocker l’état d’un processus** (registre, esp, eip, etc.)  
**Définir la stack pour les transitions de privilège (Ring 3 → Ring 0)**  

---

Le TSS en 32 bits contient plusieurs champs pour sauvegarder l’état du CPU :

| **Champ**          | **Taille** | **Description** |
|--------------------|-----------|----------------|
| **`Previous TSS`** | 4 octets   | Si on utilise des changements de tâche liés, sinon 0. |
| **`ESP0`**        | 4 octets   | Pointeur de pile (Stack) en Ring 0. |
| **`SS0`**         | 4 octets   | Segment de pile (Stack Segment) pour Ring 0. |
| **`ESP1`**        | 4 octets   | Pointeur de pile pour Ring 1 (non utilisé souvent). |
| **`SS1`**         | 4 octets   | Segment de pile pour Ring 1. |
| **`ESP2`**        | 4 octets   | Pointeur de pile pour Ring 2. |
| **`SS2`**         | 4 octets   | Segment de pile pour Ring 2. |
| **`CR3`**         | 4 octets   | Page Directory Base Register (si paging actif). |
| **`EIP`**         | 4 octets   | Instruction pointer (adresse de l’instruction exécutée). |
| **`EFLAGS`**      | 4 octets   | Registre d’état du processeur. |
| **`EAX, ECX, ...`** | 4 octets chacun | Sauvegarde des registres généraux. |
| **`LDT Selector`** | 4 octets   | Sélecteur vers la LDT utilisée. |
| **`Trap Flag`**   | 2 octets   | Déclenche une interruption après task switch (debug). |
| **`IO Map Base`** | 2 octets   | Début de la bitmap pour autoriser/interdire accès I/O. |

---
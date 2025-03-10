
---

# What is an OS?

An OS (Operating System) is a software that allows the user to run programs while managing I/O devices, memory, CPU time, and data.

- **I/O devices**: Respond to human actions.
- **Memory**: Workspace for processes.
- **CPU time**: Handles multitasking, etc.
- **Data**: Manages the file system.

### Structure of an OS Project:
- **User space**
- **Kernel**
- **Drivers**
- **Bootloader**

- **User space** communicates with signals (which are interrupts) to the kernel.
  - **System call / syscall**: These are requests to the kernel for specific tasks. The user has minimal permissions (→ **RINGS**). **The user is an idiot**.

There is an abstraction layer provided by **drivers**. They can be placed inside or outside the kernel.  
**Car analogy**: A car is just a bunch of metal and needs a driver to be useful. Similarly, a mouse needs a driver to be initialized; otherwise, it's just a piece of plastic.

The **bootloader** boots the kernel.

### 3 Kernel Designs:
1. **Monolithic kernel**: One big kernel that contains everything.
2. **Microkernel**: Contains a minimal kernel and works with servers and drivers (which are just programs). The user space communicates with these programs, not directly with the kernel (they have privileges between the kernel and user space).
3. **Hybrid kernel**: For some functionalities, communication goes directly to the kernel, while for others, it goes through drivers and servers.

---

# Booting

### What happens when you press the power button on a computer:
- The **BIOS** (Basic Input/Output System) is launched. It is responsible for booting and basic I/O operations.

The **IBM PC BIOS** became a standard in 1981 and was later renamed to **Legacy BIOS/CSM** (Compatibility Support Mode).  
**UEFI** (Unified Extensible Firmware Interface) appeared in 2006 but became widely used in 2012 with Windows 8.

**Interface**: A device or program enabling a user to communicate with a computer.

### First thing the BIOS does:
- **POST** (Power-On Self-Test): A test to determine if everything is fine with the hardware on the motherboard.
  - If everything is OK → Proceeds to the next step.
  - If not → Displays an error message.

Then, the BIOS checks its **boot priority list** (devices that can store an OS). It can be:
- Hard drive
- USB mass storage
- Network?

If you set USB mass storage as the top priority and plug in a USB stick with your childhood photos, the BIOS will try to boot from it.  
**Spoiler**: It cannot.

For this reason, a mechanism was implemented for the BIOS to determine if the storage contains bootable code. This mechanism depends on whether you are using **Legacy BIOS** or **UEFI BIOS**.

**Sector**: A 512-byte chunk of memory that mass storage devices are split into.  
The first sector of a mass storage device is called the **boot sector**.

---

## Legacy BIOS:
- Loads the boot sector into RAM at the hexadecimal address `0x7C00` and looks for a signature: `01010101 10101010` → `55 AA`.

Why `01010101 10101010`?  
If we have an 8-bit bus, all combinations will be tested, ensuring every bit can be flipped.  
The **0x86 CPU** is mostly little-endian, so the signature will be `0xAA55`.

The BIOS loads the boot sector into RAM, and the last two bytes of the boot sector must be `0x55AA`.  
It is recommended to fill the first 510 bytes with zeros to avoid misinterpretation by the CPU.

An infinite loop (`jmp $`) is added to force the CPU to always have an instruction to execute.  
Why?  
Because if not, the CPU will search the next mass storage device for a boot sector.

**Firmware**: Permanent software programmed into read-only memory.  
**Partition**: A section of the disk with a defined start and end point. It is a way to split the disk into smaller sections.

### **Master Boot Record (MBR):**

| **Offset** | **Size** | **Content** |
|------------|-----------|-------------|
| 0x0000 | 440 bytes | **Bootloader code** (boot program) |
| 0x01B8 | 4 bytes | **Disk signature** (unique ID of the disk) |
| 0x01BC | 2 bytes | **Reserved space** (unused and set to 0) |
| 0x01BE | 64 bytes | **Partition table** (4 entries of 16 bytes) |
| 0x01FE | 2 bytes | **MBR signature (0x55AA)** |

- **Partition table (64 bytes)**: Contains 4 entries of 16 bytes (1 per partition).
  - Each entry describes a partition:
    - Type (FAT32, NTFS, EXT4, etc.)
    - Start and end address
    - Status (Active/Bootable)

**MBR is something you only need if you are going to implement a file system.**

---

## UEFI:
Checks for a valid **GPT** (GUID Partition Table).  
**GUID** means **Globally Unique Identifier**.

```
+---------------------------+  Sector 0
|       Protected MBR       |  (Contains a 0xEE partition covering the entire disk)
+---------------------------+
|     GPT Header (Primary)  |  Sector 1
+---------------------------+
|  GPT Partition Table      |  Sectors 2 to 33 (usually 128 entries)
+---------------------------+
|       ESP (EFI System)    |  EFI System Partition (e.g., /boot/efi)
+---------------------------+
|     Other partitions      |  (OS, data, swap, etc.)
+---------------------------+
|    GPT Header (Backup)    |  (Last sector of the disk)
+---------------------------+
| GPT Partition Table       |  (Backup, just before the last sector)
+---------------------------+
```

---

# Using BIOS Instructions

The bootloader has to prepare the environment for the kernel.

The x86 CPU boots in **16-bit mode** and has three main modes:
- **Real mode** | 16 bits
  - The CPU has access to only 1MB of memory.
  - Uses BIOS interrupts to communicate with hardware.
  - No memory protection or multitasking.
- **Protected mode** | 32 bits
  - Access up to 4GB of RAM.
  - Enables protection like paging and segmentation.
  - The bootloader sets up the GDT and IDT.
- **Long mode** | 64 bits

### What the bootloader has to do:
1. **Load more disk space**:
   - Especially the space where the rest of the bootloader and the kernel are located. The MBR is only 512 bytes, which is too small for a full-fledged bootloader.
2. **Switch to a higher mode**:
   - Switches from 16-bit real mode to 32-bit protected mode or 64-bit long mode to access full CPU features.
3. **Gather vital information**:
   - Collects memory maps, CPU info, filesystem data, and boot parameters for the kernel.

**Interrupts** are signals sent to the CPU through an external bus with a specific code. They tell the CPU to stop its current task and handle the external event. Once the CPU finishes handling the interrupt, it resumes its previous task.

The CPU has a chip (most commonly the **8259 PIC chip**) that functions as the **Interrupt Controller**, managing these interrupts.

**PIC = Programmable Interrupt Controller**

### Example: Keyboard Interrupt
1. **Key Pressed** → Keyboard Controller Sends an Interrupt:
   - When a key is pressed, the keyboard sends a signal to the PIC (IRQ1).
2. **PIC Sends Interrupt to CPU**:
   - The PIC detects IRQ1 and sends an interrupt signal (Interrupt Vector 0x21) to the CPU’s IDT.
3. **CPU Handles the Interrupt via IDT**:
   - The CPU checks its IDT and jumps to the keyboard interrupt handler.
4. **Interrupt Handler Reads the Keyboard Port**:
   - The handler reads the scancode from I/O port 0x60.
5. **Key Processing & Acknowledgment**:
   - The handler processes the scancode and sends an **End of Interrupt (EOI)** signal to the PIC.

---

### System Calls:
- **Syscalls** are handled with interrupts. A specific interrupt code is assigned to each instruction.

In **real mode**, the CPU and BIOS communicate via interrupts, with the BIOS executing 16-bit code to handle hardware events and return data.

**BIOS is our friend.**

The BIOS has access to all CPU registers and can read them later. The BIOS offers different instructions organized in **instruction vectors**, accessed via **interrupt vectors** (IVT).

**Interrupt vectors**: A table of addresses pointing to BIOS routines.

In **protected mode**, the **IDT** replaces the **IVT** and defines handlers for exceptions, IRQs, and software interrupts.

**Instruction vector 0x10** is a BIOS interrupt service, and the values in **AH** (0x00 to 0x13) are different functions for managing text or graphic display.

### Example:
```assembly
mov ah, 0x0E
INT 0x10
```

The BIOS sends 16-bit instruction codes, so in higher bit modes, you cannot ask the BIOS to perform tasks.

---

### Hard Drive Disk (HDD):
A hard drive has multiple plates, with read/write heads between them. The plates rotate, and the heads act as electromagnets, flipping the polarization of regions to write data or detecting polarization to read data.

**Sector**: A 512-byte chunk of memory.  
**CHS** → **Cylinder-Head-Sector**:  
- **Cylinder**: A collection of tracks.
- **Head**: The read/write head.
- **Sector**: A specific section of a track.

In modern drives, **LBA (Logical Block Addressing)** has replaced CHS. Each sector is identified by a unique number (LBA address).

---

### Real Mode Addressing:
In real mode, there are only 16-bit registers, but the addressing bus is 20 bits. The **segment:offset** system is used to address 20 bits of memory.

**ADDR = (16 * Segment) + Offset**

- **DI** usually stores the offset.
- **ES** usually stores the segment but is generally not modified.

The maximum number of sectors is 128, each 512 bytes:  
128 * 512 = 65,536 bytes (64 KB).

---

### BIOS Interrupt 0x13:
This BIOS interrupt reads sectors from a disk into memory.

**Parameters**:
| Register |
|----------|
| `AH` = `0x02` |
| `AL` = Number of sectors to read (1–128) |
| `CH` = Cylinder number (0–1023, bits 0–7) |
| `CL` = Sector number (1–63, bits 0–5) + (Cylinder bits 8–9 in bits 6–7) |
| `DH` = Head number (0–255) |
| `DL` = Drive number (`0x00` = Floppy, `0x80` = First HDD) |
| `ES:BX` = Memory address to store the data |

If loading more than 64 KB, adjust **ES** accordingly.

---

# Global Descriptor Table (GDT) and Protected Mode

In **16-bit real mode**, we use a 20-bit bus, but with the release of 32-bit CPUs, a mechanism was needed to maintain backward compatibility. The solution was the **A20 line**, a gate that could be enabled or disabled to allow or block access to memory above 1 MB.

The CPU uses **FAST A20**:
1. Read from port `0x92` → `IN AL, 0x92`.
2. Flip the A20 bit → `OR AL, 2`.
3. Write to port `0x92` → `OUT 0x92, AL`.

### Logical Gates:

| **Operation** | **IN A** | **IN B** | **OUT** |
|--------------|------------|------------|-----------------|
| **AND**      | 0          | 0          | 0               |
|              | 0          | 1          | 0               |
|              | 1          | 0          | 0               |
|              | 1          | 1          | 1               |
| **OR**       | 0          | 0          | 0               |
|              | 0          | 1          | 1               |
|              | 1          | 0          | 1               |
|              | 1          | 1          | 1               |
| **XOR**      | 0          | 0          | 0               |
|              | 0          | 1          | 1               |
|              | 1          | 0          | 1               |
|              | 1          | 1          | 0               |
| **NOT**      | 0          | -          | 1               |
|              | 1          | -          | 0               |

---

### GDT (Global Descriptor Table):
The GDT is a table containing information about different memory segments, such as their base address, limit, and other attributes. It is essential for **protected mode**.

**GPF** = General Protection Fault.

The GDT can trigger a **GPF**.

### GDT Entries:
1. **NULL Descriptor**: 8 bytes of zeros (64 bits).
2. **Code/Data Descriptor**: Describes segments like base address, limit, etc.
3. **Task State Segment Descriptor (TSS)**: Used for multitasking in user space.
4. **GDTR (GDT Register)**: Points to the GDT.

---

### Task State Segment (TSS):
**TSS IS OPTIONAL**

The **TSS** is a memory structure that stores the state of a task (process or thread) in protected mode. The **TSS Descriptor (TSSD)** is an entry in the GDT that describes the location and size of the TSS.

The **TSS** is used by the CPU to:
- Manage task switching.
- Store the state of a process (registers, ESP, EIP, etc.).
- Define the stack for privilege transitions (Ring 3 → Ring 0).

### TSS in 32-bit Mode:

| **Field**          | **Size** | **Description** |
|--------------------|-----------|----------------|
| **Previous TSS**   | 4 bytes   | Used for linked task switching, otherwise 0. |
| **ESP0**           | 4 bytes   | Stack pointer for Ring 0. |
| **SS0**            | 4 bytes   | Stack segment for Ring 0. |
| **ESP1**           | 4 bytes   | Stack pointer for Ring 1 (rarely used). |
| **SS1**            | 4 bytes   | Stack segment for Ring 1. |
| **ESP2**           | 4 bytes   | Stack pointer for Ring 2. |
| **SS2**            | 4 bytes   | Stack segment for Ring 2. |
| **CR3**            | 4 bytes   | Page Directory Base Register (if paging is active). |
| **EIP**            | 4 bytes   | Instruction pointer. |
| **EFLAGS**         | 4 bytes   | CPU status register. |
| **EAX, ECX, ...**  | 4 bytes each | General-purpose registers. |
| **LDT Selector**   | 4 bytes   | Selector for the Local Descriptor Table (LDT). |
| **Trap Flag**      | 2 bytes   | Triggers an interrupt after task switching (debug). |
| **IO Map Base**    | 2 bytes   | Start of the I/O permission bitmap. |

---

### Segment Descriptor

A **Segment Descriptor** is **8 bytes long** (64 bits) and is divided into multiple fields.

### **Diagram of a Segment Descriptor (64 bits)**  

![GDT](GDT.png)

- **Base Address**: A 32-bit value representing the base address where the segment begins.
- **Limit**: A 20-bit value that indicates the size of the segment. It can either represent the size in byte units or 4 KiB pages, depending on the granularity bit.
- **Access Byte**: Contains several flags related to the segment’s properties like presence, privilege level, and type.
- **Flags**: Includes additional information such as the granularity, segment size, and mode.

### Segment Descriptor Table

| Field             | Size (bits) | Example Value | Description |
|-------------------|-------------|---------------|-------------|
| **Base Address**   | 32          | `0x00000000`  | The linear address where the segment starts |
| **Limit**          | 20          | `0xFFFFF`     | Maximum size of the segment in either byte or 4 KiB pages |
| **Access Byte**    | 8           | `0x9A` (Code) / `0x92` (Data) | Defines the segment's type and its permissions |
| **Flags**          | 4           | `0xC` (G=1, D=1) | Defines the granularity and segment size |

---

### Access Byte Breakdown

![GDT](GDT2.png)

The **Access Byte** is divided into several bits that control the properties of the segment. Here’s the breakdown:

| Bit | Name | Description |
|-----|------|-------------|
| **7** | **P (Present)** | Indicates if the segment is valid and in memory (1 = Present, 0 = Absent) |
| **6-5** | **DPL (Descriptor Privilege Level)** | Specifies the privilege level of the segment. 0 = Kernel, 3 = User |
| **4** | **S (Segment Type)** | 1 = Code/Data Segment, 0 = System Segment |
| **3** | **E (Executable)** | 1 = Code Segment, 0 = Data Segment |
| **2** | **DC (Direction/Conforming)** | For Data segments: 0 = grows up, 1 = grows down. For Code segments: Defines privilege level execution |
| **1** | **RW (Read/Write)** | 1 = Readable for Code, Writable for Data |
| **0** | **A (Accessed)** | Set by the CPU when the segment is accessed |

---

### Flags Breakdown

![GDT](GDT3.png)

The **Flags** in the Segment Descriptor provide further characteristics of the segment, specifically regarding its granularity, size, and mode.

| Bit | Name | Description |
|-----|------|-------------|
| **3** | **G (Granularity)** | If 1, the Limit is in 4 KiB blocks (page granularity). If 0, the Limit is in 1-byte blocks (byte granularity). |
| **2** | **DB (Descriptor Size)** | If 1, the segment is 32-bit. If 0, the segment is 16-bit. |
| **1** | **L (Long Mode)** | If 1, the segment is for 64-bit code. |
| **0** | **Reserved** | Reserved for future use, typically left as 0 |

---


# IDT - Interrupt Descriptor Table

## Introduction

An **interrupt** is a signal that tells the CPU to stop whatever it's doing, handle that interrupt, and then return to its previous state.

When an interrupt occurs, the CPU executes an **ISR (Interrupt Service Routine)**. We need to configure the CPU to know how it should react to these interrupts and which ISR should handle them.

## IDT Entry Structure

Each entry in the IDT (Interrupt Descriptor Table) consists of the following fields:

- **2 bytes** - Offset Low
- **2 bytes** - GDT Code Selector (usually 8)
- **1 byte**  - IST (Interrupt Stack Table), could be 0
- **1 byte**  - Attributes/Flags:
  - **Present bit (1 bit)**: Indicates if the interrupt is active.
  - **DPL (Desired Privilege Level) (2 bits)**: Specifies the privilege level (Ring 0-3).
  - **Reserved bit (1 bit)**
  - **Type (4 bits)**:
    - **Interrupt Gate** = `1110`
    - **Trap Gate** = `1111`
- **2 bytes** - Offset Mid
- **4 bytes** - Offset High
- **4 bytes** - Reserved/Zero

All **256 entries** correspond to a specific interrupt.

### Attributes Example

If we want to set an attribute, we can use a magic number like:

```
1000 1110 = 0x8E
```

This value represents an **Interrupt Gate** with **DPL 0** and **Present bit set**.

## IDTR - Interrupt Descriptor Table Register

The **IDTR (Interrupt Descriptor Table Register)** stores the size and address of the IDT.

### IDT Size Calculation

```
Size of IDT = 16 bytes * 256 = 4096 - 1 bytes (0xFFF)
```

The IDTR always holds **two values**:
1. **Size of the IDT**
2. **Address of the IDT**

## Loading the IDT

To load the IDT, we use the `LIDT` instruction, providing it with the IDT address. After loading, we must **enable interrupts** using the `STI` instruction.

## Handling an Interrupt

When an interrupt is triggered, we must restore the CPU state after handling it. The CPU state is represented by the **values of all its registers**.

### Steps to Handle an Interrupt:
1. Push all necessary registers onto the stack.
2. Save the stack pointer.
3. Call the C handler.
4. Use registers if needed.
5. Restore the CPU state.

## Interrupt Controllers

### 8259 PIC (Programmable Interrupt Controller)

The **8259 PIC** is a legacy interrupt controller, but it is not widely used today. Modern systems have replaced it with **APIC (Advanced Programmable Interrupt Controller)**.

# APIC (Advanced Programmable Interrupt Controller)

APIC is the modern replacement for the legacy PIC and provides better interrupt management, especially in multi-core processors. 8259A Legacy Mode.

- Master PIC
- Slaver PIC

We communicate to those PIC through the I/O bus (in out)
What are the ports of the Master PIC and Slave PIC ?

```
PIC    | COMMAND | DATA
-----------------------
MASTER |  0x20   | 0x21
SLAVE  |  0xA0   | 0xA1
```

### What Information Should We Send to the PIC?

- **Remapping the PIC**

### Why Is Remapping Necessary?

The IDT has **256 entries**, and the first **16 entries** are reserved for **IRQ(Interrupt Request) channels**. The CPU uses these IRQs for handling hardware requests. By default, the legacy PIC maps IRQ 0-7 to IDT entries **0x08-0x0F** and IRQ 8-15 to **0x70-0x77**. However, the first 32 entries (0x00-0x1F) are **reserved for CPU exceptions**.

To avoid conflicts, we **remap the PIC** so that IRQs start from **IDT entry 0x20 (32)** for IRQ 0-7 and **IDT entry 0x28 (40)** for IRQ 8-15. This ensures that the first 32 IDT entries remain reserved for exceptions.

```
   0 Timer            8 Real Time Clock
   1 Keyboard         9 General I/O
   2 PIC 2           10 General I/O
   3 COM 2           11 General I/O
   4 COM 1           12 General I/O
   5 LPT 2           13 Coprocessor
   6 Floppy disk     14 IDE Bus
   7 LPT 1           15 IDE Bus
```

The first thing we want to do is to send reinitialize code to both master and slave PIC. 
Then we want to input some new data for remapping the PIC.
We have to specify which IRQs  will be handled by which programmable interrupt controller.
**One programmable interrupt controller can handle 8 IRQs at the time**

The most standard way to manage this is to let the Master PIC have everything from 0 to 7 and the Slaver PIC have from 8 to 15.

To the master we have to send the IRQ because one of the IRQs of the master has to be reserved by a slave that's why it's called the slave.

### **Connection Between the Master PIC and the Slave PIC**  

The **8259 PIC (Programmable Interrupt Controller)** architecture uses **two PIC controllers** to manage up to **16 IRQs**:  
- **The Master PIC** (handles IRQs 0 to 7)  
- **The Slave PIC** (handles IRQs 8 to 15)  

However, the **CPU can only communicate with the Master PIC**. So, for the CPU to receive interrupts from the **Slave PIC**, they must be routed **through the Master PIC**.  

### **Why Is One IRQ of the Master Reserved for the Slave?**  

The **Master PIC** and the **Slave PIC** must be connected to work together. This connection is established through **IRQ2 of the Master PIC**.  

- **IRQ2 of the Master PIC** is reserved to receive interrupts from the **Slave PIC**.  
- When a device connected to the **Slave PIC** triggers an interrupt (e.g., a mouse on **IRQ12**), the **Slave PIC** sends this interrupt through **IRQ2 of the Master PIC**.  
- The **Master PIC** then notifies the CPU that **IRQ2** has been activated, and the CPU looks up the corresponding **Interrupt Service Routine (ISR)** in the IDT.   

To avoid conflicts, we **remap the PIC IRQs** starting from **interrupt 32 (0x20 in hexadecimal)**.  

After remapping:  
- **IRQ 0 - 7 (Master PIC) → Interrupts 32 - 39**  
- **IRQ 8 - 15 (Slave PIC) → Interrupts 40 - 47**  

This means that:  
- **IRQ2 of the Master PIC** corresponds to **interrupt 34 (0x22)** and is still used to receive interrupts from the **Slave PIC**.  

- Important things : 

Why send 4 instead of 2 to the Master PIC (IRQ2 is number 2 not 4).


   - In binary, **IRQ2** is represented as `00000010` (`0x2`).  
   - However, when configuring the Master PIC with **ICW3 (Initialization Command Word 3)**, we don’t provide the IRQ number directly.  
   - Instead, we use a **bitmask** to indicate **which IRQ line is reserved for the Slave PIC**.  
   - **Each bit in the value corresponds to an IRQ line**:
     - Bit `0` → IRQ0  
     - Bit `1` → IRQ1  
     - Bit `2` → IRQ2  
     - Bit `3` → IRQ3  
     - etc.  

   - Since the Slave is connected to **IRQ2**, we **set bit 2 to `1`**, which results in `00000100` (`0x4`).  

### **Why Does the Slave Receive `2` Instead of `4`?**  
For the **Slave PIC**, ICW3 is used to define **its identification in the cascade setup**.  
- Since the Slave is **connected through IRQ2** of the Master, it identifies itself with **ID `2`**.  
- That’s why we send **`2` (`0x2`)** directly to the Slave instead of using a bitmask.  

- To the **Master PIC**, we send `4` (`0b00000100`) → **Bitmask** indicating that the Slave is connected via **IRQ2**.  
- To the **Slave PIC**, we send `2` (`0x2`) → **Its cascade identification number**.  



```
Send to both Master and Slave command 0x11 to reinit their values.
Then send data 0 to the Master and 8 for the Slave (Means they will handle respectively from 0-7 and 8-15).
Then send data 4 and 2 (bitmask and cascade)
Then send data 1 and 1 (8086 CPU mode)
Last we want to mask all the interruptions and only unmask the IRQ that we actually need.
If we want to mask all the IRQs we just simply send data 0xFF to both Master and Slave port.

         Master         |      Slave
Bits : 1 1 1 1 1 1 1 1  | 1 1  1  1  1  1  1  1
IRQs : 0 1 2 3 4 5 6 7  | 8 9 10 11 12 13 14 15

Finally we send an End of Interrupt (EOI) signal, we write the 0x20 command to the 0x20 command port of the Master PIC.
```


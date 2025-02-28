
---

# 1. What is an OS?

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

# 2. Booting

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
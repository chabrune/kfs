#pragma once

#include "kernel.h"

typedef struct
{
    uint16_t offset_low;
    uint16_t code_selector;
    uint8_t  reserved;
    uint8_t  flags;
    uint16_t offset_high; 
} __attribute__((packed)) IDTEntry ;

typedef struct
{
    uint16_t    limit;
    uint32_t    base;
} __attribute__((packed)) IDTR ;

typedef enum {
    IDT_FLAG_GATE_32BIT_INT         = 0xE,
    IDT_FLAG_GATE_32BIT_TRAP        = 0xF,

    IDT_FLAG_RING0                  = (0 << 5),
    IDT_FLAG_RING3                  = (3 << 5),

    IDT_FLAG_PRESENT                = 0x80,
} IDT_FLAGS;

extern void         load_IDT(IDTR idtr);
extern void         keyboard_ISR();
void                keyboard_handler();
void                setGate(uint32_t handler, uint16_t codeSelect, uint8_t flags, int interrupt);
void                remap_pic(void);
void                init_idt();

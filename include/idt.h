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

extern void     outb(uint16_t port, uint8_t val);
extern uint8_t  inb(uint16_t port);
extern void     load_IDT(IDTR *idtr);


typedef enum {
    // tap gate contain address of current instruction (so it can be retried)
    // interrupt contain adress of next instruction
    IDT_FLAG_GATE_32BIT_INT         = 0xE,
    IDT_FLAG_GATE_32BIT_TRAP        = 0xF,

    // rings are privileges levels
    IDT_FLAG_RING0                  = (0 << 5),
    IDT_FLAG_RING3                  = (3 << 5),

    IDT_FLAG_PRESENT                = 0x80,
} IDT_FLAGS;
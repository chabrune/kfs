#pragma once

#include "kernel.h"

typedef struct {
    uint16_t offset_low;   // Offset bits 0-15
    uint16_t selector;     // Segment selector (GDT)
    uint8_t ist;           // Interrupt Stack Table (0 si pas utilisé)
    uint8_t flags;     // Flags et type d'entrée
    uint16_t offset_mid;   // Offset bits 16-31
    uint32_t zero;         // Réservé
} __attribute__((packed)) IDTEntry;



typedef struct {
    uint16_t Limit;
    uint32_t base;
} __attribute__((packed)) IDTDescriptor;

typedef enum {
    // tap gate contain address of current instruction (so it can be retried)
    // interrupt contain adress of next instruction
    IDT_FLAG_GATE_TASK              = 0x5,
    IDT_FLAG_GATE_16BIT_INT         = 0x6,
    IDT_FLAG_GATE_16BIT_TRAP        = 0x7,
    IDT_FLAG_GATE_32BIT_INT         = 0xE,
    IDT_FLAG_GATE_32BIT_TRAP        = 0xF,

    // rings are privileges levels
    IDT_FLAG_RING0                  = (0 << 5),
    IDT_FLAG_RING1                  = (1 << 5),
    IDT_FLAG_RING2                  = (2 << 5),
    IDT_FLAG_RING3                  = (3 << 5),

    IDT_FLAG_PRESENT                = 0x80,
} IDT_FLAGS;

void IDT_initialize();
void IDT_SetGate(int interrupt, void *base, uint16_t segmentDescriptor, uint8_t flags);
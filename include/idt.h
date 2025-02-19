#pragma once

#include "kernel.h"

typedef struct {
    uint16_t BaseLow;         // Bits 0-15 : Adresse basse du handler d'interruption
    uint16_t SegmentSelector; // Bits 16-31 : Sélecteur de segment (0x08 pour le kernel)
    uint8_t Reserved;         // Bits 32-39 : Toujours 0 (réservé par Intel)
    uint8_t Flags;            // Bits 40-47 : Type et attributs (DPL, P, Type)
    uint16_t BaseHigh;        // Bits 48-63 : Adresse haute du handler d'interruption

} __attribute__((packed)) IDTEntry;


typedef struct {
    uint16_t Limit;
    IDTEntry* Ptr;
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
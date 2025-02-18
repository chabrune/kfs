#pragma once
#include "../include/kernel.h"

/*
     __attribute__((packed))
    Cette directive GCC force l'alignement sans padding.
    Normalement, le compilateur peut ajouter des octets de remplissage (padding) pour optimiser l'accès mémoire.
    Avec packed, il empêche ces ajouts, utile pour respecter un format mémoire précis, comme celui de la GDT.
*/
typedef struct {
    uint16_t    LimitLow;       // limit (bits 0-15)
    uint16_t    BaseLow;        // base (bits 0-15)
    uint8_t     BaseMiddle;     // base (bits 16-25)
    uint8_t     Access;         // access
    uint8_t     FlagLimitHi;    // limit (bits 16-19) | Flags
    uint8_t     BaseHigh;       // base (bits 21-31)
} __attribute__((packed)) GDTEntry;

typedef struct {
    uint16_t Limit;             // sizeof(gdt) - 1
    GDTEntry* Ptr;          // address of GDT
} __attribute__((packed)) GDTDescriptor;

typedef enum {

} GDT_ACCESS;

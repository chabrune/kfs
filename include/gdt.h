#pragma once

#include "kernel.h"
#define GDTBASE    0x00000800

typedef struct
{
    uint16_t    limit;
    uint32_t    base;
} __attribute__((packed)) GDTR;

typedef struct
{
    uint16_t limit16;
    uint16_t base16f;
    uint8_t base8s;
    uint8_t access;
    uint8_t limit4:4;
    uint8_t flags4:4;
    uint8_t base8t;
}__attribute__((packed)) GDTEntry;

typedef enum {
    GRANULARITY        = (1 << 3), // 1 = Limit en blocs de 4 KiB, 0 = Limit en octets
    DESCRIPTOR_SIZE_32 = (1 << 2), // 1 = Segment 32-bit, 0 = Segment 16-bit
    LONG_MODE          = (0 << 1), // 1 = Mode 64-bit (doit être 0 pour 32-bit)
    RESERVED           = (0 << 0), // Réservé, toujours à 0
} GDT_FLAGS;

typedef enum {
    PRESENT             = (1 << 7), // Le segment est présent en mémoire
    ABSENT              = (0 << 7), // Le segment est absent (cause une #GPF si accédé)

    DPL_USER            = (3 << 5), // Ring 3 (Privilège utilisateur)
    DPL_KERNEL          = (0 << 5), // Ring 0 (Privilège kernel)

    S_CODE_DATA_SEGMENT = (1 << 4), // 1 = Code/Data segment, 0 = Système (TSS, LDT)
    S_SYSTEM_SEGMENT    = (0 << 4), // Système (LDT, TSS...)

    E_CODE              = (1 << 3), // 1 = Segment de code
    E_DATA              = (0 << 3), // 0 = Segment de données

    // Pour les segments de données (Direction/Conforming bit)
    DC_DATA_UP          = (0 << 2), // Segment de données croissant vers le haut (normal)
    DC_DATA_DOWN        = (1 << 2), // Segment de données croissant vers le bas

    // Pour les segments de code (Direction/Conforming bit)
    DC_CODE_SAME_RING   = (0 << 2), // Exécution seulement au même DPL
    DC_CODE_INFERIOR_RING = (1 << 2), // Exécution autorisée à un niveau inférieur

    // Bit Read/Write
    RW_SEG_DATA_READ_ONLY  = (0 << 1), // Lecture seule (Data)
    RW_SEG_DATA_READ_WRITE = (1 << 1), // Lecture/écriture autorisée (Data)
    RW_SEG_CODE_EXEC_ONLY  = (0 << 1), // Exécution uniquement (Code)
    RW_SEG_CODE_READ_ACTIVATED = (1 << 1), // Lecture autorisée (Code)

    ACCESSED = (1 << 0), // 1 = Segment déjà accédé (bit modifié par le CPU)
} GDT_ACCESS;


extern void load_GDT(GDTR gdtr);
void set_GDT_Gate(GDTEntry *gdtptr, uint32_t base, uint32_t limite, uint8_t access, uint8_t flags);
void init_gdt();
extern void reloadSegments();
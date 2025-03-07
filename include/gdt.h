#pragma once

#include "kernel.h"

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
    uint8_t limit4;
    uint8_t flags;
    uint8_t base8t;
}__attribute__((packed)) GDTEntry;

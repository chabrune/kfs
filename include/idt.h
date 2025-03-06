#pragma once

#include "kernel.h"

typedef struct
{
    uint16_t offset_low;
    uint16_t code_selector;
    uint8_t  flags;
    uint16_t offset_high; 
} __attribute__((packed)) IDTEntry ;

typedef struct
{
    uint16_t    limit;
    IDTEntry    *ptr; 
} __attribute__((packed)) IDTR ;

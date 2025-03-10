#pragma once

typedef unsigned char      uint8_t;
typedef unsigned short     uint16_t;
typedef unsigned int       uint32_t;
typedef unsigned long long uint64_t;

#include "lib.h"
#include "idt.h"
#include "vga.h"
#include "gdt.h"

void                init (void);
void                kmain (void);

void    outb(uint32_t port, uint8_t value);
uint8_t inb(uint32_t port);

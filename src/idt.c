#include "../include/kernel.h"
#include "../include/idt.h"
#include "../include/lib.h"

IDTEntry g_IDT[256];
IDTDescriptor g_IDTDescriptor = {sizeof(g_IDT) - 1, g_IDT};

extern void __attribute__((cdecl)) IDT_LOAD(IDTDescriptor* idtDescriptor);
extern void __attribute__((cdecl)) keyboard_handler();

void IDT_SetGate(int interrupt, void *base, uint16_t segmentDescriptor, uint8_t flags)
{
    g_IDT[interrupt].BaseLow = ((uint32_t)base) & 0xFFFF;
    g_IDT[interrupt].SegmentSelector = segmentDescriptor;
    g_IDT[interrupt].Reserved = 0;
    g_IDT[interrupt].Flags = flags;
    g_IDT[interrupt].BaseHigh = ((uint32_t)base >> 16) & 0xFFFF; 
}

void IDT_initialize()
{
    IDT_LOAD(&g_IDTDescriptor);
    IDT_SetGate(33, &keyboard_handler, 0x08, IDT_FLAG_PRESENT | IDT_FLAG_RING0 | IDT_FLAG_GATE_32BIT_INT);
    // ft_printk("%d", g_IDTDescriptor.Ptr);
}
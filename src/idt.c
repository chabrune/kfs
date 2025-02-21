#include "../include/kernel.h"
#include "../include/idt.h"
#include "../include/lib.h"

void test()
{
    putc('l');
}
IDTEntry g_IDT[256];
IDTDescriptor g_IDTDescriptor = {sizeof(g_IDT) - 1, g_IDT};

extern void __attribute__((cdecl)) IDT_LOAD(IDTDescriptor* idtDescriptor);

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
    putc('4');
    ft_printk("%s", "COUCOU");
    IDT_LOAD(&g_IDTDescriptor);
    ft_printk("%s", "COUCOU");
}
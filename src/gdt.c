#include "../include/kernel.h"

GDTEntry g_GDT[8];
GDTR g_GDTR;

void init_gdt() {

    // NULL SEGMENT
    set_GDT_Gate(&g_GDT[0], 0x0, 0x0, 0x0, 0x0);

    // CODE KERNEL SEGMENT
    set_GDT_Gate(&g_GDT[1], 0x0, 0xFFFFF, PRESENT | DPL_KERNEL | S_CODE_DATA_SEGMENT | E_CODE | DC_CODE_SAME_RING | RW_SEG_CODE_READ_ACTIVATED | ACCESSED, GRANULARITY | DESCRIPTOR_SIZE_32);

    // DATA KERNEL SEGMENT
    set_GDT_Gate(&g_GDT[2], 0x0, 0xFFFFF, PRESENT | DPL_KERNEL | S_CODE_DATA_SEGMENT | E_DATA | DC_DATA_UP | RW_SEG_DATA_READ_WRITE | ACCESSED, GRANULARITY | DESCRIPTOR_SIZE_32);

    // CODE USER SEGMENT
    set_GDT_Gate(&g_GDT[3], 0x0, 0xFFFFF, PRESENT | DPL_USER | S_CODE_DATA_SEGMENT | E_CODE | DC_CODE_SAME_RING | RW_SEG_CODE_READ_ACTIVATED | ACCESSED, GRANULARITY | DESCRIPTOR_SIZE_32);

    // DATA USER SEGMENT
    set_GDT_Gate(&g_GDT[4], 0x0, 0xFFFFF, PRESENT | DPL_USER | S_CODE_DATA_SEGMENT | E_DATA | DC_CODE_SAME_RING | RW_SEG_CODE_READ_ACTIVATED | ACCESSED, GRANULARITY | DESCRIPTOR_SIZE_32);

    g_GDTR.base = GDTBASE;
    g_GDTR.limit = sizeof(g_GDT);

    ft_memcpy((char*)g_GDTR.base, (char*)g_GDT, g_GDTR.limit);
    load_GDT(g_GDTR);
    reloadSegments();
}

void set_GDT_Gate(GDTEntry *gdtptr, uint32_t base, uint32_t limite, uint8_t access, uint8_t flags)
{
    gdtptr->access = access;
    gdtptr->flags4 = flags & 0xf;
    gdtptr->base16f = base & 0xffff;
    gdtptr->limit16 = limite & 0xffff;
    gdtptr->base8s = (base >> 16) & 0xff;
    gdtptr->base8t = (base >> 24) & 0xff;
    gdtptr->limit4 = (limite >> 16) & 0xf;
}


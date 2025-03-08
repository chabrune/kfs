#include "../include/kernel.h"

IDTEntry g_IDT[256];
IDTR g_IDTR;

void remap_pic(void)
{
    outb(0x20, 0x11);
    outb(0xA0, 0x11);

    outb(0x21, 0x20);
    outb(0xA1, 0x28);

    outb(0x21, 0x2);
    outb(0xA1, 0x4);

    outb(0x21, 0x1);
    outb(0xA1, 0x1);

    outb(0x21, 0xFD);
    outb(0xA1, 0xFF);
}

void init_idt() {
    g_IDTR.base = (uint32_t)&g_IDT;
    g_IDTR.limit = sizeof(g_IDT) - 1;

    setGate((uint32_t)keyboard_ISR, 0x8, IDT_FLAG_PRESENT | IDT_FLAG_RING0 | IDT_FLAG_GATE_32BIT_INT, 0x21);
    load_IDT(g_IDTR);
}

void keyboard_handler()
{
    uint8_t scancode = inb(0x60);
    char c = 0;

    static char scancode_to_char[128] = {
        0,  27, '1','2','3','4','5','6','7','8','9','0','-','=','\b',
        '\t', 'q','w','e','r','t','y','u','i','o','p','[',']','\n',
        0, 'a','s','d','f','g','h','j','k','l',';','\'','`',
        0, '\\','z','x','c','v','b','n','m',',','.','/','\0',
        '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '-', 0, 0, 0, '+',
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };

    if (scancode & 0x80)
        return;
    c = scancode_to_char[scancode];

    if (c) {
        putc(c);
    }
    outb(0x20, 0x20);
}

void setGate(uint32_t handler, uint16_t codeSelect, uint8_t flags, int interrupt)
{
    g_IDT[interrupt].offset_low = (handler & 0xFFFF); 
    //   0x12345678 =   0001 0010 0011 0100 0101 0110 0111 1000
    //         &        0000 0000 0000 0000 1111 1111 1111 1111
    g_IDT[interrupt].code_selector = codeSelect;
    g_IDT[interrupt].reserved = 0;
    g_IDT[interrupt].flags = flags;
    g_IDT[interrupt].offset_high = (handler >> 16) & 0xFFFF; 
}

#include "../include/kernel.h"
#include "../include/lib.h"
#include "../include/idt.h"

void enable_cursor(uint8_t cursor_start, uint8_t cursor_end);

volatile uint16_t *video = (uint16_t*)VIDEO;
int xpos;
int ypos;
uint8_t text_color = VGA_COLOR(VGA_LIGHT_RED, VGA_BLACK);

void init(void)
{
    enable_cursor(0, 15);
    int i;
    for (i = 0; i < COLUMNS * LINES; i++)
    {
        video[i] = ((uint16_t)text_color << 8) | ' ';
    }
    xpos = 0;
    ypos = 0;
}

/*

00000000 00000000
\______/\______/
   ↑        ↑
 Color    Caractère ASCII
*/
void putc(char c)
{
    switch (c)
    {
        case '\n':
            xpos = 0;
            ypos++;
            break;
        default:
        {
            const uint32_t index = COLUMNS * ypos + xpos;
            video[index] = ((uint16_t)text_color << 8) | c;
            xpos++;
            break;
        }
    }

    if (xpos >= COLUMNS)
    {
        xpos = 0;
        ypos++;
    }
    if (ypos >= LINES)
    {
        xpos = 0;
        ypos = 0;
    }
    set_cursor(xpos, ypos);
}

void set_cursor(int x, int y) {
    uint16_t pos = y * COLUMNS + x;
    outb(0x3D4, 0x0E);
    outb(0x3D5, (pos >> 8));
    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)pos);
}

void enable_cursor(uint8_t cursor_start, uint8_t cursor_end)
{
    outb(0x3D4, 0x0A);
    outb(0x3D5, (inb(0x3D5) & 0xC0) | cursor_start);
    outb(0x3D4, 0x0B);
    outb(0x3D5, (inb(0x3D5) & 0xE0) | cursor_end);
}

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

void puts(const char *s)
{
    uint32_t i = 0;
    while(s[i] != '\0')
    {
        putc(s[i]);
        i++;
    }
}

IDTEntry g_IDT[256];
IDTR g_IDTR;


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

void keyboard_handler()
{
    uint8_t scancode = inb(0x60);
    char c = ' ';
    switch (scancode) {
        case 0x1E: c = 'a'; break;
        case 0x30: c = 'b'; break;
        case 0x2E: c = 'c'; break;
        case 0x1C: c = '\n'; break;
        default: return;
    }
    putc(c);
    outb(0x20, 0x20);
} 

// void init_idt_entry(uint16_t selector, uint32_t offset, uint8_t type, idt_entry_t * entry) {
//     entry->isr_offset_low = (offset & 0xFFFF); // extract low part of uint32 to uint16
//     entry->selector = selector;
//     entry->zero = 0;
//     entry->type = type;
//     entry->isr_offset_high = (offset & 0xFFFF0000) >> 16; // high part

// }

void init_idt() {
    g_IDTR.base = (uint32_t)&g_IDT;
    g_IDTR.limit = sizeof(g_IDT) - 1;

    setGate((uint32_t)keyboard_ISR, 0x8, IDT_FLAG_PRESENT | IDT_FLAG_RING0 | IDT_FLAG_GATE_32BIT_INT, 0x21);

    load_IDT(g_IDTR);
    // asm("lidt (%0)" : : "r" (&g_IDTR));

    // asm("lidtl (g_IDTR)");
}

void kmain(void)
{
    init_idt();
    init();
    remap_pic();
    puts("FuckOs>$");
    // debug_IDT(0x21);
    asm volatile(
        "sti"
    );
    while(1);
}

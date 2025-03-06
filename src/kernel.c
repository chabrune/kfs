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
/*
__asm__ ("instruction" : output_operands : input_operands : clobbers);
constraint modifiers : 
a = specify eax
N = int 0-255 ? 
d = specify edx
*/
// static inline void outb(uint16_t port, uint8_t val)
// {
//     __asm__ volatile (
//         "out dx, al"
//         :
//         : "a"(val), "Nd"(port)
//     );
// }

/*
    Lit le port DX et met la valeur dans AL
    La sortie (AL) est assignée à 'ret'
    Le port est passé en entrée
*/
static inline uint8_t inb(uint16_t port)
{
    uint8_t ret;
    __asm__ volatile (
        "in al, dx"
        : "=a"(ret)
        : "Nd"(port)
    );
    return ret;
}

void    remap_pic(void)
{
    outb(0x20, 0x11);
    outb(0xA0, 0x11);

    outb(0x21, 0x20);
    outb(0xA1, 0x28);

    outb(0x21, 0x2);
    outb(0xA1, 0x4);

    outb(0x21, 0x1);
    outb(0xA1, 0x1);

    outb(0x21, 0xFF);
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

IDTEntry g_IDT[256] = {0, 0, 0, 0};
IDTR g_IDTR;

void init_idtr() {
    g_IDTR.limit = sizeof(g_IDT) - 1;
    g_IDTR.base = (uint32_t) &g_IDT;
}

extern void load_IDT(IDTR *idtr);

void    setGate(void *handler, uint16_t codeSelect, uint8_t flags, int interrupt)
{
    g_IDT[interrupt].offset_low = ((uint32_t)handler) & 0xFFFF; 
    //   0x12345678 =   0001 0010 0011 0100 0101 0110 0111 1000
    //         &        0000 0000 0000 0000 1111 1111 1111 1111
    g_IDT[interrupt].code_selector = codeSelect;
    g_IDT[interrupt].offset_high = ((uint32_t)handler >> 16) & 0xFFFF; 
    g_IDT[interrupt].flags = flags;
}

void kmain(void)
{
    remap_pic();
    init_idtr();
    load_IDT(&g_IDTR);
    init();
    puts("FuckOs>$");
    asm __inline__ ("sti");
    while(1);
}

#include "../include/kernel.h"
#include "../include/lib.h"

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
}

/*
__asm__ ("instruction" : output_operands : input_operands : clobbers);
constraint modifiers : 
a = specify eax
N = int 0-255 ? 
d = specify edx
*/
static inline void outb(uint16_t port, uint8_t val)
{
    __asm__ volatile (
        "out dx, al"
        :
        : "a"(val), "Nd"(port)
    );
}

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

void    setGate(void *handler, uint16_t codeSelect, uint8_t flags, )
{

}

void kmain(void)
{
    remap_pic();
    init();
    puts("FuckOs>$");
    while(1);
}

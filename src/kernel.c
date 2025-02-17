#include "../include/kernel.h"

volatile uint16_t *video = (uint16_t*)VIDEO;
int xpos;
int ypos;
uint8_t text_color = VGA_COLOR(VGA_RED, VGA_BLACK);


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

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile (
        "out dx, al"   // Utilise les registres dx (port) et al (valeur)
        :               // Pas de sortie
        : "a"(val), "Nd"(port)  // Entrées : "a" pour val (AL), "Nd" pour port (DX)
    );
}

void set_cursor(int x, int y) {
    uint16_t pos = y * COLUMNS + x;

    outb(0x3D4, 0x0E);
    outb(0x3D5, (pos >> 8) & 0xFF);

    outb(0x3D4, 0x0F);
    outb(0x3D5, pos & 0xFF);
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

void kmain(void)
{
    init();
    puts("42");
    set_cursor(2,0);
}

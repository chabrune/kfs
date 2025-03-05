#include "../include/kernel.h"
#include "../include/idt.h"
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
    enable_cursor(0, 15);
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

void enable_cursor(uint8_t cursor_start, uint8_t cursor_end) {
    outb(0x3D4, 0x0A);
    outb(0x3D5, (inb(0x3D5) & 0xC0) | cursor_start);

    outb(0x3D4, 0x0B);
    outb(0x3D5, (inb(0x3D5) & 0xE0) | cursor_end);
}
/*  x3D4 : Registre d'index pour le contrôleur VGA.
    0x0E : Indique au contrôleur VGA que nous voulons écrire dans le registre de la partie haute de la position du curseur.
    (pos >> 8) : Extrait les 8 bits de poids fort de pos (la partie haute).
*/
void set_cursor(int x, int y) {
    uint16_t pos = y * COLUMNS + x;

    outb(0x3D4, 0x0E);
    outb(0x3D5, (pos >> 8));

    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)pos);
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

void keyboard_callback(uint8_t scancode)
{
    char c = ' ';
    switch (scancode) {
        case 0x1E: c = 'a'; break;
        case 0x30: c = 'b'; break;
        case 0x2E: c = 'c'; break;
        case 0x1C: c = '\n'; break;
        default: return;
    }
    putc(c);
}


void kmain(void)
{
    init();
    puts("FuckOs>$");
    IDT_initialize();
    // __asm__ volatile ("sti");
}

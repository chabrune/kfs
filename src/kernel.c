#include "../include/kernel.h"

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

static inline uint8_t inb(uint16_t port)
{
    uint8_t ret;
    __asm__ volatile (
        "in al, dx"    // Lit le port DX et met la valeur dans AL
        : "=a"(ret)     // La sortie (AL) est assignée à 'ret'
        : "Nd"(port)     // Le port est passé en entrée (peut être dans n'importe quel registre)
    );
    return ret;
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
    puts("FuckOs>$");
    ft_printk("%d", inb(0x64));
}

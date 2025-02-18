#ifndef KERNEL__H
# define KERNEL__H

# define VIDEO 0xB8000
# define COLUMNS 80 // x
# define LINES 24 // y
# define MULTIBOOT_HEADER_MAGIC 0x1BADB002
# define VGA_COLOR(fg, bg) ((bg << 4) | (fg))

typedef unsigned char      uint8_t;
typedef unsigned short     uint16_t;
typedef unsigned int       uint32_t;
typedef unsigned long long uint64_t;

typedef signed char        int8_t;
typedef signed short       int16_t;
typedef signed int         int32_t;
typedef signed long long   int64_t;

# define VGA_BLACK 0x0
# define VGA_BLUE 0x1
# define VGA_GREEN 0x2
# define VGA_CYAN 0x3
# define VGA_RED 0x4
# define VGA_MAGENTA 0x5
# define VGA_BROWN 0x6
# define VGA_LIGHT_GREY 0x7
# define VGA_DARK_GREY 0x8
# define VGA_LIGHT_BLUE 0x9
# define VGA_LIGHT_GREEN 0xA
# define VGA_LIGHT_CYAN 0xB
# define VGA_LIGHT_RED 0xC
# define VGA_LIGHT_MAGENTA 0xD
# define VGA_LIGHT_BROWN 0xE
# define VGA_WHITE 0xF


void init (void);
void kmain (void);
static inline void outb(uint16_t port, uint8_t val);
void set_cursor(int x, int y);

#endif
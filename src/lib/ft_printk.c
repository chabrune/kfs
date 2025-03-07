#include "../../include/lib.h"

char *ft_itoa_base(uint32_t nb, int base) 
{
    static char buffer[32]; 
    char *digits = "0123456789abcdef";
    int i = 30;

    buffer[31] = '\0';

    if (nb == 0) {
        buffer[i] = '0';
        return &buffer[i];
    }

    while (nb > 0 && i) {
        buffer[i--] = digits[nb % base];
        nb /= base;
    }

    return &buffer[i + 1];
}


void ft_printk(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    while(*fmt)
    {
        if(*fmt == '%' && *(fmt + 1))
        {
            fmt++;
            switch(*fmt)
            {
                case 'c':
                {
                    char c = va_arg(args, int);
                    putc(c);
                    break;
                }
                case 's':
                {
                    char *str = va_arg(args, char*);
                    puts(str);
                    break;
                }
                case 'd':
                {
                    int nb = va_arg(args, int);
                    puts(ft_itoa(nb));
                    break;
                }
                case 'x': 
                {
                    uint32_t hex = va_arg(args, uint32_t);
                    puts(ft_itoa_base(hex, 16));
                    break;
                }
                default:
                {
                    putc(*fmt);
                }
            }
            fmt++;
        }
    }
    va_end(args);
}

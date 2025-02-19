#include "../../include/libk.h"

void ft_printk(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    while(*fmt)
    {
        if(*fmt == '%' && (fmt + 1))
        {
            fmt++;
            switch(*fmt)
            {
                case 'c':
                {
                    char c = va_arg(args, int);
                    putc(c);
                }
                case 's':
                {
                    char *str = va_arg(args, char*);
                    puts(str);
                }
                case 'd':
                {
                    int nb = va_arg(args, int);
                    ft_itoa(nb);
                }
                // case 'x':
                // {

                // }
                // default:
                //     putc(*fmt);
            }
        }
    }
}

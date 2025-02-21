#include "../../include/lib.h"

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
                // case 'x':
                // {

                // }
                default:
                {
                    putc(*fmt);
                }
            }
            *fmt++;
        }
    }
    va_end(args);
}

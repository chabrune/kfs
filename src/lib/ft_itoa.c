#include "../../include/lib.h"

int ft_lenght(int nb)
{
    unsigned int len = 0;
    if(nb < 0)
    {
        nb = -nb;
        len++;
    }
    if(nb == 0)
        len++;
    while(nb > 0)
    {
        nb /= 10;
        len++;
    }
    return(len);
}

char *ft_itoa(int nb)
{
    static char buffer[12];
    int len = ft_lenght(nb);
    int is_negative = 0;

    if (nb == 0)
        return "0";

    buffer[len] = '\0';

    if (nb < 0)
    {
        is_negative = 1;
        nb = -nb;
    }

    while (nb > 0)
    {
        buffer[--len] = '0' + (nb % 10);
        nb /= 10;
    }

    if (is_negative)
        buffer[--len] = '-';

    return &buffer[len];
}

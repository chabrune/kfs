#include "../../include/libk.h"

char *strcpy(char *dest, const char *src)
{
    uint32_t i = 0;
    while(src[i])
    {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
    return(dest);
}
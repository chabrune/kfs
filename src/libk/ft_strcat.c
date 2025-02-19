#include "../../include/libk.h"

void     ft_strcat(char *dest, const char *src)
{
    uint32_t len = ft_strlen(dest);
    uint32_t i;
    for(i = 0; src[i]; i++)
        dest[len + i] = src[i];
    dest[len + i] = '\0';
    return;
}
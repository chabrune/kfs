#include "../../include/libk.h"

void *ft_memcpy(void *dest, const void *src, uint32_t n)
{
    unsigned char *pd = (unsigned char *)dest;
    const unsigned char *ps = (const unsigned char *)src;
    
    for (uint32_t i = 0; i < n; i++)
        pd[i] = ps[i];

    return (dest);
}

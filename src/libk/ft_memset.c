#include "../../include/libk.h"

void *memset (void *s, int c, uint32_t n)
{
    unsigned char *ptr = (unsigned char*)s;
    for(uint32_t i = 0; i < n; i++)
    {
        ptr[i] = (unsigned char)c;
    }
    return s;
}
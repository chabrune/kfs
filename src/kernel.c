#include "../include/kernel.h"

void kmain(void)
{
    init_idt();
    init();
    remap_pic();
    puts("FuckOs>$");
    asm volatile("sti");
    while(1);
}

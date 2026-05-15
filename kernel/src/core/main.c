#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "drivers/display.h"
#include "main.h"

void hcf(void)
{
    for (;;)
    {
        asm ("hlt");
    }
}

void kmain(void)
{

    if (display_check())
    {
        display_init();
    }
    else
    {
        hcf();
    }

    hcf();
}
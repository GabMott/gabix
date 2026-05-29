#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "drivers/display.h"
#include "main.h"

void halt(void)
{
    for (;;)
    {
        asm("hlt");
    }
}

void kmain(void)
{

    if (display_check())
    {
        display_init();
        print_str("Beep boop I am computer\n");
        print_str("This is another line\n");
        print_str("And another :)\n");
        print_str_dbg("This is a number: % - yay!\n", 57);
        print_str_dbg("Display height = %\n", get_display_height());
        print_str_dbg("Display width  = %\n", get_display_width());
        print_str_dbg("Display bpp = %\n", get_display_bpp());
        print_str_dbg("Display memory_model = %\n", get_display_memory_model());
        print_str_dbg("Display red_mask_size = %\n", get_display_red_mask_size());
        print_str_dbg("Display red_mask_shift = %\n", get_display_red_mask_shift());
        print_str_dbg("Display green_mask_size = %\n", get_display_green_mask_size());
        print_str_dbg("Display green_mask_shift = %\n", get_display_green_mask_shift());
        print_str_dbg("Display blue_mask_size = %\n", get_display_blue_mask_size());
        print_str_dbg("Display blue_mask_shift = %\n", get_display_blue_mask_shift());
    }
    else
    {
        halt();
    }

    halt();
}
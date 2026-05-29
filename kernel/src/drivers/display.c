#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "lib/memutil.h"
#include "lib/util.h"
#include "vendor/limine.h"
#include "vendor/terminus_font_16x32.h"
#include "display.h"

/*
    Limine stuff
*/

__attribute__((used, section(".limine_requests_start"))) static volatile uint64_t limine_requests_start_marker[] = LIMINE_REQUESTS_START_MARKER;

__attribute__((used, section(".limine_requests"))) static volatile uint64_t limine_base_revision[] = LIMINE_BASE_REVISION(6);

__attribute__((used, section(".limine_requests"))) static volatile struct limine_framebuffer_request framebuffer_request =
    {
        .id = LIMINE_FRAMEBUFFER_REQUEST_ID,
        .revision = 0};

__attribute__((used, section(".limine_requests_end"))) static volatile uint64_t limine_requests_end_marker[] = LIMINE_REQUESTS_END_MARKER;

/*
    Some globals - current pixel position and framebuffer info pointer.
    Eventually I use a better solution but this works for now.
*/

static uint32_t cur_x = 0;
static uint32_t cur_y = 0;

static struct limine_framebuffer *framebuffer;

/*
    Check some stuff about the framebuffer before using it.
*/

int display_check(void)
{
    if (LIMINE_BASE_REVISION_SUPPORTED(limine_base_revision) == false)
    {
        return 0;
    }

    if (framebuffer_request.response == NULL || framebuffer_request.response->framebuffer_count < 1)
    {
        return 0;
    }

    return 1;
}

uint64_t get_display_height(void)
{
    return framebuffer->height;
}

uint64_t get_display_width(void)
{
    return framebuffer->width;
}


uint16_t get_display_bpp(void)
{
    return framebuffer->bpp;
}

uint8_t get_display_memory_model(void)
{
    return framebuffer->memory_model;
}

uint8_t get_display_red_mask_size(void)
{
    return framebuffer->red_mask_size;
}

uint8_t get_display_red_mask_shift(void)
{
    return framebuffer->red_mask_shift;
}

uint8_t get_display_green_mask_size(void)
{
    return framebuffer->green_mask_size;
}

uint8_t get_display_green_mask_shift(void)
{
    return framebuffer->green_mask_shift;
}

uint8_t get_display_blue_mask_size(void)
{
    return framebuffer->blue_mask_size;
}

uint8_t get_display_blue_mask_shift(void)
{
    return framebuffer->blue_mask_shift;
}


/*
    Scollback the text on the display, for use when it is filled up
*/

void scrollback(void)
{
    for (uint32_t i = 0; i < (get_display_height() / CHARH) - 1; ++i)
    {
        uint32_t *src = framebuffer->address + ((i + 1) * (get_display_width() * CHARH)) * sizeof(uint32_t);
        uint32_t *dst = framebuffer->address + (i * (get_display_width() * CHARH)) * sizeof(uint32_t);

        memcpy(dst, src, (get_display_width() * CHARH) * sizeof(uint32_t));
    }
}

/*
    Just puts a pixel of a specific color somewhere.
*/

void put_pixel(uint32_t x, uint32_t y, uint32_t color)
{
    volatile uint32_t *framebuf_ptr = framebuffer->address;
    framebuf_ptr[y * (framebuffer->pitch / 4) + x] = color;
}

/*
    Renders a character using the bitmap font in the framebuffer.
    Just puts it in the next 16x32 px left to right top to bottom, it
    would be nice to impilement scrolling at some point but its not
    super important right now.
*/

void put_char(char symbol)
{
    for (uint32_t y = 0; y < 32; y++)
    {
        for (uint32_t x = 0; x < 16; x++)
        {
            uint16_t bitmap = terminus_font_16x32[(uint16_t)symbol][y];
            if (((bitmap >> (16 - x)) & 1) != 0)
            {
                put_pixel(x + cur_x, y + cur_y, WHITE);
            }
        }
    }

    cur_x += 16;
    if (cur_x >= get_display_width())
    {
        cur_x = 0;
        cur_y += 32;
        if (cur_y >= get_display_height())
        {
            cur_y -= 32;
            scrollback();
        }
    }
}

/*
    Loop through string until string terminator put'ing the chars
    along the way.
*/

void print_str(const char *str)
{
    for (const char *i = str; *i != '\0'; ++i)
    {
        if (*i == '\n')
        {
            cur_x = 0;
            cur_y += 32;
            if (cur_y >= get_display_height())
            {
                cur_y -= 32;
                scrollback();
            }
        }
        else
        {
            put_char(*i);
        }
    }
}

void print_str_dbg(const char *str, uint64_t val)
{
    for (const char *i = str; *i != '\0'; ++i)
    {
        if (*i == '\n')
        {
            cur_x = 0;
            cur_y += 32;
            if (cur_y >= get_display_height())
            {
                cur_y -= 32;
                scrollback();
            }
        }
        else if (*i == '%')
        {
            print_str(uint64_to_str(val));
        }
        else
        {
            put_char(*i);
        }
    }
}

/*
    Grab framebuffer info as a global pointer for later use,
    and set every pixel to black to initialize the framebuffer.
*/

int display_init(void)
{
    // Init global pointer (should do this differently probably)
    framebuffer = framebuffer_request.response->framebuffers[0];

    for (uint32_t y = 0; y < get_display_height(); y++)
    {
        for (uint32_t x = 0; x < get_display_width(); x++)
        {
            put_pixel(x, y, BLACK);
        }
    }

    return 0;
}
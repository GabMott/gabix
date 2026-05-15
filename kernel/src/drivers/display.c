#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "vendor/limine.h"
#include "display.h"
#include "vendor/terminus_font_16x32.h"

__attribute__((used, section(".limine_requests"))) static volatile uint64_t limine_base_revision[] = LIMINE_BASE_REVISION(6);

__attribute__((used, section(".limine_requests"))) static volatile struct limine_framebuffer_request framebuffer_request =
    {
        .id = LIMINE_FRAMEBUFFER_REQUEST_ID,
        .revision = 0};

__attribute__((used, section(".limine_requests_start"))) static volatile uint64_t limine_requests_start_marker[] = LIMINE_REQUESTS_START_MARKER;

__attribute__((used, section(".limine_requests_end"))) static volatile uint64_t limine_requests_end_marker[] = LIMINE_REQUESTS_END_MARKER;

static uint32_t cur_x = 0;
static uint32_t cur_y = 0;

static struct limine_framebuffer *framebuffer;

uint16_t swap8(uint16_t swap)
{
    return ((swap & 0x00FF) << 8) | ((swap & 0xFF00) >> 8);
}

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

    if (framebuffer_request.response->framebuffers[0]->width > DISPLAY_WIDTH || framebuffer_request.response->framebuffers[0]->height > DISPLAY_HEIGHT)
    {
        return 0;
    }

    return 1;
}

void put_pixel(uint32_t x, uint32_t y, uint32_t color)
{
    volatile uint32_t *framebuf_ptr = framebuffer->address;
    framebuf_ptr[y * (framebuffer->pitch / 4) + x] = color;
}

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

    // Assume that each char is 16x32px. May not always be correct but for now that's what I'm using
    // After putting a char lets go to the next cell
    cur_x += 16;
    if (cur_x >= DISPLAY_WIDTH)
    {
        cur_x = 0;
        cur_y += 32;
    }
}

int display_init(void)
{
    // Init global pointer (should do this differently probably)
    framebuffer = framebuffer_request.response->framebuffers[0];

    for (uint32_t y = 0; y < DISPLAY_HEIGHT; y++)
    {
        for (uint32_t x = 0; x < DISPLAY_WIDTH; x++)
        {
            put_pixel(x, y, BLACK);
        }
    }

    const char hello[] = "Beep boop computer\0";
    for (const char *i = hello; *i != '\0'; i++)
    {
        put_char(*i);
    }

    return 0;
}
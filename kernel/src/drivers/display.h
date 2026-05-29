#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include <stdint.h>

#define WHITE 0xFFFFFFFF
#define BLACK 0x00000000

#define CHARW 16
#define CHARH 32

void scrollback(void);
void print_str(const char *);
void print_str_dbg(const char *, uint64_t);
int display_check(void);
uint64_t get_display_height(void);
uint64_t get_display_width(void);
uint64_t get_display_pitch(void);
uint16_t get_display_bpp(void);
uint8_t  get_display_memory_model(void);
uint8_t  get_display_red_mask_size(void);
uint8_t  get_display_red_mask_shift(void);
uint8_t  get_display_green_mask_size(void);
uint8_t  get_display_green_mask_shift(void);
uint8_t  get_display_blue_mask_size(void);
uint8_t  get_display_blue_mask_shift(void);

int display_init(void);

#endif // _DISPLAY_H_
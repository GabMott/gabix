#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#define WHITE 0xFFFFFFFF
#define BLACK 0x00000000

#define DISPLAY_WIDTH 1280
#define DISPLAY_HEIGHT 800

#define CHARW 16
#define CHARH 32
#define PAD 5

int display_check(void);
int display_init(void);

#endif // _DISPLAY_H_
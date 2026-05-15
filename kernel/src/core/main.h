#ifndef _MAIN_H_
#define _MAIN_H_

static void hcf(void)
{
    for (;;)
    {
        asm ("hlt");
    }
}

void kmain(void);

#endif // _MAIN_H_
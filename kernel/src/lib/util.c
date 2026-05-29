#include "util.h"
#include <stddef.h>
#include <stdint.h>

void uint64_to_str_r(char **ptr, uint64_t val)
{
    char char_val = (val % 10) + '0';

    if(val >= 10)
    {
        uint64_to_str_r(ptr, val / 10);
    }
    
    *((*ptr)++) = char_val;
}

char* uint64_to_str(uint64_t val)
{
    static char str[21];
    char *ptr = str;

    uint64_to_str_r(&ptr, val);
    *ptr = '\0';
    return str;
}
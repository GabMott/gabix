#include "main.h"
#include "drivers/display.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

void kmain(void) {

  if (display_check()) {
    display_init();
  } else {
    hcf();
  }

  hcf();
}
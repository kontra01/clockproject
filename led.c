#include "segments.c"
#include "definitions.c"

void led_init() {
    // maybe?
    return 0;
}

void led_out(char segment) {
    outb_p(segment, BASEPORT);
}

void clear_screen() {
    led_out(EMPTY_CHAR);
}

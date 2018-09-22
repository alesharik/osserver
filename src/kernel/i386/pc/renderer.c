#include "graphics/screen.h"
#include <stdint.h>
#include <string.h>

void render_loading(void) {

}

void render_console(void) {

}

void render_error(const char *msg) {
    size_t size = strlen(msg);
    screen_setcursor(screen_resolution_x() / 2 - size / 2, screen_resolution_y() / 2 + 1);
    screen_set_foreground(COLOR_RED);
    screen_set_background(COLOR_WHITE);
    screen_write_string(msg);
}
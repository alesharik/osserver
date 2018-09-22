#ifndef OSSERVER_SCREEN_H
#define OSSERVER_SCREEN_H

#include <stdbool.h>
#include <stdint.h>
#include "vga.h"

enum basic_color {
    COLOR_BLACK = 0,
    COLOR_BLUE = 1,
    COLOR_GREEN = 2,
    COLOR_CYAN = 3,
    COLOR_RED = 4,
    COLOR_MAGENTA = 5,
    COLOR_BROWN = 6,
    COLOR_LIGHT_GREY = 7,
    COLOR_DARK_GREY = 8,
    COLOR_LIGHT_BLUE = 9,
    COLOR_LIGHT_GREEN = 10,
    COLOR_LIGHT_CYAN = 11,
    COLOR_LIGHT_RED = 12,
    COLOR_LIGHT_MAGENTA = 13,
    COLOR_LIGHT_BROWN = 14,
    COLOR_WHITE = 15,
};

bool _use_vga = false;
size_t _screen_x_pos = 0;
size_t _screen_y_pos = 0;
uint32_t _screen_foreground = COLOR_WHITE;
uint32_t _screen_background = COLOR_BLACK;

///VGA - 8 bit palette
void screen_initialize(void) {
    _use_vga = true;
    if (_use_vga)
        vga_terminal_initialize();
}

size_t screen_resolution_x() {
    if (_use_vga)
        return VGA_WIDTH;
    return 0;
}

size_t screen_resolution_y() {
    if (_use_vga)
        return VGA_HEIGHT;
    return 0;
}

void screen_set_foreground(uint32_t color) {
    _screen_foreground = color;
}

void screen_set_background(uint32_t color) {
    _screen_background = color;
}

void screen_setcursor(size_t x, size_t y) {
    if (x > screen_resolution_x())
        _screen_x_pos = screen_resolution_x();
    else
        _screen_x_pos = x;

    if (y > screen_resolution_y())
        _screen_y_pos = screen_resolution_y();
    else
        _screen_y_pos = y;
}

void _screen_driver_write_char(char c) {
    if(_use_vga) {
        vga_putentryat(c, vga_entry_color((uint8_t) _screen_foreground, (uint8_t) _screen_background), _screen_x_pos, _screen_y_pos);
        _screen_x_pos++;
        if(_screen_x_pos == screen_resolution_x()) {
            _screen_x_pos = 0;
            _screen_y_pos++;
            if(_screen_y_pos == screen_resolution_y())
                _screen_y_pos = 0;
        }
    }
}

void screen_write_char(char c) {
    if(c == '\n') {
        _screen_x_pos = 0;
        _screen_y_pos++;
        if(_screen_y_pos == screen_resolution_y())
            _screen_y_pos = 0;
    } else
        _screen_driver_write_char(c);
}

void screen_write(const char *data, size_t size) {
    for (size_t i = 0; i < size; i++)
        screen_write_char(data[i]);
}

void screen_write_string(const char *data) {
    screen_write(data, strlen(data));
}

bool screen_is_vga() {
    return _use_vga;
}

#endif //OSSERVER_SCREEN_H

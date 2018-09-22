#ifndef OSSERVER_SCREEN_H
#define OSSERVER_SCREEN_H

#include <stdbool.h>
#include <stdint.h>
#include "vga.h"
#include "basic_color_enum.h"

extern bool _use_vga;
extern size_t _screen_x_pos;
extern size_t _screen_y_pos;
extern uint32_t _screen_foreground;
extern uint32_t _screen_background;

///VGA - 8 bit palette
void screen_initialize(void);

size_t screen_resolution_x();

size_t screen_resolution_y();

void screen_set_foreground(uint32_t color);

void screen_set_background(uint32_t color);

void screen_setcursor(size_t x, size_t y);

void _screen_driver_write_char(char c);

void screen_write_char(char c);

void screen_write(const char *data, size_t size);

void screen_write_string(const char *data);

bool screen_is_vga();

#endif //OSSERVER_SCREEN_H

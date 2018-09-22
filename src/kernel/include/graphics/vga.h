#ifndef OSSERVER_VGA_H
#define OSSERVER_VGA_H
#include <stddef.h>
#include <stdint.h>
#include "string.h"

static inline uint8_t vga_entry_color(uint8_t fg, uint8_t bg) {
    return fg | bg << 4;
}

static const size_t VGA_WIDTH = 320;
static const size_t VGA_HEIGHT = 200;

extern uint16_t* _vga_buffer;

void vga_terminal_initialize(void);

void vga_putentryat(char c, uint8_t color, size_t x, size_t y);


#endif //OSSERVER_VGA_H

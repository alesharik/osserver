#ifndef OSSERVER_VGA_H
#define OSSERVER_VGA_H
#include <stddef.h>
#include <stdint.h>
#include "string.h"

static inline uint8_t vga_entry_color(uint8_t fg, uint8_t bg) {
    return fg | bg << 4;
}

static inline uint16_t vga_entry(unsigned char uc, uint8_t color) {
    return (uint16_t) uc | (uint16_t) color << 8;
}

static const size_t VGA_WIDTH = 640;
static const size_t VGA_HEIGHT = 480;

uint16_t* _vga_buffer;

void vga_terminal_initialize(void) {
    _vga_buffer = (uint16_t*) 0xB8000;
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t index = y * VGA_WIDTH + x;
            _vga_buffer[index] = vga_entry(' ', (uint8_t) _vga_buffer);
        }
    }
}

void vga_putentryat(char c, uint8_t color, size_t x, size_t y) {
    const size_t index = y * VGA_WIDTH + x;
    _vga_buffer[index] = vga_entry((unsigned char) c, color);
}


#endif //OSSERVER_VGA_H

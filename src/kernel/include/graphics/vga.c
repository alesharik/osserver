#include <stddef.h>
#include <stdint.h>
#include "basic_color_enum.h"

static inline uint16_t vga_entry(unsigned char uc, uint8_t color) {
    return (uint16_t) uc | (uint16_t) color << 8;
}

static const size_t VGA_WIDTH;
static const size_t VGA_HEIGHT;

int16_t* _vga_buffer = 0;

void vga_terminal_initialize(void) {
    _vga_buffer = (uint16_t*) 0xB8000;
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t index = y * VGA_WIDTH + x;
            _vga_buffer[index] = vga_entry(' ', COLOR_WHITE | COLOR_BLACK >> 4);
        }
    }
}

void vga_putentryat(char c, uint8_t color, size_t x, size_t y) {
    const size_t index = y * VGA_WIDTH + x;
    _vga_buffer[index] = vga_entry((unsigned char) c, color);
}
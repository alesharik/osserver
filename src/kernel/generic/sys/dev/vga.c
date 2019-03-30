#include "sys/dev/vga.h"
#include <stdint.h>
#include "sys/kernel.h"
#include "string.h"

#define VGA_ADDRESS (uint16_t *) (0xB8000 + KERNEL_VIRTUAL_BASE)

uint16_t vga_y_pos = 0;
uint16_t vga_width;
uint16_t vga_height;

static inline uint16_t vga_entry(char uc, uint8_t color) {
    return (uint16_t) uc | (uint16_t) color << 8;
}

static inline uint16_t vga_symbol(char c, vga_color foreground, vga_color background) {
    return vga_entry(c, foreground | background >> 4);
}

void vga_init(uint16_t width, uint16_t height) {
    vga_width = width;
    vga_height = height;
    uint16_t *screen = VGA_ADDRESS;
    for (int h = 0; h < height; ++h)
        for (int w = 0; w < width; ++w)
            screen[h * width + w] = vga_symbol(' ', VGA_COLOR_WHITE, VGA_COLOR_BLACK);
}

void vga_write_line(const char *str, vga_color foreground) {
    uint16_t *screen = VGA_ADDRESS;
    if(vga_y_pos == vga_height) {
        for (int h = 1; h < vga_height; ++h)
            for (int w = 0; w < vga_width; ++w)
                screen[(h - 1) * vga_width + w] = screen[h * vga_width + w];
        for (int w = 0; w < vga_width; ++w)
            screen[(vga_height - 1) * vga_width + w] = vga_symbol(' ', VGA_COLOR_WHITE, VGA_COLOR_BLACK);
        vga_y_pos--;
    }
    uint16_t len = strlen(str);
    if(len > vga_width)
        len = vga_width;
    for (int i = 0; i < len; ++i)
        screen[vga_y_pos * vga_width + i] = vga_symbol(str[i], foreground, VGA_COLOR_BLACK);
    vga_y_pos++;
}
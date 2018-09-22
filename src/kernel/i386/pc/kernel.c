#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "string.h"
#include "sys/nmi.h"
#include "graphics/screen.h"
#include "graphics/renderer.h"

#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

#if !defined(__i386__)
#error "This os needs to be compiled with a ix86-elf compiler"
#endif

void terminal_initialize(void) {
    uint16_t *terminal_buffer = (uint16_t*) 0xB8000;
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t index = y * VGA_WIDTH + x;
            terminal_buffer[index] = ' ' | ((0 | 15 >> 4) << 8);
        }
    }
}

void kernel_main(void) {
    nmi_enable();
    terminal_initialize();
    screen_initialize();

    render_error("Test");
}
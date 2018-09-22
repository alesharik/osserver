#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "string.h"
#include "sys/nmi.h"
#include "sys/asm.h"
#include "graphics/screen.h"
#include "renderer.h"

#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

#if !defined(__i386__)
#error "This os needs to be compiled with a ix86-elf compiler"
#endif

void kernel_main(void) {
    nmi_enable();
    screen_initialize();

    screen_write_string("test");
    render_error("Test");
}
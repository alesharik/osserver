#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "multiboot.h"
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

void kernel_main(unsigned long magic, unsigned long addr) {
    if(magic != MULTIBOOT_BOOTLOADER_MAGIC) {
        //FIXME DO SOMETHING
        return;
    }
    multiboot_info_t *mbt = addr;

    screen_initialize();

    render_error("qwe");
}
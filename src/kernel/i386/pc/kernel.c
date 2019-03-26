#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "multiboot.h"
#include "sys/cpu/detect.h"
#include "sys/gdt.h"
#include "sys/kernel.h"
#include "sys/interrupt/idt.h"
#include "sys/interrupt/lapic.h"
#include "sys/interrupt/ioapic.h"
#include "sys/interrupt/pic.h"
#include "sys/time/pit.h"
#include "sys/acpi/acpi.h"

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

void kpanic(const char *message) {
    render_error(message);
    __kernel_hang();
}

void check_cpu() {
    if ((cpu_info.edx_features & EDX_MSR) != EDX_MSR)
        kpanic("Cannot run without MSR support");
//    if((cpu_info.edx_features & EDX_ACPI) != EDX_ACPI) because qemu doesn't have ACPI support but it generates ACPI tables
//        kpanic("Cannot run without ACPI support");
    if ((cpu_info.edx_features & EDX_APIC) != EDX_APIC)
        kpanic("Cannot run without APIC chip");
}

void kernel_premain(unsigned long magic, unsigned long addr) {

    if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
        kernel_exit();
        return;
    }

    gdt_init();
    screen_initialize();
}

void kernel_main(unsigned long magic, unsigned long addr) {
    multiboot_info_t *mbt = (multiboot_info_t *) addr;
    multiboot_memory_map_t *mmap = mbt->mmap_addr;
//    while (mmap < mbt->mmap_addr + mbt->mmap_length) {
//        mmap = (multiboot_memory_map_t *) ((unsigned int) mmap + mmap->size + sizeof(mmap->size));
//    }

    cpu_init_info();
    check_cpu();
    acpi_init();

    ioapic_init();
    idt_init();
    pic_init();
    lapic_init();
    pit_init();

    __asm__ volatile("sti"); //Enable interrupts

    render_error("qwe");

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
    for (;;) {
        asm("hlt");
    }
#pragma clang diagnostic pop
    render_error("exit");
}
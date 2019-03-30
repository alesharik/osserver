#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "internals.h"
#include "multiboot.h"
#include "sys/mem/paging.h"
#include "sys/mem/kmem.h"
#include "sys/cpu/detect.h"
#include "sys/gdt.h"
#include "sys/kernel.h"
#include "sys/interrupt/idt.h"
#include "sys/interrupt/lapic.h"
#include "sys/interrupt/ioapic.h"
#include "sys/interrupt/pic.h"
#include "sys/time/pit.h"
#include "sys/acpi/acpi.h"
#include "sys/ps2/controller.h"
#include "sys/ps2/manager.h"
#include "sys/dev/vga.h"
#include "sys/klog.h"

#include "string.h"
#include "sys/nmi.h"

#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

#if !defined(__i386__)
#error "This os needs to be compiled with a ix86-elf compiler"
#endif

uint16_t kdevmap_index = 0;

void kpanic(const char *message) {
    kerror(message);
    __kernel_hang();
}

void check_cpu() {
    if ((cpu_info.edx_features & EDX_MSR) != EDX_MSR)
        kpanic("Cannot run without MSR support");
//    if((cpu_info.edx_features & EDX_ACPI) != EDX_ACPI) because qemu doesn't have ACPI support but it generates ACPI tables
//        kpanic("Cannot run without ACPI support");
    if ((cpu_info.edx_features & EDX_APIC) != EDX_APIC)
        kpanic("Cannot run without APIC chip");
    if((cpu_info.edx_features & EDX_PAE) != EDX_PAE)
        kpanic("Cannot run without PAE support");
}

void kernel_premain(unsigned long magic, unsigned long addr) {
    if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
        kernel_exit();
        return;
    }
    uint32_t *pages = (uint32_t *) __get_kernel_page_start();

    multiboot_info_t *mbt = (multiboot_info_t *) addr;
    multiboot_memory_map_t *mmap = mbt->mmap_addr + KERNEL_VIRTUAL_BASE;

    bool memorySetup = false;

    while (mmap < (mbt->mmap_addr + KERNEL_VIRTUAL_BASE) + mbt->mmap_length) {
        mmap = (multiboot_memory_map_t *) ((unsigned int) mmap + mmap->size + sizeof(mmap->size));
        if(mmap->type != MULTIBOOT_MEMORY_AVAILABLE)
            continue;
        uint64_t addr = (mmap->addr % 4096) > 0 ? (mmap->addr + mmap->addr % 4096) : mmap->addr;
        uint64_t len = (mmap->addr % 4096) > 0 ? (mmap->len - mmap->addr % 4096) : mmap->len;
        if(!memorySetup && len > 32 * 1024 * 1024 + 0x300000) {
            for (int i = 0; i < 32 * 1024 / 4; ++i) {
                pages[i] = (addr + i * 4096 + 0x300000) | 3;
            }
            memory_kernel_init((void *) (0xC0800000));
            memorySetup = true;
            //TODO setup user memory
        }
    }
    vga_init(mbt->framebuffer_width, mbt->framebuffer_height);
    klog_init();

    if(!memorySetup)
        kpanic("Not enough memory to setup kernel");
    uint32_t directory = __get_kernel_page_directory();
    uint16_t count = __get_kernel_page_directory_size();
    klog("Paging setup");
    __32_paging_init((uint32_t *) directory, count, pages, 770);

    klog("Loading CPUID...");
    cpu_init_info();
    klog("Checking CPU...");
    check_cpu();

    klog("Setting up GDT...");
    gdt_init();

    klog("Loading ACPI...");
    acpi_init();
}

void kernel_main(unsigned long magic, unsigned long addr) {
    klog("Setting up IOAPIC...");
    ioapic_init();
    klog("Setting up IDT...");
    idt_init();
    klog("Disabling PIC...");
    pic_init();
    klog("Setting up LAPIC...");
    lapic_init();
    klog("Setting up PIT...");
    pit_init();

    klog("Enabling interrupts...");
    idt_int_enable();
    klog("Yay! Interrupts enabled!");

    klog("Setting up PS/2 controller...");
    if(!ps2_controller_init())
        kwarning("Cannot setup PS/2 controller");
    else {
        klog("Setting up PS/2 devices...");
        ps2_manager_init();
    }
    klog("Kernel ready!");

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
    for (;;) {
        asm("hlt");
    }
#pragma clang diagnostic pop
}

void *kernel_map_device_map(void *ptr) {
    if(kdevmap_index >= 1024)
        kpanic("Cannot reserve device page: devmap full");
    uint32_t *page = __get_kernel_dev_page();
    page[kdevmap_index++] = ((uint32_t)ptr & ~0xFFF) | 3;
    __tlb_flush();
    return 0xc2800000 + ((kdevmap_index - 1) * 4096);
}
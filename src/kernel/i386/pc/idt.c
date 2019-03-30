#include "sys/interrupt/idt.h"
#include "egdt.h"
#include "sys/interrupt/ioapic.h"
#include "sys/acpi/acpi.h"
#include "sys/ps2/manager.h"
#include "sys/ps2/controller.h"

#define INT_TIMER 0x20
#define INT_PS2_FIRST 0x21
#define INT_PS2_SECOND 0x22
#define INT_SPURIOUS 0xff

#define IRQ_BASE                        0x20

#define IRQ_TIMER                       0x02
#define IRQ_KEYBOARD                    0x01
#define IRQ_COM2                        0x03
#define IRQ_COM1                        0x04
#define IRQ_FLOPPY                      0x06
#define IRQ_ATA0                        0x0e
#define IRQ_ATA1                        0x0f

struct idt_descriptor {
    uint16_t offset_1;
    uint16_t selector; // a code segment selector in GDT or LDT
    uint8_t zero;      // unused, set to 0
    uint8_t type_attr; // type and attributes, see below
    uint16_t offset_2; // offset bits 16..31
} __attribute((packed));

struct idt_desc {
    uint16_t limit;
    uint64_t base;
} __attribute((packed));

struct idt_descriptor idt_descriptors[256];
bool idt_interrupts_enabled = false;
bool idt_setup = false;

void idt_init(void) {
    for (int i = 0; i < 20; ++i)
        idt_set_handler(i, INTERRUPT, __idt_exception_handlers[i]);
    for (int i = 20; i < 32; ++i)
        idt_set_handler(i, INTERRUPT, __idt_default_exception_handler);
    for (int i = 32; i < 256; ++i)
        idt_set_handler(i, TRAP, __idt_default_interrupt_handler);

    idt_set_handler(INT_SPURIOUS, INTERRUPT, __idt_spurious_interrupt_handler);
    idt_set_handler(INT_TIMER, INTERRUPT, __idt_pit_interrupt_handler);
    idt_set_handler(INT_PS2_FIRST, INTERRUPT, __idt_ps2_first_interrupt_handler);
    idt_set_handler(INT_PS2_SECOND, INTERRUPT, __idt_ps2_second_interrupt_handler);

    //Enable interrupts
    ioapic_all_set_entry(acpi_remap_irq(IRQ_TIMER), INT_TIMER);
    ioapic_all_set_entry(acpi_remap_irq(PS2_CONTROLLER_IRQ_FIRST_PORT), INT_PS2_FIRST);
    ioapic_all_set_entry(acpi_remap_irq(PS2_CONTROLLER_IRQ_SECOND_PORT), INT_PS2_SECOND);

    struct idt_desc desc = {
            .limit = 256 * sizeof(struct idt_descriptor) - 1,
            .base = (uint64_t) idt_descriptors
    };

    __asm__ volatile("lidt %0" : : "m" (desc) : "memory");
    idt_setup = true;
}

void idt_set_entry(uint8_t index, uint32_t offset, uint16_t selector, uint8_t type) {
    struct idt_descriptor descriptor = {
            .selector = selector,
            .zero = 0,
            .type_attr = type | 0b10000000,
            .offset_1 = offset & 0xFFFF,
            .offset_2 = offset >> 16
    };
    idt_descriptors[index] = descriptor;
}

void idt_set_handler(uint8_t interrupt, idt_interrupt_type type, void(*handler)()) {
    if (handler)
        idt_set_entry(interrupt, (uint32_t) handler, __gdt_get_code_selector(), type);
    else
        idt_set_entry(interrupt, 0, 0, 0);
}

void idt_int_disable() {
    if (idt_interrupts_enabled)
        __asm__ volatile("cli");
    idt_interrupts_enabled = false;
}

void idt_int_enable() {
    if(!idt_setup)
        return;
    if (!idt_interrupts_enabled)
        __asm__ volatile("sti");
    idt_interrupts_enabled = true;
}
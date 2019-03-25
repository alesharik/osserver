#include "sys/interrupt/idt.h"
#include "egdt.h"
#include "graphics/screen.h"

#define INT_TIMER 0x20
#define INT_SPURIOUS 0xff

struct idt_descriptor {
    uint16_t offset_1;
    uint16_t selector; // a code segment selector in GDT or LDT
    uint8_t zero;      // unused, set to 0
    uint8_t type_attr; // type and attributes, see below
    uint16_t offset_2; // offset bits 16..31
} __attribute((packed));

typedef struct IdtDesc
{
    uint16_t limit;
    uint64_t base;
}  __attribute((packed)) IdtDesc;

struct idt_descriptor idt_descriptors[256];

void idt_init(void) {
    for (int i = 0; i < 20; ++i)
        idt_set_handler(i, INTERRUPT, __idt_exception_handlers[i]);
    for (int i = 20; i < 32; ++i)
        idt_set_handler(i, INTERRUPT, __idt_default_exception_handler);
    for (int i = 32; i < 256; ++i)
        idt_set_handler(i, TRAP, __idt_default_interrupt_handler);

    idt_set_handler(INT_SPURIOUS, INTERRUPT, __idt_spurious_interrupt_handler);
    idt_set_handler(INT_TIMER, INTERRUPT, __idt_pit_interrupt_handler);

//    __idt_set(&idt_descriptors, sizeof(idt_descriptors));

    IdtDesc idtDesc =
            {
                    .limit = 256 * sizeof(struct idt_descriptor) - 1,
                    .base = (uint64_t) idt_descriptors
            };
    __asm__ volatile("lidt %0" : : "m" (idtDesc) : "memory");
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
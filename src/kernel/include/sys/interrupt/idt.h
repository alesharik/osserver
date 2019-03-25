#ifndef OSSERVER_IDT_H
#define OSSERVER_IDT_H

#include <stdint.h>

typedef enum {
    TASK = 0b0101,
    INTERRUPT = 0b1110,
    TRAP = 0b1111
} idt_interrupt_type;

void idt_init(void);

void idt_set_handler(uint8_t interrupt, idt_interrupt_type type, void(*handler)());

void __idt_set(void *idt, uint16_t size);

extern void __idt_spurious_interrupt_handler();
extern void __idt_pit_interrupt_handler();
extern void __idt_default_exception_handler();
extern void __idt_default_interrupt_handler();
extern void (*__idt_exception_handlers[20])();

#endif //OSSERVER_IDT_H

#ifndef OSSERVER_IDT_H
#define OSSERVER_IDT_H

#include <stdint.h>

typedef enum {
    TASK = 0b0101,
    INTERRUPT = 0b1110,
    TRAP = 0b1111
} idt_interrupt_type;

void idt_init(void);

///
/// \param interrupt
/// \param type
/// \param handler must be ready-to-use asm interrupt code
void idt_set_handler(uint8_t interrupt, idt_interrupt_type type, void(*handler)());

void idt_int_disable();

void idt_int_enable();

extern void __idt_ps2_first_interrupt_handler();
extern void __idt_ps2_second_interrupt_handler();
extern void __idt_spurious_interrupt_handler();
extern void __idt_pit_interrupt_handler();
extern void __idt_default_exception_handler();
extern void __idt_default_interrupt_handler();
extern void (*__idt_exception_handlers[20])();

#endif //OSSERVER_IDT_H

#ifndef OSSERVER_ASM_H
#define OSSERVER_ASM_H

#include <stdint.h>
#include <stdbool.h>

static inline void outb(uint16_t port, uint8_t val) {
    asm volatile ( "outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile ( "inb %1, %0": "=a"(ret): "Nd"(port));
    return ret;
}

static inline unsigned long save_irqdisable(void) {
    unsigned long flags;
    asm volatile ("pushf\n\tcli\n\tpop %0" : "=r"(flags) : : "memory");
    return flags;
}

static inline void irqrestore(unsigned long flags) {
    asm ("push %0\n\tpopf" : : "rm"(flags) : "memory","cc");
}

#endif //OSSERVER_ASM_H

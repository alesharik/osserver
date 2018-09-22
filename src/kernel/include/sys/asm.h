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

static inline bool are_interrupts_enabled() {
    unsigned long flags;
    asm volatile ( "pushf\n\t"
                   "pop %0": "=g"(flags) );
    return (bool)(flags & (1 << 9));
}

static inline unsigned long save_irqdisable(void) {
    unsigned long flags;
    asm volatile ("pushf\n\tcli\n\tpop %0" : "=r"(flags) : : "memory");
    return flags;
}

static inline void irqrestore(unsigned long flags) {
    asm ("push %0\n\tpopf" : : "rm"(flags) : "memory","cc");
}

static inline void lidt(void* base, uint16_t size) {
    struct {
        uint16_t length;
        void*    base;
    } __attribute__((packed)) IDTR = { size, base };
    asm ( "lidt %0" : : "m"(IDTR) );
}

#endif //OSSERVER_ASM_H

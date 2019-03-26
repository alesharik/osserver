#include <stdint.h>
#include "sys/asm.h"
#include "sys/interrupt/lapic.h"

#define PIT_COUNTER0                        0x40
#define PIT_CMD                             0x43

#define PIT_CMD_BINARY                      0x00
#define PIT_CMD_BCD                         0x01

#define PIT_CMD_MODE_INTERRUPT              0x00
#define PIT_CMD_MODE_ONESHOT                0x02
#define PIT_CMD_MODE_RATE                   0x04
#define PIT_CMD_MODE_SQUARE                 0x06
#define PIT_CMD_MODE_SOFT_STROBE            0x08
#define PIT_CMD_MODE_HARD_STROBE            0x0a

#define PIT_CMD_LATCH                       0x00
#define PIT_CMD_RW_LOW                      0x10
#define PIT_CMD_RW_HI                       0x20
#define PIT_CMD_RW_BOTH                     0x30

#define PIT_CMD_COUNTER0                    0x00
#define PIT_CMD_COUNTER1                    0x40
#define PIT_CMD_COUNTER2                    0x80
#define PIT_CMD_READBACK                    0xc0

#define PIT_FREQUENCY                       1193182
#define PIT_COUNTER0_RUN_FREQUENCY          1000

unsigned long long int __pit_time = 0;

void pit_init() {
    unsigned int divisor = PIT_FREQUENCY / PIT_COUNTER0_RUN_FREQUENCY;
    outb(PIT_CMD, PIT_CMD_BINARY | PIT_CMD_MODE_SQUARE | PIT_CMD_RW_BOTH | PIT_CMD_COUNTER0);
    outb(PIT_COUNTER0, divisor);
    outb(PIT_COUNTER0, divisor >> 8);
}

void pit_sleep(unsigned int ms) {
    unsigned long long int now = __pit_time;
    while (__pit_time - now < ms);
}

void __pit_tick() {
    __pit_time++;
    lapic_reset_irq();
}

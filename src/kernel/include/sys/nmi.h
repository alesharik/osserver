#ifndef OSSERVER_NMI_H
#define OSSERVER_NMI_H

#include "asm.h"
#include <stdint.h>
#include <stdbool.h>

enum nmi_error_a {
    ALTERNATE_HOT_RESET = 0,
    ALTERNATE_GATE_A20 = 1,
    RESERVED = 2,
    SECURITY_LOCK = 3,
    WATCHDOG_TIMER_STATUS = 4,
    RESERVED_2 = 5,
    HDD_2_DRIVE_ACTIVITY = 6,
    HDD_1_DRIVE_ACTIVITY = 7
};

enum nmi_error_b {
    TIMER_2_TIED_TO_SPEAKER = 0 >> 8,
    SPEAKER_DATA_ENABLE = 1 >> 8,
    PARITY_CHECK_ENABLE = 2 >> 8,
    CHANNEL_CHECK_ENABLE = 3 >> 8,
    REFRESH_REQUEST = 4 >> 8,
    TIMER_2_OUTPUT = 5 >> 8,
    ///PCI/bus error
            CHANNEL_CHECK = 6 >> 8,
    ///Memory IO error
            PARITY_CHECK = 7 >> 8
};

bool is_nmi_on() {
    return (inb(0x70) & 0x80) == 0;
}

void nmi_enable(void) {
    outb(0x70, (uint8_t) (inb(0x70) & 0x7F));
}

void nmi_disable(void) {
    outb(0x7, (uint8_t) (inb(0x70) | 0x80));
}

int16_t nmi_status(void) {
    return inb(0x92) | (inb(0x61) >> 8);
}

bool nmi_critical(void) {
    int16_t status = nmi_status();
    return ((status | WATCHDOG_TIMER_STATUS) == WATCHDOG_TIMER_STATUS)
           || ((status | CHANNEL_CHECK) == CHANNEL_CHECK)
           || ((status | PARITY_CHECK) == PARITY_CHECK);
}

#endif //OSSERVER_NMI_H

#include "sys/interrupt/lapic.h"
#include <stdint.h>
#include "sys/cpu/msr.h"
#include "sys/acpi/acpi.h"

#define LAPIC_ID                        0x0020  // Local APIC ID
#define LAPIC_VER                       0x0030  // Local APIC Version
#define LAPIC_TPR                       0x0080  // Task Priority
#define LAPIC_APR                       0x0090  // Arbitration Priority
#define LAPIC_PPR                       0x00a0  // Processor Priority
#define LAPIC_EOI                       0x00b0  // EOI
#define LAPIC_RRD                       0x00c0  // Remote Read
#define LAPIC_LDR                       0x00d0  // Logical Destination
#define LAPIC_DFR                       0x00e0  // Destination Format
#define LAPIC_SVR                       0x00f0  // Spurious Interrupt Vector
#define LAPIC_ISR                       0x0100  // In-Service (8 registers)
#define LAPIC_TMR                       0x0180  // Trigger Mode (8 registers)
#define LAPIC_IRR                       0x0200  // Interrupt Request (8 registers)
#define LAPIC_ESR                       0x0280  // Error Status
#define LAPIC_ICRLO                     0x0300  // Interrupt Command
#define LAPIC_ICRHI                     0x0310  // Interrupt Command [63:32]
#define LAPIC_TIMER                     0x0320  // LVT Timer
#define LAPIC_THERMAL                   0x0330  // LVT Thermal Sensor
#define LAPIC_PERF                      0x0340  // LVT Performance Counter
#define LAPIC_LINT0                     0x0350  // LVT LINT0
#define LAPIC_LINT1                     0x0360  // LVT LINT1
#define LAPIC_ERROR                     0x0370  // LVT Error
#define LAPIC_TICR                      0x0380  // Initial Count (for Timer)
#define LAPIC_TCCR                      0x0390  // Current Count (for Timer)
#define LAPIC_TDCR                      0x03e0  // Divide Configuration (for Timer)

#define ICR_FIXED                       0x00000000
#define ICR_LOWEST                      0x00000100
#define ICR_SMI                         0x00000200
#define ICR_NMI                         0x00000400
#define ICR_INIT                        0x00000500
#define ICR_STARTUP                     0x00000600

#define ICR_PHYSICAL                    0x00000000
#define ICR_LOGICAL                     0x00000800

#define ICR_IDLE                        0x00000000
#define ICR_SEND_PENDING                0x00001000

#define ICR_DEASSERT                    0x00000000
#define ICR_ASSERT                      0x00004000

#define ICR_EDGE                        0x00000000
#define ICR_LEVEL                       0x00008000

#define ICR_NO_SHORTHAND                0x00000000
#define ICR_SELF                        0x00040000
#define ICR_ALL_INCLUDING_SELF          0x00080000
#define ICR_ALL_EXCLUDING_SELF          0x000c0000

#define ICR_DESTINATION_SHIFT           24

//FIXME higher half

static inline uint32_t lapic_read(uint32_t reg) {
    return *(volatile uint32_t *)(acpi_apic_info.lapic_addr + reg);
}

static inline void lapic_write(uint32_t reg, uint32_t value) {
    *(volatile uint32_t *)(acpi_apic_info.lapic_addr + reg) = value;
}

void lapic_init(void) {
    //Clear task priority
    lapic_write(LAPIC_TPR, 0);

    lapic_write(LAPIC_DFR, 0xffffffff); //Flat mode
    lapic_write(LAPIC_LDR, 0x01000000); //CPU id = 1

    lapic_write(LAPIC_SVR, 0x100 | 0xff);
}

uint32_t lapic_get_id(void) {
    return lapic_read(LAPIC_ID) >> 24;
}

void lapic_send_init(uint32_t id) {
    lapic_write(LAPIC_ICRHI, id << ICR_DESTINATION_SHIFT);
    lapic_write(LAPIC_ICRLO, ICR_INIT | ICR_PHYSICAL | ICR_ASSERT | ICR_EDGE | ICR_NO_SHORTHAND);
    while (lapic_read(LAPIC_ICRLO) & ICR_SEND_PENDING);
}

void lapic_send_startup(uint32_t id, uint32_t vector) {
    lapic_write(LAPIC_ICRHI, id << ICR_DESTINATION_SHIFT);
    lapic_write(LAPIC_ICRLO, vector | ICR_STARTUP | ICR_PHYSICAL | ICR_ASSERT | ICR_EDGE | ICR_NO_SHORTHAND);
    while (lapic_read(LAPIC_ICRLO) & ICR_SEND_PENDING);
}

extern inline void lapic_reset_irq() {
    lapic_write(LAPIC_EOI, 0);
}
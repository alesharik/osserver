#include "sys/interrupt/ioapic.h"
#include "sys/acpi/acpi.h"

#define IOREGSEL                        0x00
#define IOWIN                           0x10

// IO APIC Registers
#define IOAPICID                        0x00
#define IOAPICVER                       0x01
#define IOAPICARB                       0x02
#define IOREDTBL                        0x10

//TODO higher half

void ioapic_out(uint32_t addr, uint32_t reg, uint32_t val) {
    *(volatile uint32_t *) (addr + IOREGSEL) = reg;
    *(volatile uint32_t *) (addr + IOWIN) = val;
}

uint32_t ioapic_in(uint32_t addr, uint32_t reg) {
    *(volatile uint32_t *) (addr + IOREGSEL) = reg;
    return *(volatile uint32_t *) (addr + IOWIN);
}

void ioapic_set_entry(uint8_t id, uint8_t index, uint64_t data) {
    if (id >= acpi_apic_info.ioapic_size)
        return;
    ioapic_out(acpi_apic_info.ioapic_addrs[id], IOREDTBL + index * 2, (uint32_t) data);
    ioapic_out(acpi_apic_info.ioapic_addrs[id], IOREDTBL + index * 2 + 1, (uint32_t) (data >> 32));
}

void ioapic_all_set_entry(uint8_t index, uint64_t data) {
    for (int i = 0; i < acpi_apic_info.ioapic_size; ++i)
        ioapic_set_entry(i, index, data);
}

void ioapic_init(void) {
    for (int i = 0; i < acpi_apic_info.ioapic_size; ++i) {
        uint32_t addr = acpi_apic_info.ioapic_addrs[i];
        uint32_t entryCount = ioapic_in(addr, IOAPICVER);
        uint32_t maxRedirectCount = ((entryCount >> 16) & 0xFF) + 1;

        //Disable interrupts
        for (unsigned int i = 0; i < maxRedirectCount; ++i) {
            ioapic_set_entry(addr, i, 1 << 16);
        }
    }
}
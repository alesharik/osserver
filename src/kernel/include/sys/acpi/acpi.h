#ifndef OSSERVER_ACPI_H
#define OSSERVER_ACPI_H

#include <stdint.h>
#include <stdbool.h>

#define MAX_CPU_COUNT 16

unsigned int acpi_cpu_count;
unsigned char acpi_cpu_ids[MAX_CPU_COUNT];

#define MAX_IOAPIC_COUNT 16

typedef struct {
    uint32_t lapic_addr;
    uint8_t ioapic_size;
    uint32_t ioapic_addrs[16];
} apic_info_struct;

apic_info_struct acpi_apic_info;
bool acpi_ps2_controller_exists;

void acpi_init(void);

unsigned int acpi_remap_irq(unsigned int irq);

#endif //OSSERVER_ACPI_H

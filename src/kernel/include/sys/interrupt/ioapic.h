#ifndef OSSERVER_IOAPIC_H
#define OSSERVER_IOAPIC_H

#include <stdint.h>

void ioapic_init(void);

void ioapic_set_entry(uint8_t id, uint8_t index, uint64_t data);

void ioapic_all_set_entry(uint8_t index, uint64_t data);

#endif //OSSERVER_IOAPIC_H

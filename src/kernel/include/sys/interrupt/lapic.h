#ifndef OSSERVER_LAPIC_H
#define OSSERVER_LAPIC_H

#include <stdint.h>

void lapic_init(void);

uint32_t lapic_get_id(void);

void lapic_send_init(uint32_t id);

void lapic_send_startup(uint32_t id, uint32_t vector);

inline void lapic_reset_irq();

#endif //OSSERVER_LAPIC_H

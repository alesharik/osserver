#ifndef OSSERVER_MSR_H
#define OSSERVER_MSR_H

#include <stdint.h>

void msr_get(uint32_t msr, uint32_t *lo, uint32_t *hi);

void msr_set(uint32_t msr, uint32_t lo, uint32_t hi);

#endif //OSSERVER_MSR_H

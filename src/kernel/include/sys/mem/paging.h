#ifndef OSSERVER_PAGING_H
#define OSSERVER_PAGING_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define PAGING_WRITE_THROUGH 0b0001
#define PAGING_USER 0b001
#define PAGING_RW 0b01
#define PAGING_NO_CACHE 0b00001

#define PAGING_DEVMAP_START 0xB8000000

void __32_paging_init(uint32_t *directory, uint16_t size, uint32_t *hpages, uint16_t hpages_start);

bool paging_map(void *phys, void *virt, size_t size, unsigned int flags);

bool paging_map_to_handler(void *virt, size_t size, void (*func)(void *));

bool paging_unmap(void *virt, size_t size);

void *paging_get_physical(void *virt);

#endif //OSSERVER_PAGING_H

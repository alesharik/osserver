#include "sys/mem/paging.h"
#include <stdint.h>
#include <sys/kernel.h>
#include <sys/mem/kmem_liballoc.h>
#include "internals.h"

uint32_t *paging_dir;
uint16_t paging_size;
uint32_t *paging_directory_map[1024];
uint32_t *higher_pages;
uint16_t higher_pages_start;

void __32_paging_init(uint32_t *directory, uint16_t size, uint32_t *hpages, uint16_t hpages_start) {
    paging_dir = directory;
    paging_size = size;
    higher_pages = hpages;
    higher_pages_start = hpages_start;
}

bool paging_map(void *phys, void *virt, size_t size, unsigned int flags) {
    size_t off = 0;
    do {
        unsigned long pdindex = (unsigned long) (virt + off) >> 22;
        unsigned long ptindex = (unsigned long) (virt + off) >> 12 & 0x03FF;
        if (pdindex > paging_size)
            return false;
        if (paging_dir[pdindex] == 0) {
            void *table = kmem_liballoc_alloc(1);
            if (table == NULL)
                kpanic("OOM");
            paging_dir[pdindex] = (uint32_t) paging_get_physical(table) | 3;
            paging_directory_map[pdindex] = table;
        }
        if (paging_directory_map[pdindex][ptindex] != 0)
            return false;
        paging_directory_map[pdindex][ptindex] = ((uint32_t) (phys + off) & ~0xFFF) | flags | 1;
    } while ((off += 4096) < size);
    __tlb_flush();
    return true;
}

bool paging_map_to_handler(void *virt, size_t size, void (*func)(void *)) {
    //TODO
    return false;
}

bool paging_unmap(void *virt, size_t size) {
    size_t off = 0;
    do {
        unsigned long pdindex = (unsigned long) (virt + off) >> 22;
        unsigned long ptindex = (unsigned long) (virt + off) >> 12 & 0x03FF;
        if (pdindex > paging_size)
            return false;
        if (paging_dir[pdindex] == 0)
            continue;
        if (paging_directory_map[pdindex][ptindex] != 0)
            paging_directory_map[pdindex][ptindex] = 0;
    } while ((off += 4096) < size);
    __tlb_flush();
    return true;
}

void *paging_get_physical(void *virt) {
    unsigned long pdindex = (unsigned long) virt >> 22;
    unsigned long ptindex = (unsigned long) virt >> 12 & 0x03FF;
    if(pdindex >= higher_pages_start)
        return (void *) (higher_pages[(pdindex - higher_pages_start) * 4096 + ptindex] & ~0xFFF) + ((unsigned long)virt & 0xFFF);
    if(paging_dir[pdindex] == 0)
        return NULL;

    if(paging_directory_map[pdindex][ptindex] == 0)
        return NULL;
    return (void *)((paging_directory_map[pdindex][ptindex] & ~0xFFF) + ((unsigned long)virt & 0xFFF));
}
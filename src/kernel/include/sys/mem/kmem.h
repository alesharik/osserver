#ifndef OSSERVER_KMEM_H
#define OSSERVER_KMEM_H

#include <stdint.h>
#include "multiboot.h"

#define KERNEL_MEMORY_SIZE_KB 32 * 1024

void memory_kernel_init(void *ptr);

void *kmalloc(uint32_t size);

void *kcalloc(uint32_t count, uint32_t size);

void *krealloc(void *ptr, uint32_t size);

void kfree(void *ptr);

#endif //OSSERVER_KMEM_H

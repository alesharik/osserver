#include "sys/mem/kmem.h"
#include "sys/mem/kmem_liballoc.h"
#include "sys/interrupt/idt.h"
#include <stddef.h>
#include <sys/kernel.h>

typedef struct kmem_page_struct {
    void *ptr;
    uint16_t size;
    struct kmem_page_struct *next;
} kmem_pages;

kmem_pages kmem_init_pages[KERNEL_MEMORY_SIZE_KB / 4];
kmem_pages *kmem_alloc_pages;

void *kmem_base;
kmem_pages *kmem_free_pages;

void memory_kernel_init(void *ptr) {
    kmem_base = ptr;
    kmem_alloc_pages = &kmem_init_pages[1];
    for (int i = 2; i < KERNEL_MEMORY_SIZE_KB / 4; ++i) {
        kmem_init_pages[i].next = kmem_alloc_pages;
        kmem_alloc_pages->next = &kmem_init_pages[i];
    }
    kmem_free_pages = &kmem_init_pages[0];
    kmem_free_pages->size = KERNEL_MEMORY_SIZE_KB / 4;
    kmem_free_pages->ptr = kmem_base;
}

extern int kmem_liballoc_lock() {
    idt_int_disable();
    return 0;
}

extern int kmem_liballoc_unlock() {
    idt_int_enable();
    return 0;
}

/** This is the hook into the local system which allocates pages. It
 * accepts an integer parameter which is the number of pages
 * required.  The page size was set up in the liballoc_init function.
 *
 * \return NULL if the pages were not allocated.
 * \return A pointer to the allocated memory.
 */
extern void* kmem_liballoc_alloc(int pages) {
    kmem_pages *page = kmem_free_pages;
    kmem_pages *prev = NULL;
    if(page == NULL)
        return NULL;
    do {
        if(page->size == pages) {
            if(prev == NULL)
                kmem_free_pages = page->next;
            else
                prev->next = page->next;
            void *ptr = page->ptr;
            page->ptr = 0;
            page->next = kmem_alloc_pages;
            page->size = 0;
            kmem_alloc_pages = page;
            return ptr;
        } else if(page->size > pages) {
            void *ptr = page->ptr;
            page->size -= pages;
            page->ptr += pages * 4096;
            return ptr;
        }
        prev = page;
    } while ((page = page->next) != 0);
    return NULL;
}

/** This frees previously allocated memory. The void* parameter passed
 * to the function is the exact same value returned from a previous
 * liballoc_alloc call.
 *
 * The integer value is the number of pages to free.
 *
 * \return 0 if the memory was successfully freed.
 */
extern int kmem_liballoc_free(void *ptr, int size) {
    if(kmem_free_pages == NULL) {
        if(kmem_free_pages == NULL)
            kpanic("Cannot free page: not enough free page structs");
        kmem_pages *pages = kmem_alloc_pages;
        kmem_alloc_pages = kmem_alloc_pages->next;
        pages->size = size;
        pages->ptr = ptr;
        kmem_free_pages = pages;
        return 0;
    }
    kmem_pages *p = kmem_free_pages;
    kmem_pages *prev;
    do {
        prev = p;
    } while ((p = p->next) != NULL && p->ptr < ptr);
    if(prev->ptr + prev->size * 4096 == ptr)
        prev->size += size;
    else if(p != NULL && ptr + size * 4096 == p->ptr)
        p->size += size;
    else {
        if(kmem_free_pages == NULL)
            kpanic("Cannot free page: not enough free page structs");
        kmem_pages *pages = kmem_alloc_pages;
        kmem_alloc_pages = kmem_alloc_pages->next;
        pages->next = p;
        pages->ptr = ptr;
        pages->size = size;
        prev->next = pages;
    }
    if(p != NULL && prev->ptr + prev->size * 4096 == p->ptr) {
        prev->size += p->size;
        prev->next = p->next;
        p->next = kmem_alloc_pages;
        kmem_alloc_pages = p;
    }
    return 0;
}

void *kmalloc(uint32_t size) {
    return kmem_malloc(size);
}

void *kcalloc(uint32_t count, uint32_t size) {
    return kmem_calloc(count, size);
}

void *krealloc(void *ptr, uint32_t size) {
    return kmem_realloc(ptr, size);
}

void kfree(void *ptr) {
    kmem_free(ptr);
}
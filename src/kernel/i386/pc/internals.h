#ifndef OSSERVER_INTERNALS_H
#define OSSERVER_INTERNALS_H

#include <stdint.h>

uint32_t __get_kernel_page_start(void);

uint32_t __get_kernel_page_directory(void);

uint32_t __get_kernel_page_directory_size(void);

void __tlb_flush(void);

uint32_t __get_kernel_dev_page(void);

#endif //OSSERVER_INTERNALS_H

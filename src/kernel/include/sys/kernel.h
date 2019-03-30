#define KERNEL_VIRTUAL_BASE 0xC0000000

void kernel_exit();

void __kernel_hang();

void kpanic(const char *message);

void *kernel_map_device_map(void *ptr);
void gdt_setup();

void __gdt_set(void *gdt, unsigned int size);

void __gdt_reload_segments(void);
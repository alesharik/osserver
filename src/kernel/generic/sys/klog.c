#include "sys/klog.h"
#include "sys/dev/vga.h"
#include "sys/mem/kmem.h"
#include "std/format.h"

void klog_init() {
    klog("KLog init");
}

void klog(const char *msg, ...) {
    va_list args;

    va_start(args, msg);
    char *buf = kmalloc(300);
    vsnprintf(buf, 300, msg, args);
    va_end(args);
    vga_write_line(buf, VGA_COLOR_WHITE);
    kfree(buf);
}

void kwarning(const char *msg, ...) {
    va_list args;

    va_start(args, msg);
    char *buf = kmalloc(300);
    vsnprintf(buf, 300, msg, args);
    va_end(args);
    vga_write_line(buf, VGA_COLOR_GREEN);
    kfree(buf);
}

void kerror(const char *msg, ...) {
    va_list args;

    va_start(args, msg);
    char *buf = kmalloc(300);
    vsnprintf(buf, 300, msg, args);
    va_end(args);
    vga_write_line(buf, VGA_COLOR_RED);
    kfree(buf);
}

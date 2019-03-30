#ifndef OSSERVER_FORMAT_H
#define OSSERVER_FORMAT_H

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>

int vsscanf(const char *str, const char *fmt, va_list args);
int sscanf(const char *str, const char *fmt, ...);

int vsnprintf(char *str, size_t size, const char *fmt, va_list args);
int snprintf(char *str, size_t size, const char *fmt, ...);

int64_t strtoul(const char *ptr_start, char **ptr_end, int base);

#endif //OSSERVER_FORMAT_H

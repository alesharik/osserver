#ifndef OSSERVER_STRING_H
#define OSSERVER_STRING_H

#include <stddef.h>

size_t strlen(const char *);

int memcmp(const char *, const char *, size_t);

void memset(void *ptr, uint8_t data, size_t len);

void memcpy(char *dest, const char *source, size_t len);

#endif //OSSERVER_STRING_H

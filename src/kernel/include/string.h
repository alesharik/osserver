#ifndef OSSERVER_STRING_H
#define OSSERVER_STRING_H

#include <stddef.h>

size_t strlen(const char *str) {
    size_t len = 0;
    while (str[len])
        len++;
    return len;
}

int memcmp(const char *s1, const char *s2, size_t len) {
    for (size_t i = 0; i < len; ++i)
        if(s1[i] != s2[i])
            return 0;
    return 1;
}

#endif //OSSERVER_STRING_H

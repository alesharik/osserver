#include <stddef.h>
#include <stdint.h>

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

void memset(void *ptr, uint8_t data, size_t len) {
    for (int i = 0; i < len; ++i)
        ((uint8_t *) ptr)[i] = data;
}

void memcpy(char *dest, const char *source, size_t len) {
    for (int i = 0; i < len; ++i)
        dest[i] = source[i];
}
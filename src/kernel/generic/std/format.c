#include "std/format.h"
#include <stdarg.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include "std/stdlib.h"

enum {
    PAD_ZERO = 1,
    PAD_LEFT
};

typedef struct {
    char *current;
    char *end;
    uint8_t flags;
    int32_t width;
} fmt;

static bool format_is_space(char c) {
    return c == ' ' || c == '\t' || c == '\r' || c == '\n' || c == '\f' || c == '\v';
}

static bool format_is_digit(char c) {
    return c >= '0' && c <= '9';
}

static void format_out_char(fmt *fmt, char c) {
    if (fmt->current < fmt->end)
        *fmt->current++ = c;
}

static void format_out_string(fmt *fmt, const char *str) {
    int32_t width = fmt->width;
    char pad = (fmt->flags & PAD_ZERO) == PAD_ZERO ? '0' : ' ';
    if (~fmt->flags & PAD_LEFT) {
        while (--width >= 0)
            format_out_char(fmt, pad);
    }
    while (*str)
        format_out_char(fmt, *str++);

    while (--width >= 0)
        format_out_char(fmt, pad);
}

static void format_out_dec(fmt *fmt, uint64_t num) {
    char buf[32];
    char *end = buf + sizeof(buf) - 1;
    char *start = end;
    *start = '\0';

    do {
        div_t d = div(num, 10);
        char c = '0' + d.rem;
        *--start = c;
        num = d.quot;
    } while (num > 0);

    fmt->width -= end - start;
    format_out_string(fmt, start);
}

static void format_out_hex(fmt *fmt, char type, uint64_t num) {
    char buf[32];
    char *end = buf + sizeof(buf) - 1;
    char *s = end;
    *s = '\0';

    do {
        uint8_t digit = num & 0xf;
        char c;
        if (digit < 10)
            c = '0' + digit;
        else if (type == 'x')
            c = 'a' + digit - 10;
        else
            c = 'A' + digit - 10;

        *--s = c;
        num >>= 4;
    } while (num > 0);

    fmt->width -= end - s;
    format_out_string(fmt, s);
}

static void format_out_pointer(fmt *fmt, void *p) {
    uint64_t n = (uintptr_t) p;
    format_out_hex(fmt, 'x', n);
}

int vsnprintf(char *str, size_t size, const char *format, va_list args) {
    fmt fmt;
    fmt.current = str;
    fmt.end = str + size - 1;

    for (;;) {
        char c = *format++;
        if (!c)
            break;
        if (c != '%') {
            format_out_char(&fmt, c);
            continue;
        }

        c = *format++;

        fmt.flags = 0;
        if (c == '-') {
            fmt.flags |= PAD_LEFT;
            c = *format++;
        } else if (c == '0') {
            fmt.flags |= PAD_ZERO;
            c = *format++;
        }

        fmt.width = -1;
        if (format_is_digit(c)) {
            int width = 0;
            do {
                width = width * 10 + c - '0';
                c = *format++;
            } while (format_is_digit(c));

            fmt.width = width;
        }

        bool isLongLong = false;

        if (c == 'l') {
            c = *format++;
            if (c == 'l') {
                c = *format++;
                isLongLong = true;
            }
        }

        char type = c;
        switch (type) {
            case '%':
                format_out_char(&fmt, '%');
                break;
            case 'c':
                c = va_arg(args, int);
                format_out_char(&fmt, c);
                break;
            case 's': {
                char *s = va_arg(args, char *);
                if (!s)
                    s = "(null)";

                if (fmt.width > 0) {
                    char *p = s;
                    while (*p)
                        ++p;

                    fmt.width -= p - s;
                }

                format_out_string(&fmt, s);
                break;
            }
            case 'd': {
                long long n;
                if (isLongLong)
                    n = va_arg(args, long long);
                else
                    n = va_arg(args, int);

                if (n < 0) {
                    format_out_char(&fmt, '-');
                    n = -n;
                }

                format_out_dec(&fmt, n);
                break;
            }
            case 'u': {
                unsigned long long n;
                if (isLongLong)
                    n = va_arg(args, unsigned long long);
                else
                    n = va_arg(args, unsigned int);

                format_out_dec(&fmt, n);
                break;
            }
            case 'x':
            case 'X': {
                unsigned long long n;
                if (isLongLong)
                    n = va_arg(args, unsigned long long);
                else
                    n = va_arg(args, unsigned int);

                format_out_hex(&fmt, type, n);
                break;
            }
            case 'p': {
                void *p = va_arg(args, void *);

                format_out_char(&fmt, '0');
                format_out_char(&fmt, 'x');
                format_out_pointer(&fmt, p);
                break;
            }
            default:
                return -1;
        }
    }

    if (fmt.current < fmt.end + 1)
        *fmt.current = '\0';

    return fmt.current - str;
}

int snprintf(char *str, size_t size, const char *format, ...) {
    va_list args;

    va_start(args, format);
    int len = vsnprintf(str, size, format, args);
    va_end(args);

    return len;
}

int vsscanf(const char *str, const char *fmt, va_list args) {
    int count = 0;

    for (;;) {
        char c = *fmt++;
        if (!c)
            break;

        if (format_is_space(c))
            while (format_is_space(*str))
                ++str;
        else if (c != '%') {
            match_literal:
            if (*str == '\0')
                goto end_of_input;

            if (*str != c)
                goto match_failure;

            ++str;
        } else {
            c = *fmt++;
            char type = c;
            switch (type) {
                case '%':
                    goto match_literal;
                case 'd': {
                    int sign = 1;

                    c = *str++;
                    if (c == '\0')
                        goto end_of_input;

                    if (c == '-') {
                        sign = -1;
                        c = *str++;
                    }

                    int n = 0;
                    while (format_is_digit(c)) {
                        n = n * 10 + c - '0';
                        c = *str++;
                    }

                    n *= sign;
                    --str;

                    int *result = va_arg(args, int *);
                    *result = n;
                    ++count;
                    break;
                }
                default:
                    return -1;
            }
        }
    }

    match_failure:
    return count;

    end_of_input:
    return count ? count : -1;
}

int sscanf(const char *str, const char *fmt, ...) {
    va_list args;

    va_start(args, fmt);
    int count = vsscanf(str, fmt, args);
    va_end(args);

    return count;
}

int64_t strtoul(const char *ptr_start, char **ptr_end, int base) {
    const char *cur = ptr_start;

    while (format_is_space(*cur))
        ++cur;

    bool isNegative = false;
    if (*cur == '-') {
        isNegative = true;
        ++cur;
    } else if (*cur == '+')
        ++cur;

    if (base == 0) {
        if (*cur == '0') {
            ++cur;
            if (*cur == 'x') {
                base = 16;
                ++cur;
            } else
                base = 8;
        } else
            base = 10;
    } else if (base == 16)
        if (*cur == '0' && *(cur + 1) == 'x')
            cur += 2;

    //TODO write to errno
    int64_t result = 0;
    bool done = false;
    while (!done) {
        char currentChar = *cur;
        int64_t currentVal = 0;
        if (currentChar >= '0' && currentChar <= '9')
            currentVal = currentChar - '0';
        else if (currentChar >= 'a' && currentChar <= 'z')
            currentVal = currentChar - 'a' + 10;
        else if (currentChar >= 'A' && currentChar <= 'Z')
            currentVal = currentChar - 'A' + 10;
        else
            done = true;

        if (!done) {
            if (currentVal >= base)
                done = true;//TODO handle error
            else {
                ++cur;
                result *= base;
                result += currentVal;
            }
        }
    }

    if (isNegative)
        result = -result;

    if (ptr_end != 0)
        *ptr_end = (char *) cur;

    return result;
}
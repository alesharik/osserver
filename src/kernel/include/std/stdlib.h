#ifndef OSSERVER_STDLIB_H
#define OSSERVER_STDLIB_H

typedef struct _div_t {
    int quot;
    int rem;
} div_t;

div_t div(int numer, int denom);

#endif //OSSERVER_STDLIB_H

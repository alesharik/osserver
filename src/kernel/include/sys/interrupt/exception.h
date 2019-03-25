#ifndef OSSERVER_EXCEPTION_H
#define OSSERVER_EXCEPTION_H

#include <stdint.h>

#if defined(__i386__)
struct registers {
    uint32_t di, si, bp, bx, dx, cx, ax;
    uint32_t intNum, errCode;
    uint32_t rip, cs, eflags, rsp, ss;
};
#define REGISTER_SIZE uint32_t
#elif defined(__amd64__)
struct registers {
    uint64_t di, si, bp, bx, dx, cx, ax;
    uint64_t intNum, errCode;
    uint64_t rip, cs, eflags, rsp, ss;
};
#define REGISTER_SIZE uint64_t
#endif

void handle_exception(REGISTER_SIZE cs, REGISTER_SIZE ss, REGISTER_SIZE ax,
                      REGISTER_SIZE cx, REGISTER_SIZE dx, REGISTER_SIZE bx,
                      REGISTER_SIZE bp, REGISTER_SIZE si, REGISTER_SIZE di,
                      REGISTER_SIZE interrupt, REGISTER_SIZE err);

#endif
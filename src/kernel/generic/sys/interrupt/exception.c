#include <graphics/renderer.h>
#include "sys/interrupt/exception.h"
#include "graphics/screen.h"

static const char *exception_description[20] = {
    [0] = "Divide By Zero Error",
    [1] = "Debug",
    [2] = "NMI",
    [3] = "Breakpoint",
    [4] = "Overflow",
    [5] = "Bound Range Exceeded",
    [6] = "Invalid Opcode",
    [7] = "Device Not Available",
    [8] = "Double Fault",
    [9] = "Coprocessor Segment Overrun",
    [10] = "Invalid TSS",
    [11] = "Segment Not Present",
    [12] = "Stack-Segment Fault",
    [13] = "General Protection",
    [14] = "Page Fault",
    [16] = "Floating Point Error",
    [17] = "Alignment Check",
    [18] = "Machine Check",
    [19] = "SIMD Exception"
};

void handle_exception(REGISTER_SIZE cs, REGISTER_SIZE ss, REGISTER_SIZE ax,
                      REGISTER_SIZE cx, REGISTER_SIZE dx, REGISTER_SIZE bx,
                      REGISTER_SIZE bp, REGISTER_SIZE si, REGISTER_SIZE di,
                      REGISTER_SIZE interrupt, REGISTER_SIZE err) {
    const char *name = "Unknown";
    if(err < 20)
        name = exception_description[err];//FIXME add register dump
    screen_write_string("\nError: ");
    screen_write_string(name);
}
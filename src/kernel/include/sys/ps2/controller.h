#ifndef OSSERVER_PS2_CONTROLLER_H
#define OSSERVER_PS2_CONTROLLER_H

#include <stdbool.h>
#include <stdint.h>

#define PS2_CONTROLLER_IRQ_FIRST_PORT 1
#define PS2_CONTROLLER_IRQ_SECOND_PORT 12

typedef enum {
    PS2_PORT_FIRST = 0,
    PS2_PORT_SECOND = 1
} ps2_port;

bool ps2_controller_working;

bool ps2_controller_has_second_port;

bool ps2_controller_init(void);

void ps2_controller_first_port_translation(bool enable);

bool ps2_controller_send(ps2_port port, uint8_t data);

///Expected to be called in IRQ
uint8_t ps2_controller_read();

bool ps2_controller_is_port_connected(ps2_port port);

void ps2_controller_set_port_connected(ps2_port port, bool is);

void cpu_reset();

#endif //OSSERVER_PS2_CONTROLLER_H

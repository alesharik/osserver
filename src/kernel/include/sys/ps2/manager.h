#ifndef OSSERVER_MANAGER_H
#define OSSERVER_MANAGER_H

#include "controller.h"

typedef enum {
    UNKNOWN,
    KEYBOARD,
    MOUSE,
    MOUSE_WITH_SCROLL,
    MOUSE_WITH_5_KEYS
} device_type;

void ps2_manager_init(void);

void ps2_manager_send_port_check(void);

device_type ps2_manager_get_device_type(ps2_port port);

void ps2_manager_send(ps2_port port, uint8_t data);

void __ps2_irq_first_port();

void __ps2_irq_second_port();

#endif //OSSERVER_MANAGER_H

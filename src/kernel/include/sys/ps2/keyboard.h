#ifndef OSSERVER_KEYBOARD_H
#define OSSERVER_KEYBOARD_H

#include <stdint.h>
#include <stdbool.h>
#include "controller.h"

void __ps2_keyboard_handle_byte(uint8_t devId, uint8_t byte);

void __ps2_keyboard_setup(uint8_t devId, bool scancode2);

void ps2_keyboard_set_led(ps2_port port, uint8_t id, bool on);

#endif //OSSERVER_KEYBOARD_H

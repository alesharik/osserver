#include "sys/ps2/controller.h"
#include "sys/acpi/acpi.h"
#include "sys/asm.h"
#include "sys/kernel.h"
#include "sys/time/ktimer.h"
#include <stdint.h>

#define PS2_CONTROLLER_DISABLE_PS_PORT_1 0xAD
#define PS2_CONTROLLER_DISABLE_PS_PORT_2 0xA7
#define PS2_CONTROLLER_ENABLE_PS_PORT_1 0xAE
#define PS2_CONTROLLER_ENABLE_PS_PORT_2 0xA8
#define PS2_CONTROLLER_READ_CONFIGURATION 0x20
#define PS2_CONTROLLER_WRITE_CONFIGURATION 0x60
#define PS2_CONTROLLER_TEST 0xAA
#define PS2_CONTROLLER_TEST_FIRST_PORT 0xAB
#define PS2_CONTROLLER_TEST_SECOND_PORT 0xA9
#define PS2_CONTROLLER_SEND_TO_2_PORT 0xD4

#define PS2_CONTROLLER_STATUS_OUTPUT_READY_BIT 1
#define PS2_CONTROLLER_STATUS_INPUT_FULL_BIT 2
#define PS2_CONTROLLER_STATUS_SYSTEM_BIT 4
#define PS2_CONTROLLER_STATUS_TIMEOUT_ERROR_BIT 64
#define PS2_CONTROLLER_STATUS_PARITY_ERROR_BIT 128

#define PS2_CONTROLLER_CONFIG_FIRST_PORT_INTERRUPT_BIT 1
#define PS2_CONTROLLER_CONFIG_SECOND_PORT_INTERRUPT_BIT 2
#define PS2_CONTROLLER_CONFIG_FIRST_PORT_CLOCK_BIT 16
#define PS2_CONTROLLER_CONFIG_SECOND_PORT_CLOCK_BIT 32
#define PS2_CONTROLLER_CONFIG_FIRST_PORT_TRANSLATION_BIT 64

#define PS2_PORT_TIMEOUT 20
uint32_t ps2_controller_timeouts[2] = {0, 0};

bool ps2_controller_working = false;
bool ps2_controller_has_second_port = false;

bool first_port_connected = false;
bool second_port_connected = false;

static void timeout_timer() {
    if (ps2_controller_timeouts[0] > 0)
        ps2_controller_timeouts[0] -= 5;
    if (ps2_controller_timeouts[1] > 0)
        ps2_controller_timeouts[0] -= 5;
}

static timer_task timeout_task = {
        .interval = 5,
        .func = timeout_timer
};

static inline uint8_t get_status() {
    return inb(0x64);
}

static bool send_command(uint8_t command, ps2_port port) {
    ps2_controller_timeouts[port] = PS2_PORT_TIMEOUT;
    while (((get_status() & PS2_CONTROLLER_STATUS_INPUT_FULL_BIT) == PS2_CONTROLLER_STATUS_INPUT_FULL_BIT) &&
            ps2_controller_timeouts[port] > 0) asm("pause");
    if((get_status() & PS2_CONTROLLER_STATUS_INPUT_FULL_BIT) == PS2_CONTROLLER_STATUS_INPUT_FULL_BIT)
        return false;
    outb(0x64, command);
    return true;
}

static bool send_data_command(uint8_t command, uint8_t data, ps2_port port) {
    ps2_controller_timeouts[port] = PS2_PORT_TIMEOUT;
    while (((get_status() & PS2_CONTROLLER_STATUS_INPUT_FULL_BIT) == PS2_CONTROLLER_STATUS_INPUT_FULL_BIT) &&
            ps2_controller_timeouts[port] > 0) asm("pause");
    if((get_status() & PS2_CONTROLLER_STATUS_INPUT_FULL_BIT) == PS2_CONTROLLER_STATUS_INPUT_FULL_BIT)
        return false;
    outb(0x64, command);
    outb(0x60, data);
    return true;
}

static bool send_data(uint8_t data, ps2_port port) {
    while (((get_status() & PS2_CONTROLLER_STATUS_INPUT_FULL_BIT) == PS2_CONTROLLER_STATUS_INPUT_FULL_BIT) &&
            ps2_controller_timeouts[port] > 0) asm("pause");
    if((get_status() & PS2_CONTROLLER_STATUS_INPUT_FULL_BIT) == PS2_CONTROLLER_STATUS_INPUT_FULL_BIT)
        return false;
    outb(0x60, data);
    return true;
}

static uint8_t get_response(ps2_port port) {
    ps2_controller_timeouts[port] = PS2_PORT_TIMEOUT;
    while (((get_status() & PS2_CONTROLLER_STATUS_OUTPUT_READY_BIT) !=
            PS2_CONTROLLER_STATUS_OUTPUT_READY_BIT) && ps2_controller_timeouts[port] > 0) asm("pause");
    if ((get_status() & PS2_CONTROLLER_STATUS_OUTPUT_READY_BIT) !=
        PS2_CONTROLLER_STATUS_OUTPUT_READY_BIT)
        return -1;
    return inb(0x60);
}

bool ps2_controller_init() {
    if (!acpi_ps2_controller_exists)
        return false;
    if ((get_status() & PS2_CONTROLLER_STATUS_SYSTEM_BIT) != PS2_CONTROLLER_STATUS_SYSTEM_BIT)
        kpanic("PS/2 controller hasn't been checked while POST");

    send_command(PS2_CONTROLLER_DISABLE_PS_PORT_1, PS2_PORT_FIRST);
    send_command(PS2_CONTROLLER_DISABLE_PS_PORT_2, PS2_PORT_SECOND);

    inb(0x60); //Cleanup data register

    send_command(PS2_CONTROLLER_READ_CONFIGURATION, PS2_PORT_FIRST);
    uint8_t config = get_response(PS2_PORT_FIRST);
    config &= ~PS2_CONTROLLER_CONFIG_FIRST_PORT_INTERRUPT_BIT;
    config &= ~PS2_CONTROLLER_CONFIG_SECOND_PORT_INTERRUPT_BIT;
    config &= ~PS2_CONTROLLER_CONFIG_FIRST_PORT_TRANSLATION_BIT;
    send_data_command(PS2_CONTROLLER_WRITE_CONFIGURATION, config, PS2_PORT_FIRST);

    bool hasSecondPort =
            (config & PS2_CONTROLLER_CONFIG_SECOND_PORT_CLOCK_BIT) != PS2_CONTROLLER_CONFIG_SECOND_PORT_CLOCK_BIT;

    send_data_command(PS2_CONTROLLER_WRITE_CONFIGURATION, config, PS2_PORT_FIRST);

    send_command(PS2_CONTROLLER_TEST_FIRST_PORT, PS2_PORT_FIRST);
    bool firstPortWorking = get_response(PS2_PORT_FIRST) == 0;
    bool secondPortWorking = false;
    if (hasSecondPort) {
        send_command(PS2_CONTROLLER_TEST_SECOND_PORT, PS2_PORT_SECOND);
        secondPortWorking = get_response(PS2_PORT_SECOND) == 0;
    }

    if (!firstPortWorking && !secondPortWorking) {
        ps2_controller_working = false;
        //TODO report
        return false;
    }

    send_command(PS2_CONTROLLER_ENABLE_PS_PORT_1, PS2_PORT_FIRST);
    send_command(PS2_CONTROLLER_ENABLE_PS_PORT_2, PS2_PORT_SECOND);

    config |= PS2_CONTROLLER_CONFIG_SECOND_PORT_INTERRUPT_BIT;
    config |= PS2_CONTROLLER_CONFIG_FIRST_PORT_INTERRUPT_BIT;
    send_data_command(PS2_CONTROLLER_WRITE_CONFIGURATION, config, PS2_PORT_FIRST);

    uint8_t tmp = inb(0x61);
    outb(0x61, tmp | 0x80);
    outb(0x61, tmp & 0x7F);
    inb(0x60);
    ps2_controller_working = true;
    ps2_controller_has_second_port = true;

    ktimer_add_task(&timeout_task);
    return true;
}

void ps2_controller_first_port_translation(bool enable) {
    send_command(PS2_CONTROLLER_READ_CONFIGURATION, PS2_PORT_FIRST);
    uint8_t config = get_response(PS2_PORT_FIRST);
    if (enable)
        config |= PS2_CONTROLLER_CONFIG_FIRST_PORT_TRANSLATION_BIT;
    else
        config &= ~PS2_CONTROLLER_CONFIG_FIRST_PORT_TRANSLATION_BIT;
    send_data_command(PS2_CONTROLLER_WRITE_CONFIGURATION, config, PS2_PORT_FIRST);
}

bool ps2_controller_send(ps2_port port, uint8_t data) {
    if ((port == PS2_PORT_FIRST && !first_port_connected) || (port == PS2_PORT_SECOND && !second_port_connected))
        return true;
    if (port == PS2_PORT_FIRST)
        return send_data(data, port);
    else if (port == PS2_PORT_SECOND)
        return send_data_command(PS2_CONTROLLER_SEND_TO_2_PORT, data, port);
    else
        return false;
}

uint8_t ps2_controller_read() {
    return inb(0x60);
}

void ps2_controller_set_port_connected(ps2_port port, bool is) {
    if (port == PS2_PORT_FIRST)
        first_port_connected = is;
    else if (port == PS2_PORT_SECOND)
        second_port_connected = is;
}

bool ps2_controller_is_port_connected(ps2_port port) {
    if (port == PS2_PORT_FIRST)
        return first_port_connected;
    else if (port == PS2_PORT_SECOND)
        return second_port_connected;
    else
        return false;
}

void cpu_reset() {
    send_command(0xFE, PS2_PORT_FIRST);
}
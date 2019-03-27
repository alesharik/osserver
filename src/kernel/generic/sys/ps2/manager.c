#include "sys/ps2/manager.h"
#include "sys/ps2/controller.h"
#include "sys/time/ktimer.h"
#include "sys/interrupt/lapic.h"
#include "sys/time/pit.h"
#include "sys/ps2/keyboard.h"
#include "graphics/screen.h"
#include <stdint.h>

#define PS2_DEVICE_RESPONSE_OK 0xAA
#define PS2_DEVICE_RESPONSE_ERROR 0xFC
#define PS2_DEVICE_RESPONSE_ACK 0xFA
#define PS2_DEVICE_RESPONSE_RESEND 0xFE
#define PS2_DEVICE_RESPONSE_ECHO 0xEE

#define PS2_DEVICE_COMMAND_DISABLE_SCANNING 0xF5
#define PS2_DEVICE_COMMAND_IDENTIFY 0xF2
#define PS2_DEVICE_COMMAND_ECHO 0xEE
#define PS2_DEVICE_COMMAND_RESET 0xFF

#define PS2_DEVICE_MAX_PING 3000

typedef enum {
    NOT_CONNECTED,
    ERRORED,
    CONNECTED,
    SCAN_DISABLED,
    IDENTIFY_IN_PROGRESS,
    READY
} device_state;

typedef struct device_queue_item_struct {
    uint8_t command;
    struct device_queue_item_struct *next;
} device_queue_item;

#define PS2_DEVICE_MAX_BUFFER_SIZE 4

typedef struct {
    device_queue_item *item;
    device_state state;
    device_type type;
    uint64_t last_echo;
    uint8_t buffer_size;
    uint8_t buffer[PS2_DEVICE_MAX_BUFFER_SIZE];
    bool supported;
} device;

device devices[2] = {
        {
                .item = 0,
                .state = NOT_CONNECTED,
                .type = UNKNOWN,
                .last_echo = 0,
                .buffer_size = 0,
                .supported = false
        },
        {
                .item = 0,
                .state = NOT_CONNECTED,
                .type = UNKNOWN,
                .last_echo = 0,
                .buffer_size = 0,
                .supported = false
        }
};

///In milliseconds
#define PS2_MAX_TIMEOUT 20
#define PS2_INTERVAL 5
int32_t ps2_manager_timeouts[2] = {-1, -1};

static void init_device(uint8_t devId, uint8_t *id, int8_t size) {
    device *dev = &devices[devId];
    if(size == 0) {
        dev->supported = true;
        dev->type = KEYBOARD;
        __ps2_keyboard_setup(devId, false);
    } else if(size == 1) {
        dev->supported = true;
        if(id[0] == 0x00)
            dev->type = MOUSE;
        else if(id[0] == 0x03)
            dev->type = MOUSE_WITH_SCROLL;
        else if(id[0] == 0x04)
            dev->type = MOUSE_WITH_5_KEYS;
        else
            dev->supported = false;
    } else if(size == 2) {
        if(id[0] == 0xAB) {
            if(id[1] == 0x41 || id[1] == 0xC1) {
                dev->supported = true;
                dev->type = KEYBOARD;
                __ps2_keyboard_setup(devId, false);
            } else if(id[1] == 0x83) {
                dev->supported = true;
                dev->type = KEYBOARD;
                __ps2_keyboard_setup(devId, true);
            }
        }
    }
    dev->state = READY;
}

static void route_to_device_driver(uint8_t devId, uint8_t byte) {
    if(devices[devId].type == KEYBOARD)
        __ps2_keyboard_handle_byte(devId, byte);
}

static void timeout_timer() {
    if (ps2_manager_timeouts[0] > 0)
        ps2_manager_timeouts[0] -= PS2_INTERVAL;
    else if(ps2_manager_timeouts[0] == 0) {
        init_device(0, devices[0].buffer, devices[0].buffer_size);
        ps2_manager_timeouts[0] = -1;
    }

    if (ps2_manager_timeouts[1] > 0)
        ps2_manager_timeouts[1] -= PS2_INTERVAL;
    else if(ps2_manager_timeouts[1] == 0) {
        init_device(1, devices[1].buffer, devices[1].buffer_size);
        ps2_manager_timeouts[1] = -1;
    }
}

timer_task timeout_task = {
        .func = timeout_timer,
        .interval = PS2_INTERVAL
};

timer_task port_check_task = {
        .func = ps2_manager_send_port_check,
        .interval = 1000
};

static void handle_irq(unsigned int devId) {
    ps2_controller_set_port_connected(devId, true);
    uint8_t cmd = ps2_controller_read();
    device *dev = &devices[devId];
    
    if(dev->state == NOT_CONNECTED && !(cmd == PS2_DEVICE_RESPONSE_OK || cmd == PS2_DEVICE_RESPONSE_ERROR)) { //Keyboard doesn't want to configure because it already is
        dev->state = READY;
        dev->supported = true;
        dev->type = KEYBOARD;
        __ps2_keyboard_setup(devId, true);
        __ps2_keyboard_handle_byte(devId, cmd);
        return;
    }
    
    if (cmd == PS2_DEVICE_RESPONSE_ECHO)
        dev->last_echo = pit_get_time_since_startup();
    else if (cmd == PS2_DEVICE_RESPONSE_RESEND) {
        if (dev->item != 0)
            ps2_controller_send(devId, dev->item->command);
    } else if (cmd == PS2_DEVICE_RESPONSE_OK) {
        dev->state = CONNECTED;
        ps2_controller_send(devId, PS2_DEVICE_COMMAND_DISABLE_SCANNING);
    } else if (cmd == PS2_DEVICE_RESPONSE_ERROR) {
        dev->state = ERRORED;
        ps2_controller_send(devId, PS2_DEVICE_COMMAND_RESET);
    } else if (cmd == PS2_DEVICE_RESPONSE_ACK) {
        if (dev->state == CONNECTED) {
            dev->state = SCAN_DISABLED;
            ps2_controller_send(devId, PS2_DEVICE_COMMAND_IDENTIFY);
        } else if (dev->state == SCAN_DISABLED) {
            dev->state = IDENTIFY_IN_PROGRESS;
            ps2_manager_timeouts[devId] = PS2_MAX_TIMEOUT;
        }
    } else {
        if(dev->state == IDENTIFY_IN_PROGRESS) {
            if (dev->buffer_size >= PS2_DEVICE_MAX_BUFFER_SIZE) {
                //FIXME report buffer overflow
            } else {
                dev->buffer[dev->buffer_size] = cmd;
                dev->buffer_size++;
            }
        } else if(dev->state == READY && dev->supported)
            route_to_device_driver(devId, cmd);
    }

    lapic_reset_irq();
}

void ps2_manager_init() {
    ktimer_add_task(&timeout_task);
    ktimer_add_task(&port_check_task);
    ps2_manager_send_port_check();
}

void __ps2_irq_first_port() {
    handle_irq(0);
}

void __ps2_irq_second_port() {
    handle_irq(1);
}

void ps2_manager_send_port_check() {
    unsigned long long int time = pit_get_time_since_startup();
    if ((time - devices[0].last_echo) > PS2_DEVICE_MAX_PING) {
        devices[0].type = UNKNOWN;
        devices[0].state = NOT_CONNECTED;
        devices[0].item = 0;
        ps2_controller_set_port_connected(PS2_PORT_FIRST, false);
    }
    if (ps2_controller_has_second_port && (time - devices[1].last_echo) > PS2_DEVICE_MAX_PING) {
        devices[1].type = UNKNOWN;
        devices[1].state = NOT_CONNECTED;
        devices[1].item = 0;
        ps2_controller_set_port_connected(PS2_PORT_SECOND, false);
    }
    if (ps2_controller_is_port_connected(PS2_PORT_FIRST))
        ps2_controller_send(PS2_PORT_FIRST, PS2_DEVICE_COMMAND_ECHO);
    else
        ps2_controller_send(PS2_PORT_FIRST, PS2_DEVICE_COMMAND_RESET);
    if (ps2_controller_has_second_port) {
        if (ps2_controller_is_port_connected(PS2_PORT_FIRST))
            ps2_controller_send(PS2_PORT_SECOND, PS2_DEVICE_COMMAND_ECHO);
        else
            ps2_controller_send(PS2_PORT_SECOND, PS2_DEVICE_COMMAND_RESET);
    }
}

device_type ps2_manager_get_device_type(ps2_port port) {
    return devices[port].type;
}

void ps2_manager_send(ps2_port port, uint8_t data) {
    //FIXME   
}
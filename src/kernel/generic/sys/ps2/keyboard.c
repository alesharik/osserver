#include <sys/kernel.h>
#include "sys/ps2/keyboard.h"
#include "sys/ps2/controller.h"

#include "graphics/screen.h"

uint8_t leds[2] = {0, 0};
uint8_t buffers[2][6];

void __ps2_keyboard_setup(uint8_t devId, bool scancode2) {
    if(scancode2)
        ps2_controller_first_port_translation(true);
    //TODO check outs
    bool ok = true;
    ok &= ps2_controller_send(devId, 0xF0);
    ok &= ps2_controller_send(devId, 1);
    ok &= ps2_controller_send(devId, 0xF3);
    ok &= ps2_controller_send(devId, 0x11111b | 0x0000000 | 0b00000000);//TODO add typematic config
    ok &= ps2_controller_send(devId, 0xED);
    ok &= ps2_controller_send(devId, 0);
    ok &= ps2_controller_send(devId, 0xF4);
    if(!ok)
        kpanic("asd");
}

void __ps2_keyboard_handle_byte(uint8_t devId, uint8_t byte) {
    screen_setcursor(300, 300);
    screen_write_string("Keyboard: ");
    screen_write_char(byte);
    screen_setcursor(0, 0);
}

void ps2_keyboard_set_led(ps2_port port, uint8_t id, bool on) {
    uint8_t led = leds[port];
    if(on)
        led |= 1 >> id;
    else
        led &= ~(1 >> id);
    ps2_controller_send(port, 0xED);
    ps2_controller_send(port, led);
    leds[port] = led;
}
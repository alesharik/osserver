#include <stdint.h>
#include <stdatomic.h>
#include <stdbool.h>
#include "asm.h"

#define CURRENT_YEAR        2018

uint8_t century_register = 0x00;

unsigned char _second;
unsigned char _minute;
unsigned char _hour;
unsigned char _day;
unsigned char _month;
unsigned int _year;
atomic_bool _update;

static inline void _rtc_wait_for_update(void) {
    while (_update);
}

char rtc_get_seconds() {
    _rtc_wait_for_update();
    return _second;
}

char rtc_get_minute() {
    _rtc_wait_for_update();
    return _minute;
}

char rtc_get_hour() {
    _rtc_wait_for_update();
    return _hour;
}

char rtc_get_day() {
    _rtc_wait_for_update();
    return _day;
}

char rtc_get_month() {
    _rtc_wait_for_update();
    return _month;
}

int rtc_get_year() {
    _rtc_wait_for_update();
    return _year;
}

enum {
    _rtc_cmos_address = 0x70,
    _rtc_cmos_data    = 0x71
};

int _rtc_update_in_progress_flag() {
    outb(_rtc_cmos_address, 0x0A);
    return (inb(_rtc_cmos_data) & 0x80);
}

unsigned char _rtc_get_register(uint8_t reg) {
    outb(_rtc_cmos_address, reg);
    return inb(_rtc_cmos_data);
}

void _rtc_read() {
    _update = true;
    unsigned char century;
    unsigned char last_second;
    unsigned char last_minute;
    unsigned char last_hour;
    unsigned char last_day;
    unsigned char last_month;
    unsigned int last_year;
    unsigned char last_century;
    unsigned char registerB;

    // Note: This uses the "read registers until you get the same values twice in a row" technique
    //       to avoid getting dodgy/inconsistent values due to RTC updates

    while (_rtc_update_in_progress_flag());                // Make sure an update isn't in progress
    _second = _rtc_get_register(0x00);
    _minute = _rtc_get_register(0x02);
    _hour = _rtc_get_register(0x04);
    _day = _rtc_get_register(0x07);
    _month = _rtc_get_register(0x08);
    _year = _rtc_get_register(0x09);

    if(century_register != 0)
        century = _rtc_get_register(century_register);
    else
        century = 0;

    do {
        last_second = _second;
        last_minute = _minute;
        last_hour = _hour;
        last_day = _day;
        last_month = _month;
        last_year = _year;
        last_century = century;

        while (_rtc_update_in_progress_flag());           // Make sure an update isn't in progress
        _second = _rtc_get_register(0x00);
        _minute = _rtc_get_register(0x02);
        _hour = _rtc_get_register(0x04);
        _day = _rtc_get_register(0x07);
        _month = _rtc_get_register(0x08);
        _year = _rtc_get_register(0x09);
        if(century_register != 0)
            century = _rtc_get_register(century_register);
    } while((last_second != _second)
            || (last_minute != _minute)
            || (last_hour != _hour)
            || (last_day != _day)
            || (last_month != _month)
            || (last_year != _year)
            || (last_century != century)
            );

    registerB = _rtc_get_register(0x0B);

    // Convert BCD to binary values if necessary

    if (!(registerB & 0x04)) {
        _second = (unsigned char) ((_second & 0x0F) + ((_second / 16) * 10));
        _minute = (unsigned char) ((_minute & 0x0F) + ((_minute / 16) * 10));
        _hour = (unsigned char) (((_hour & 0x0F) + (((_hour & 0x70) / 16) * 10) ) | (_hour & 0x80));
        _day = (unsigned char) ((_day & 0x0F) + ((_day / 16) * 10));
        _month = (unsigned char) ((_month & 0x0F) + ((_month / 16) * 10));
        _year = ((_year & 0x0F) + ((_year / 16) * 10));
        if(century_register != 0)
            century = (unsigned char) ((century & 0x0F) + ((century / 16) * 10));
    }

    // Convert 12 hour clock to 24 hour clock if necessary

    if (!(registerB & 0x02) && (_hour & 0x80))
        _hour = (unsigned char) (((_hour & 0x7F) + 12) % 24);

    // Calculate the full (4-digit) year

    if(century_register != 0)
        _year += century * 100;
    else {
        _year += (CURRENT_YEAR / 100) * 100;
        if(_year < CURRENT_YEAR)
            _year += 100;
    }
    _update = false;
}
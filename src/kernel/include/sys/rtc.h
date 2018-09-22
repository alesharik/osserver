#ifndef OSSERVER_RTC_H
#define OSSERVER_RTC_H

#include "asm.h"
#include <stdatomic.h>

#define CURRENT_YEAR        2018

extern uint8_t century_register;

extern unsigned char _second;
extern unsigned char _minute;
extern unsigned char _hour;
extern unsigned char _day;
extern unsigned char _month;
extern unsigned int _year;
extern atomic_bool _update;

char rtc_get_seconds();

char rtc_get_minute();

char rtc_get_hour();

char rtc_get_day();

char rtc_get_month();

int rtc_get_year();

int _rtc_update_in_progress_flag();

unsigned char _rtc_get_register(uint8_t reg);

void _rtc_read();

#endif //OSSERVER_RTC_H

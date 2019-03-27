#ifndef OSSERVER_PIT_H
#define OSSERVER_PIT_H

void pit_init();

void pit_sleep(unsigned int ms);

/// Return time since PIC startup
/// \return time in milliseconds
unsigned long long int pit_get_time_since_startup();

void __pit_tick();

#endif //OSSERVER_PIT_H

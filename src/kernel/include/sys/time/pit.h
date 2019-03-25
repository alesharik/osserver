#ifndef OSSERVER_PIT_H
#define OSSERVER_PIT_H

void pit_init();

void pit_sleep(unsigned int ms);

void __pit_tick();

#endif //OSSERVER_PIT_H

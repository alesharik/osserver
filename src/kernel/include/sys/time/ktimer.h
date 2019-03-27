#ifndef OSSERVER_KTIMER_H
#define OSSERVER_KTIMER_H

#include <stdint.h>

typedef struct timer_task_struct {
    void (*func)(void);
    /// \internal
    struct timer_task_struct *_next;
    /// Interval in milliseconds
    uint32_t interval;
    /// \internal
    uint32_t _counter;
} timer_task;

void ktimer_add_task(timer_task *task);

void ktimer_remove_task(timer_task *task);

void __ktimer_tick(void);

#endif //OSSERVER_KTIMER_H

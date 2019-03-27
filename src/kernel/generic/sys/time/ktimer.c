#include "sys/time/ktimer.h"

timer_task *root = 0;

void ktimer_add_task(timer_task *task) {
    task->_counter = task->interval;
    if(root == 0)
        root = task;
    else {
        timer_task *i = root;
        while (i->_next != 0)
            i = i->_next;
        i->_next = task;
    }
}

void ktimer_remove_task(timer_task *task) {
    if(root == task)
        root = root->_next;
    else {
        timer_task *task = root;
        while (task->_next != 0) {
            if(task->_next == task) {
                task->_next = task->_next->_next;
                return;
            }
            task = task->_next;
        }
    }
}

void __ktimer_tick(void) {
    timer_task *task = root;
    if(task == 0)
        return;
    do {
        task->_counter--;
        if(task->_counter == 0) {
            task->func();
            task->_counter = task->interval;
        }
    } while ((task = task->_next) != 0);
}
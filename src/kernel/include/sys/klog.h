#ifndef OSSERVER_KLOG_H
#define OSSERVER_KLOG_H

void klog_init();

void klog(const char *msg, ...);

void kwarning(const char *msg, ...);

void kerror(const char *msg, ...);

#endif //OSSERVER_KLOG_H

#ifndef __INOTIFY_UTILS_H
#define __INOTIFY_UTILS_H

#include "event_queue.h"

void print_event (struct inotify_event event);
int read_event (int fd, struct inotify_event *event);
int event_check (int fd);
int read_and_print_events (queue_t q, int fd);
int watch_dir (int fd, const char *dirname, unsigned long mask);
int ignore_wd (int fd, int wd);
int dev_stats (int fd);
int dev_setdebug (int fd, int debug);
int close_dev (int fd);
int open_dev ();


#endif

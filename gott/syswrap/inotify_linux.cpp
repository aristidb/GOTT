// (C) 2006 by Aristid Breitkreuz (aribrei@arcor.de)
#include "inotify_linux.hpp"
#include "errno.hpp"
extern "C" {
#include "inotify-syscalls.h"
}

int gott::inotify_init_linux() {
  int result = inotify_init();
  if (result < 0)
    throw_errno_exception();
  return result;
}

boost::uint32_t 
gott::inotify_add_watch_linux(int fd, const char *p, boost::uint32_t m) {
  int result = inotify_add_watch(fd, p, m);
  if (result < 0)
    throw_errno_exception();
  return result;
}

void gott::inotify_rm_watch_linux(int fd, boost::uint32_t wd) {
  if (inotify_rm_watch(fd, wd) < 0)
    throw_errno_exception();
}

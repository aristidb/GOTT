// (C) 2006 by Aristid Breitkreuz (aribrei@arcor.de)
#include "epoll_linux.hpp"
#include "errno.hpp"

int gott::epoll_create_linux(int size) {
  int result = epoll_create(size);
  if (result == -1)
    throw_errno_exception();
  return result;
}

void gott::epoll_ctl_linux(int epfd, int op, int fd, epoll_event *event) {
  if (epoll_ctl(epfd, op, fd, event) == -1)
    throw_errno_exception();
}

int gott::epoll_wait_linux(int epfd, epoll_event *ev, int maxev, int tmo) {
  int result = epoll_wait(epfd, ev, maxev, tmo);
  if (result == -1)
    throw_errno_exception();
  return result;
}

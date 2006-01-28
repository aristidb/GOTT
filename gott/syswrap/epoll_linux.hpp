// (C) 2006 by Aristid Breitkreuz (aribrei@arcor.de)
#ifndef GOTT_SYSWRAP_EPOLL_LINUX_HPP
#define GOTT_SYSWRAP_EPOLL_LINUX_HPP

#include <gott/visibility.hpp>
#include <sys/epoll.h>

namespace gott {

GOTT_EXPORT int epoll_create_linux(int size);
GOTT_EXPORT void epoll_ctl_linux(int epfd, int op, int fd, epoll_event *event);
GOTT_EXPORT int epoll_wait_linux(int epfd, epoll_event *ev, int maxev, int tmo);

}

#endif

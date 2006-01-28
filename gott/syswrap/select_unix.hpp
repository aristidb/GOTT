// (C) 2006 by Aristid Breitkreuz (aribrei@arcor.de)
#ifndef GOTT_SYSWRAP_SELECT_UNIX_HPP
#define GOTT_SYSWRAP_SELECT_UNIX_HPP

#include <gott/visibility.hpp>
#include <sys/select.h>

namespace gott {

GOTT_EXPORT
int select_unix(
    int n,
    fd_set *readfds,
    fd_set *writefds,
    fd_set *exceptfds,
    struct timeval *timeout);

}

#endif

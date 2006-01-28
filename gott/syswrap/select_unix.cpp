// (C) 2006 by Aristid Breitkreuz (aribrei@arcor.de)
#include "select_unix.hpp"
#include "errno.hpp"

int gott::select_unix(
    int n,
    fd_set *readfds,
    fd_set *writefds,
    fd_set *exceptfds,
    struct timeval *timeout) {
  int result = ::select(n, readfds, writefds, exceptfds, timeout);
  if (result == -1)
    throw_errno_exception();
  return result;
}

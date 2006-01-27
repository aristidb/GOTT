// (C) 2006 by Aristid Breitkreuz (aribrei@arcor.de)
#include "read_write_unix.hpp"
#include "errno.hpp"
#include <unistd.h>

using std::size_t;

size_t gott::read_unix(int fd, void *buf, size_t len) {
  ssize_t result = ::read(fd, buf, len);
  if (result == -1)
    throw_errno_exception();
  return result;
}

size_t gott::write_unix(int fd, void const *buf, size_t len) {
  ssize_t result = ::write(fd, buf, len);
  if (result == -1)
    throw_errno_exception();
  return result;
}

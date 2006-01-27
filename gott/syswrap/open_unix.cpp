// (C) 2006 by Aristid Breitkreuz (aribrei@arcor.de)
#include "open_unix.hpp"
#include "errno.hpp"
#include <sys/stat.h>
#include <fcntl.h>

int gott::open_unix(char const *filename, int flags, mode_t mode) {
  int result = ::open(filename, flags, mode);
  if (result == -1)
    throw_errno_exception();
  return result;
}

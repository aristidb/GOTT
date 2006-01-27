// (C) 2006 by Aristid Breitkreuz (aribrei@arcor.de)
#include "scoped_unix_file.hpp"
#include "errno.hpp"
#include <unistd.h>

using gott::scoped_unix_file;

scoped_unix_file::~scoped_unix_file() {
  try {
    close();
  } catch (system_error const &) {
    // TODO: log?
  }
}

void scoped_unix_file::close() {
  if (fd == -1)
    return;
  if (::close(fd) == -1)
    throw_errno_exception();
}

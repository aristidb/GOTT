// (C) 2006 by Aristid Breitkreuz (aribrei@arcor.de)

#include "errno.hpp"
#include "scoped_unix_file.hpp"
#include "open_unix.hpp"
#include "read_write_unix.hpp"
#include "sysconf_posix.hpp"
#include <iostream>
#include <errno.h>

static void throw_catch() {
  try {
    gott::throw_errno_exception();
  } catch (gott::system_error const &e) {
    std::cout << "Caught: " << e.what() << std::endl;
  }
  errno = 0;
}

int main() {
  close(666);
  throw_catch();
  gott::scoped_unix_file f(gott::creat_unix("testfile", 00666));
  gott::write_unix(f.access(), "Zero-terminated file.\n");
  f.reset(gott::open_unix("testfile", O_RDONLY));
  char buf[1024];
  std::size_t len = gott::read_unix(f.access(), buf);
  std::cout << "Read " << len << " characters: " << buf;
}

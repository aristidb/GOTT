// (C) 2006 by Aristid Breitkreuz (aribrei@arcor.de)

#include "errno.hpp"
#include "scoped_unix_file.hpp"
#include "open_unix.hpp"
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
  std::cout << f.access() << std::endl;
}

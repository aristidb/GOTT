// (C) 2006 by Aristid Breitkreuz (aribrei@arcor.de)

#include "errno.hpp"
#include <iostream>
#include <errno.h>

static void throw_catch() {
  try {
    gott::throw_errno_exception();
  } catch (gott::system_error const &e) {
    std::cerr << "Caught: " << e.what() << '\n';
  }
  errno = 0;
}

int main() {
  close(666);
  throw_catch();
}

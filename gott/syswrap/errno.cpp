// (C) 2006 by Aristid Breitkreuz (aribrei@arcor.de)
#include "errno.hpp"

#include <errno.h>
#include <string.h>

#include <string>

#include <iostream>

using gott::errno_exception;

void gott::throw_errno_exception() {
  if (errno == 0)
    return;
  throw errno_exception(errno);
}

namespace {
  std::string desc(int n) {
    return strerror(n); // TODO threadsafe?
  }
}

errno_exception::errno_exception(int n) : system_error(desc(n)), num(n) {}
errno_exception::~errno_exception() throw() {}

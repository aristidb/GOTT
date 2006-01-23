// (C) 2006 by Aristid Breitkreuz (aribrei@arcor.de)
#include "errno.hpp"
#include <errno.h>
#include <string.h>
#include <string>

using gott::errno_exception;

void gott::throw_errno_exception() {
  if (errno == 0)
    return;
  throw errno_exception(errno);
}

namespace {
  std::string desc(int n) {
    std::string result(32, '\0');
    for (;;) {
      if (strerror_r(n, &result[0], result.size()) == 0)
        break;
      switch (errno) {
      case ERANGE:
        result.assign(result.size() * 2, '\0');
        break;
      default:
        gott::throw_errno_exception();
        break;
      }
    }
    return result;
  }
}

errno_exception::errno_exception(int n) : std::runtime_error(desc(n)), num(n) {}
errno_exception::~errno_exception() throw() {}

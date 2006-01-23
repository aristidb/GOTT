// (C) 2006 by Aristid Breitkreuz (aribrei@arcor.de)
#ifndef GOTT_SYSWRAP_ERRNO_HPP
#define GOTT_SYSWRAP_ERRNO_HPP

#include <gott/visibility.hpp>
#include <stdexcept>

namespace gott {
GOTT_EXPORT void throw_errno_exception();

class GOTT_EXPORT errno_exception : public std::runtime_error {
public:
  GOTT_LOCAL int number() const { return num; }
  ~errno_exception() throw();
private:
  errno_exception(int);
  friend void throw_errno_exception();
private:
  int num;
};

}

#endif

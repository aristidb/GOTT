// (C) 2006 by Aristid Breitkreuz (aribrei@arcor.de)
#ifndef GOTT_SYSWRAP_ERRNO_HPP
#define GOTT_SYSWRAP_ERRNO_HPP

#include <gott/visibility.hpp>
#include "system_error.hpp"

namespace gott {

/**
 * If there is an errno error pending (errno != 0), throw the appropriate 
 * exception. Does not reset errno.
 */
GOTT_EXPORT void throw_errno_exception();

/**
 * Exception class for errno errors.
 */
class GOTT_EXPORT errno_exception : public system_error {
public:
  /// The errno of the error.
  GOTT_LOCAL int number() const { return num; }
  /// Destructor.
  ~errno_exception() throw();
private:
  errno_exception(int);
  friend void throw_errno_exception();
private:
  int num;
};

}

#endif

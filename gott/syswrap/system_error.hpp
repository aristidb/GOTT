// (C) 2006 by Aristid Breitkreuz (aribrei@arcor.de)
#ifndef GOTT_SYSWRAP_SYSERR_HPP
#define GOTT_SYSWRAP_SYSERR_HPP

#include <gott/visibility.hpp>
#include <stdexcept>

namespace gott {

/**
 * Exception base class for system errors.
 */
class GOTT_EXPORT system_error : public std::runtime_error {
protected:
  system_error(std::string const &);
  ~system_error() throw();
};

};

#endif

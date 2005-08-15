// Copyright (C) 2004-2005 by Aristid Breitkreuz (aribrei@arcor.de)
// Content: TDL common base
// Authors: Aristid Breitkreuz
//
// This file is part of the Gott Project (http://gott.sf.net)
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#ifndef GOTT_UTIL_TDL_EXCEPTIONS_HPP
#define GOTT_UTIL_TDL_EXCEPTIONS_HPP

#include <gott/util/visibility.hpp>
#include <boost/scoped_ptr.hpp>
#include <ntl.h>

namespace gott {
class string;

namespace tdl {

class GOTT_EXPORT tdl_exception : public std::exception {
public:
  tdl_exception(string const &msg) GOTT_LOCAL;
  tdl_exception(tdl_exception const &) GOTT_LOCAL;
  ~tdl_exception() throw();
  char const *what() const throw();

private:
  class IMPL;
  boost::scoped_ptr<IMPL> p;

  tdl_exception() GOTT_LOCAL;
};

namespace schema {

namespace detail { struct stream_position; }

/**
 * Schema mismatch exception.
 * Thrown when a rule failed to match and could not be backtracked.
 */
struct GOTT_EXPORT mismatch : public tdl_exception {
  GOTT_LOCAL
  mismatch(detail::stream_position const &p, Vector<string> const &);
  ~mismatch() throw();
};

struct GOTT_EXPORT unregistered_type : public tdl_exception {
  unregistered_type(string const &type) GOTT_LOCAL;
  ~unregistered_type() throw();
};

struct GOTT_EXPORT dont_accept : public tdl_exception {
  dont_accept(string const &desc);
  ~dont_accept() throw();
};

}

namespace structure {

struct GOTT_EXPORT failed_repatch : public tdl_exception {
  failed_repatch(string const &desc);
  ~failed_repatch() throw();
};

}
}}

#endif

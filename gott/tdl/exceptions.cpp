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

#include "exceptions.hpp"
#include <gott/range_algo.hpp>
#include <gott/string/string.hpp>

using gott::tdl::tdl_exception;

class tdl_exception::impl {
public:
  impl(string const &s) : len(s.size() + 1), msg(new char[len]) {
    copy(s.as_utf8().cast<char const *>(), msg);
    msg[s.size()] = '\0';
  }
  impl(impl const &o) : len(o.len), msg(new char[len]) {
    copy(range(o.msg, len), msg);
  }
  ~impl() throw() { delete [] msg; }
  std::size_t len;
  char *msg;
};

tdl_exception::tdl_exception(string const &msg)
: p(new impl(msg)) {}

tdl_exception::tdl_exception(tdl_exception const &o)
: std::exception(), p(new impl(*o.p)) {}

tdl_exception::~tdl_exception() throw() {}

char const *tdl_exception::what() const throw() {
  return p->msg;
}

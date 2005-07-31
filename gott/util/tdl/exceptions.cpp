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

using gott::util::tdl::tdl_exception;

tdl_exception::tdl_exception(std::wstring const &msg) : message_wide(msg) {
  std::ostringstream ss;
  ss << message_wide;
  message_narrow = ss.str();
}

tdl_exception::tdl_exception(std::string const &msg) : message_narrow(msg) {
  std::wostringstream ss;
  ss << message_narrow;
  message_wide = ss.str();
}

tdl_exception::~tdl_exception() throw() {}

char const *tdl_exception::what() const throw() {
  return message_narrow.c_str();
}

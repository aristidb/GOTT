// Copyright (C) 2005 by Aristid Breitkreuz (aribrei@arcor.de)
// Content: Text storage and manipulation
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

#ifndef GOTT_UTIL_NSTRING_UTIL_HPP
#define GOTT_UTIL_NSTRING_UTIL_HPP

#include "types.hpp"

namespace gott {

inline std::size_t utf8_len(utf8_t const *s) {
  std::size_t result = 0;
  while (*s++)
    ++result;
  return result;
}

inline std::size_t utf32_len(utf32_t const *s) {
  std::size_t result = 0;
  while (*s++)
    ++result;
  return result;
}

}

#endif

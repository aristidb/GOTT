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

#ifndef GOTT_UTIL_NSTRING_TYPES_HPP
#define GOTT_UTIL_NSTRING_TYPES_HPP

#include <boost/cstdint.hpp>
#include <cstddef>

namespace gott {

typedef uint8_t utf8_t;
typedef uint32_t utf32_t;

enum encoding {
  // Full Unicode
  utf8,
  utf16be, utf16 = utf16be, utf16le,
  utf32be, utf32 = utf32be, utf32le,
  utf7,
  c99,java,
  // European
  ascii,
  iso8859_1, iso8859_2, iso8859_3, iso8859_4, iso8859_5, iso8859_6, iso8859_7,
  iso8859_8, iso8859_9, iso8859_10, iso8859_11, iso8859_12, iso8859_13, 
  iso8859_14, iso8859_15, iso8859_16,
  NO_ENCODING
};

}

#endif

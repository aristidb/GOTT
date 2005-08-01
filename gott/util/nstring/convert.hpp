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

#ifndef GOTT_UTIL_NSTRING_CONVERT_HPP
#define GOTT_UTIL_NSTRING_CONVERT_HPP

#include "types.hpp"

namespace gott {

utf8_t *to_utf8_alloc(char const *, encoding);
utf32_t to_utf32_char(char const *, char const *&, encoding);
void write_utf32_to_utf8(utf32_t, utf8_t *&);
std::size_t utf8_len(char const *, encoding);
std::size_t utf32_len(char const *, encoding);
std::size_t utf8_len(utf32_t);

}

#endif

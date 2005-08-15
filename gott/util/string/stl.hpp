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

#ifndef GOTT_UTIL_NSTRING_STL_HPP
#define GOTT_UTIL_NSTRING_STL_HPP

#include "types.hpp"
#include <gott/util/visibility.hpp>
#include <string>

namespace gott {

class string;

/**
 * Convert a string to a STL-string with a specific encoding.
 * \param str The string to convert.
 * \param enc The encoding of the resulting string.
 * \return The encoded STL-string.
 */
std::string to_string(string const &str, encoding enc = utf8) GOTT_EXPORT;

/**
 * Convert a string to a STL-wide-string with a specific encoding.
 * \param str The string to convert.
 * \param enc The encoding of the resulting string.
 * \return The encoded STL-wide-string.
 */
std::wstring to_wstring(string const &str, encoding enc = utf32) GOTT_EXPORT;

}

#endif

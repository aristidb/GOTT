/*
   Copyright (C) 2005 by Rüdiger Sonderfeld <kingruedi@c-plusplus.de>
   Content: UTF32 compatible strings
   Changes:

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef GOTT_UTIL_UTF32STRING_HPP
#define GOTT_UTIL_UTF32STRING_HPP

#include <string>
#include <boost/cstdint.hpp>

namespace gott {
namespace util {
// #ifdef WCHAR_T_IS_UTF32

  /// UTF-32 Character Type 
  typedef wchar_t utf32_t;
  /// UTF-32 String Class
  typedef ::std::wstring utf32string;
#define GOTT_STR(x) L##x

// #else //wchar_t is not utf32 compatible (e.g. Older Compilers or Windows)
//   typedef ::boost::uint32_t utf32_t;
//   typedef ::std::basic_string<utf32_t> utf32string;
// #define GOTT_STR(x) reinterpret_cast<utf32_t const*>(L##x)
// ...
// #endif //WCHAR_T_IS_UTF32
}}

#endif
